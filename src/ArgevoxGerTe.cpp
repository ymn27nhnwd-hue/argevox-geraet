// SPDX-License-Identifier: MIT
// L: carrier. R: speech. Parallel analysis and synthesis bands feed centered output.
#include "mpc_plugin.h"
#include "dsp/biquad.h"
#include "dsp/smoother.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <string>

namespace {

using mpc::dsp::Biquad;
using mpc::dsp::BiquadCoeffs;
using mpc::dsp::Smoother;

constexpr int bandCount = 20;

float gain(float db) { return std::pow(10.0f, db / 20.0f); }

float follow(float state, float target, float attack, float release)
{
    return state + (target - state) * (target > state ? attack : release);
}

struct Band
{
    Biquad analysis, synthesis;
    float envelope = 0.0f;
    Smoother gain;
    float sibilanceWeight = 0.0f;
};

class Argevox : public mpc::Plugin
{
    enum { Output, Voice, Carrier, Consonants, Response, CarrierMeter, VoiceMeter, FirstBand, Threshold = FirstBand + bandCount, DeEss, FirstBandGain };

public:
    Argevox() : mpc::Plugin(makeInfo())
    {
        auto output = mpc::floatParam("output", "Ausgang", -30, 6, 0, "dB", 1);
        output.automatable = false;
        addParam(output);
        addParam(mpc::floatParam("voice", "Stimme", -24, 24, 6, "dB", 1));
        addParam(mpc::floatParam("carrier", "Träger", -24, 18, 0, "dB", 1));
        addParam(mpc::floatParam("consonants", "Konsonanten", 0, 100, 35, "%", 0));
        addParam(mpc::floatParam("response", "Reaktion", 0, 100, 28, "%", 0));
        addMeter("carrier_meter", "OSZ L");
        addMeter("voice_meter", "MIK R");
        for (int band = 0; band < bandCount; ++band)
            addMeter("band_" + std::to_string(band + 1), "Band " + std::to_string(band + 1));
        addParam(mpc::floatParam("threshold", "Schwelle", -80, 0, -48, "dBFS", 0));
        addParam(mpc::floatParam("deess", "Entzischung", 0, 6, 3, "dB", 1));
        for (int band = 0; band < bandCount; ++band)
        {
            const std::string suffix = std::to_string(band + 1);
            addParam(mpc::floatParam(("band_gain_" + suffix).c_str(), ("Band " + suffix).c_str(),
                                     -18, 18, 0, "dB", 1));
        }
        addPreset("Klassisch", 35, 28);
        addPreset("Roboter", 24, 8);
        addPreset("Weich", 45, 73);
    }

    void prepare(double rate, int) override
    {
        sampleRate_ = rate;
        const float top = std::min(10000.0f, static_cast<float>(rate * 0.4));
        for (int band = 0; band < bandCount; ++band)
        {
            const float frequency = 120.0f * std::pow(top / 120.0f, (band + 0.5f) / bandCount);
            const auto coeffs = BiquadCoeffs::bandpass(rate, frequency, 4.4f);
            bands_[band].analysis.setCoeffs(coeffs);
            bands_[band].synthesis.setCoeffs(coeffs);
            bands_[band].gain.setTime(15.0f, rate);
            bands_[band].sibilanceWeight = std::clamp((frequency - 3200.0f) / 2800.0f, 0.0f, 1.0f);
        }
        speechHigh_.setCoeffs(BiquadCoeffs::highpass(rate, 3100.0f));
        deEssHigh_.setCoeffs(BiquadCoeffs::highpass(rate, 5200.0f));
        gateAttack_ = timeCoefficient(2.0f);
        gateRelease_ = timeCoefficient(20.0f);
        gateDetectorRelease_ = timeCoefficient(36.0f);
        deEssAttack_ = timeCoefficient(2.0f);
        deEssRelease_ = timeCoefficient(65.0f);
        noiseHigh_.setCoeffs(BiquadCoeffs::highpass(rate, 2300.0f));
        fadeUp_ = timeCoefficient(3.0f);
        fadeDown_ = timeCoefficient(26.0f);
        meterDecay_ = timeCoefficient(125.0f);
        for (Smoother* control : {&voiceGain_, &carrierGain_, &consonants_, &outputGain_})
            control->setTime(12.0f, rate);
    }

    void reset() override
    {
        for (auto& band : bands_)
        {
            band.analysis.reset();
            band.synthesis.reset();
            band.envelope = 0.0f;
        }
        for (int band = 0; band < bandCount; ++band)
            bands_[band].gain.snap(gain(get(FirstBandGain + band)));
        speechHigh_.reset();
        deEssHigh_.reset();
        gateLevel_ = gateGain_ = deEssAmount_ = deEssPeak_ = deEssWide_ = 0.0f;
        gateOpen_ = false;
        noiseHigh_.reset();
        speechLevel_ = highSpeechLevel_ = carrierLevel_ = 0.0f;
        voicePeak_ = carrierPeak_ = 0.0f;
        meterFrames_ = 0;
        noiseState_ = 0x6d2b79f5u;
        voiceGain_.snap(gain(get(Voice)));
        carrierGain_.snap(gain(get(Carrier)));
        consonants_.snap(get(Consonants) * 0.01f);
        outputGain_.snap(gain(get(Output)));
    }

    void process(mpc::ProcessContext& ctx) override
    {
        voiceGain_.setTarget(gain(get(Voice)));
        carrierGain_.setTarget(gain(get(Carrier)));
        consonants_.setTarget(get(Consonants) * 0.01f);
        outputGain_.setTarget(gain(get(Output)));

        for (int band = 0; band < bandCount; ++band)
            bands_[band].gain.setTarget(gain(get(FirstBandGain + band)));

        const float thresholdDb = get(Threshold);
        const float gateThreshold = gain(thresholdDb);
        const float gateCloseThreshold = gateThreshold * 0.63f;
        const float maxDeEss = 1.0f - gain(-get(DeEss));
        const float response = get(Response) * 0.01f;
        const float attack = timeCoefficient(2.0f + 24.0f * response);
        const float release = timeCoefficient(12.0f + 150.0f * response * response);
        const float* carrierInput = ctx.numInputs > 0 ? ctx.inputs[0] : nullptr;
        const float* voiceInput = ctx.numInputs > 1 ? ctx.inputs[1] : nullptr;

        for (int frame = 0; frame < ctx.numFrames; ++frame)
        {
            const float carrier = carrierInput ? carrierInput[frame] : 0.0f;
            const float voice = voiceInput ? voiceInput[frame] : 0.0f;
            voicePeak_ = follow(voicePeak_, std::abs(voice), 1.0f, meterDecay_);
            carrierPeak_ = follow(carrierPeak_, std::abs(carrier), 1.0f, meterDecay_);

            // Detect the gate from the raw mic, before the Stimme gain control.
            gateLevel_ = follow(gateLevel_, std::abs(voice), 1.0f, gateDetectorRelease_);
            gateOpen_ = thresholdDb <= -80.0f || gateLevel_ >= (gateOpen_ ? gateCloseThreshold : gateThreshold);
            gateGain_ = follow(gateGain_, gateOpen_ ? 1.0f : 0.0f, gateAttack_, gateRelease_);
            if (gateGain_ < 0.0001f) gateGain_ = 0.0f;

            // Detect sibilance, then attenuate only the upper bands and consonant noise.
            const float high = deEssHigh_.process(voice);
            deEssPeak_ = follow(deEssPeak_, std::abs(high), deEssAttack_, deEssRelease_);
            deEssWide_ = follow(deEssWide_, std::abs(voice), deEssAttack_, deEssRelease_);
            const float sibilant = std::clamp((deEssPeak_ - 0.35f * deEssWide_) /
                                             (0.55f * deEssWide_ + 0.002f), 0.0f, 1.0f);
            deEssAmount_ = follow(deEssAmount_, maxDeEss * sibilant, deEssAttack_, deEssRelease_);
            const float speech = voice * voiceGain_.process();
            const float synth = carrier * carrierGain_.process();
            float wet = 0.0f;
            for (auto& band : bands_)
            {
                const float level = std::abs(band.analysis.process(speech));
                band.envelope = follow(band.envelope, level, attack, release);
                wet += band.synthesis.process(synth) * band.envelope * band.gain.process()
                     * (1.0f - deEssAmount_ * band.sibilanceWeight);
            }

            speechLevel_ = follow(speechLevel_, std::abs(speech), fadeUp_, fadeDown_);
            highSpeechLevel_ = follow(highSpeechLevel_, std::abs(speechHigh_.process(speech)), fadeUp_, fadeDown_);
            carrierLevel_ = follow(carrierLevel_, std::abs(synth), fadeUp_, fadeDown_);
            const float sibilance = std::max(0.0f, highSpeechLevel_ - 0.22f * speechLevel_);
            const float excitation = std::min(1.0f, 6.0f * carrierLevel_);
            wet += 0.28f * consonants_.process() * excitation * sibilance * (1.0f - deEssAmount_) * noiseHigh_.process(noise());

            const float output = 7.5f * outputGain_.process() * wet * gateGain_;
            const float limited = output / (1.0f + std::abs(output));
            for (int channel = 0; channel < ctx.numOutputs; ++channel)
                ctx.outputs[channel][frame] = limited;
        }

        meterFrames_ += ctx.numFrames;
        const int meterPeriod = static_cast<int>(sampleRate_ / 24.0);
        if (meterFrames_ >= meterPeriod)
        {
            meterFrames_ %= meterPeriod;
            set(CarrierMeter, meterValue(carrierPeak_, 1.0f));
            set(VoiceMeter, meterValue(voicePeak_, 1.0f));
            for (int band = 0; band < bandCount; ++band)
                set(FirstBand + band, meterValue(bands_[band].envelope, 4.0f));
        }
    }

private:
    void addPreset(const char* name, float consonants, float response)
    {
        std::vector<float> values(numParams(), 0.0f);
        values[Voice] = 6.0f;
        values[Consonants] = consonants;
        values[Response] = response;
        values[Threshold] = -48.0f;
        values[DeEss] = 3.0f;
        addProgram(name, std::move(values));
    }

    static mpc::PluginInfo makeInfo()
    {
        mpc::PluginInfo info;
        info.name = "Argevox Gerät";
        info.vendor = "Argerax";
        info.uniqueId = MPC_FOURCC('A', 'w', 'p', 't');
        info.version = 210;
        info.isInstrument = false;
        info.numInputs = 2;
        info.numOutputs = 2;
        info.tailSamples = 0;
        return info;
    }

    void addMeter(const std::string& id, const std::string& label)
    {
        auto meter = mpc::floatParam(id.c_str(), label.c_str(), 0, 1, 0);
        meter.automatable = false;
        addParam(meter);
    }

    float timeCoefficient(float ms) const
    {
        return 1.0f - std::exp(-1000.0f / (ms * static_cast<float>(sampleRate_)));
    }

    static float meterValue(float value, float scale)
    {
        return std::sqrt(std::min(1.0f, value * scale));
    }

    float noise()
    {
        noiseState_ ^= noiseState_ << 13;
        noiseState_ ^= noiseState_ >> 17;
        noiseState_ ^= noiseState_ << 5;
        return static_cast<float>(noiseState_ >> 8) * (2.0f / 16777216.0f) - 1.0f;
    }

    std::array<Band, bandCount> bands_{};
    Biquad speechHigh_, noiseHigh_, deEssHigh_;
    float gateAttack_ = 0.0f, gateRelease_ = 0.0f, gateDetectorRelease_ = 0.0f;
    float gateLevel_ = 0.0f, gateGain_ = 0.0f;
    bool gateOpen_ = false;
    float deEssAttack_ = 0.0f, deEssRelease_ = 0.0f;
    float deEssPeak_ = 0.0f, deEssWide_ = 0.0f, deEssAmount_ = 0.0f;
    Smoother voiceGain_, carrierGain_, consonants_, outputGain_;
    double sampleRate_ = 44100.0;
    float fadeUp_ = 0.0f, fadeDown_ = 0.0f, meterDecay_ = 0.0f;
    float speechLevel_ = 0.0f, highSpeechLevel_ = 0.0f, carrierLevel_ = 0.0f;
    float voicePeak_ = 0.0f, carrierPeak_ = 0.0f;
    int meterFrames_ = 0;
    std::uint32_t noiseState_ = 0x6d2b79f5u;
};

} // namespace

MPC_PLUGIN_ENTRY(Argevox)




