// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Argerax. SDK DSP primitives retain their inMusic notices.
// L carrier / R microphone -> analysis -> envelopes -> excitation selection ->
// per-channel modulation BEFORE synthesis filters -> speech addition -> peak protection.
// Functional digital interpretation of the VSM-201 manual, not a component-exact model.
#include "mpc_plugin.h"
#include "dsp/biquad.h"
#include "dsp/smoother.h"
#include "dsp/oscillator.h"
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
// Approximate centres read from the manufacturer's Illustration 8 (K1-K20).
// These are rounded graph readings, not precision component-derived frequencies.
constexpr std::array<float, bandCount> centres = {
    140, 230, 330, 450, 580, 700, 850, 1000, 1170, 1360,
    1570, 1830, 2140, 2500, 2900, 3400, 4000, 4800, 5800, 7500
};
float gain(float db) { return std::pow(10.0f, db / 20.0f); }
float follow(float state, float target, float attack, float release)
{
    return state + (target - state) * (target > state ? attack : release);
}
struct FilterBankChannel {
    std::array<Biquad, 3> analysis, synthesis;
    float envelope = 0, envelopeLowpass = 0;
    float attack = 0, release = 0, deEssWeight = 0;
    Smoother level, speechMix;
    void reset()
    {
        for (auto& f : analysis) f.reset();
        for (auto& f : synthesis) f.reset();
        envelope = envelopeLowpass = 0;
    }
};
class Argevox : public mpc::Plugin {
    // Published indices 0-48 are permanent. All additions are appended.
    enum { Output, Voice, Carrier, Consonants, Response, CarrierMeter, VoiceMeter,
           FirstBand, Threshold = FirstBand + bandCount, DeEss, FirstBandGain,
           Mode = FirstBandGain + bandCount, VoiceMode, Formant, Darkness, Clarity,
           VoiceAgc, CarrierAgc, PinkNoise, Source, Pitch, SpeechMix, PauseMix,
           PauseMode, Contour, FirstSpeechBand, FirstPausePair = FirstSpeechBand + bandCount,
           ParamCount = FirstPausePair + 10 };
public:
    Argevox() : mpc::Plugin(makeInfo())
    {
        auto output = mpc::floatParam("output", "Ausgang", -30, 6, 0, "dB", 1);
        output.automatable = false;
        addParam(output);
        addParam(mpc::floatParam("voice", "Stimme", -24, 24, 0, "dB", 1));
        addParam(mpc::floatParam("carrier", "Träger", -24, 18, 0, "dB", 1));
        addParam(mpc::floatParam("consonants", "Konsonanten", 0, 100, 55, "%", 0));
        addParam(mpc::floatParam("response", "Reaktion", 0, 100, 12, "%", 0));
        addMeter("carrier_meter", "OSZ L");
        addMeter("voice_meter", "MIK R");
        for (int i = 0; i < bandCount; ++i)
            addMeter("band_" + std::to_string(i + 1), "Band " + std::to_string(i + 1));
        addParam(mpc::floatParam("threshold", "Schwelle", -80, 0, -58, "dBFS", 0));
        addParam(mpc::floatParam("deess", "Entzischung", 0, 6, 0, "dB", 1));
        for (int i = 0; i < bandCount; ++i)
            addParam(mpc::floatParam(("band_gain_" + std::to_string(i + 1)).c_str(),
                                    ("Band " + std::to_string(i + 1)).c_str(), -18, 18, 0, "dB", 1));
        addParam(mpc::choiceParam("mode", "Betriebsart", {"Vocoder", "Multifilter", "Stimme", "Träger"}));
        addParam(mpc::choiceParam("voice_mode", "Stimmart", {"Automatik", "Tonhaft", "Rauschen"}));
        addParam(mpc::floatParam("formant", "Formanten", -12, 12, 0, "HT", 1));
        addParam(mpc::floatParam("darkness", "Dunkelheit", 0, 100, 0, "%", 0));
        addParam(mpc::floatParam("clarity", "Klarheit", 0, 100, 0, "%", 0));
        addParam(mpc::boolParam("voice_agc", "Stimme AGC", true));
        addParam(mpc::boolParam("carrier_agc", "Träger AGC", true));
        addParam(mpc::boolParam("pink_noise", "Rosa Rauschen", false));
        addParam(mpc::choiceParam("source", "Tonquelle", {"Extern L", "Säge intern", "Puls intern"}));
        addParam(mpc::floatParam("pitch", "Tonhöhe", 45, 440, 110, "Hz", 1));
        addParam(mpc::floatParam("speech_mix", "Sprachanteil", 0, 100, 0, "%", 0));
        addParam(mpc::floatParam("pause_mix", "Pausenpegel", 0, 100, 0, "%", 0));
        addParam(mpc::choiceParam("pause_mode", "Pausenmodus", {"Aus", "Global", "Paarweise"}));
        addParam(mpc::floatParam("contour", "Kontur", 0, 100, 30, "%", 0));
        for (int i = 0; i < bandCount; ++i)
            addParam(mpc::floatParam(("speech_band_" + std::to_string(i + 1)).c_str(),
                ("Sprache " + std::to_string(i + 1)).c_str(), 0, 100, 100, "%", 0));
        for (int i = 0; i < 10; ++i)
            addParam(mpc::floatParam(("pause_pair_" + std::to_string(i + 1)).c_str(),
                ("Pausenpaar " + std::to_string(i + 1)).c_str(), 0, 100, 100, "%", 0));
        addPreset("Klar 201", 0, 0, 0, 0, 0);
        addPreset("Dunkle Stimme", -4, 45, 0, 0, 0);
        addPreset("Tiefe Maschine", -7, 65, 0, 2, 0);
        addPreset("Flüstern", 0, 10, 2, 0, 0);
        addPreset("Multifilter", 0, 0, 0, 0, 1);
        addPreset("Pausenbrücke", 0, 20, 0, 0, 0, 20);
    }
    void prepare(double rate, int) override
    {
        sampleRate_ = rate;
        for (int i = 0; i < bandCount; ++i) {
            // Three resonators per bank, as a functional six-pole approximation.
            // Match overlap to local spacing rather than use one Q for every channel.
            const float lo = i == 0 ? 90.0f : centres[i - 1];
            const float hi = i == bandCount - 1 ? 9500.0f : centres[i + 1];
            const float q = std::clamp(1.02f * centres[i] / (hi - lo), 0.65f, 4.5f);
            const auto c = BiquadCoeffs::bandpass(rate, centres[i], q);
            for (auto& f : bands_[i].analysis) f.setCoeffs(c);
            for (auto& f : bands_[i].synthesis) f.setCoeffs(c);
            bands_[i].deEssWeight = std::clamp((centres[i] - 3000.0f) / 3000.0f, 0.0f, 1.0f);
            bands_[i].level.setTime(15, rate);
            bands_[i].speechMix.setTime(15, rate);
        }
        voiceHighpass_.setCoeffs(BiquadCoeffs::highpass(rate, 70));
        voicePresence_.setCoeffs(BiquadCoeffs::highShelf(rate, 1500, 0.7071f, 4));
        clarityHighpass_.setCoeffs(BiquadCoeffs::highpass(rate, 2300));
        lowVoice_.setCoeffs(BiquadCoeffs::lowpass(rate, 800));
        highVoice_.setCoeffs(BiquadCoeffs::highpass(rate, 4000));
        oscillator_.setSampleRate(rate);
        for (auto* s : {&voiceGain_, &carrierGain_, &outputGain_, &consonants_, &clarity_,
                        &speechMix_, &darkness_, &pauseMix_, &contour_, &sourceMix_})
            s->setTime(15, rate);
        for (auto& s : pausePairs_) s.setTime(20, rate);
        detectorAttack_ = coefficient(1);
        detectorRelease_ = coefficient(60);
        agcAttack_ = coefficient(4);
        agcRelease_ = coefficient(180);
        gateAttack_ = coefficient(1);
        gateRelease_ = coefficient(18);
        classify_ = coefficient(4);
        limiterRelease_ = coefficient(70);
        meterDecay_ = coefficient(125);
        for (int i = 0; i < 8; ++i) {
            const float cutoff = 60.0f * std::pow(2.0f, static_cast<float>(i));
            pinkCoefficients_[i] = 1 - std::exp(-6.2831853f * cutoff / static_cast<float>(rate));
            pinkWeights_[i] = std::sqrt(60.0f / cutoff);
        }
        updateFormantMap(get(Formant));
    }
    void reset() override
    {
        for (int i = 0; i < bandCount; ++i) {
            bands_[i].reset();
            bands_[i].level.snap(gain(get(FirstBandGain + i)));
            bands_[i].speechMix.snap(get(FirstSpeechBand + i) * 0.01f);
        }
        for (int i = 0; i < 10; ++i) pausePairs_[i].snap(get(FirstPausePair + i) * 0.01f);
        voiceHighpass_.reset(); voicePresence_.reset(); clarityHighpass_.reset();
        lowVoice_.reset(); highVoice_.reset(); oscillator_.resetPhase();
        voiceGain_.snap(gain(get(Voice))); carrierGain_.snap(gain(get(Carrier)));
        outputGain_.snap(gain(get(Output))); consonants_.snap(get(Consonants) * 0.01f);
        clarity_.snap(get(Clarity) * 0.01f); speechMix_.snap(get(SpeechMix) * 0.01f);
        darkness_.snap(get(Darkness) * 0.01f); pauseMix_.snap(get(PauseMix) * 0.01f);
        contour_.snap(get(Contour) * 0.01f); sourceMix_.snap(getInt(Source) == 0 ? 0.0f : 1.0f);
        voicePeak_ = carrierPeak_ = rawVoiceLevel_ = voiceLevel_ = carrierLevel_ = 0;
        lowLevel_ = highLevel_ = noiseBlend_ = gateGain_ = 0;
        pinkStates_.fill(0);
        limiterGain_ = 1; noiseState_ = 0x6d2b79f5u; gateOpen_ = false; meterFrames_ = 0;
        formantValue_ = 1000;
    }
    void process(mpc::ProcessContext& ctx) override
    {
        voiceGain_.setTarget(gain(get(Voice))); carrierGain_.setTarget(gain(get(Carrier)));
        outputGain_.setTarget(gain(get(Output))); consonants_.setTarget(get(Consonants) * 0.01f);
        clarity_.setTarget(get(Clarity) * 0.01f); speechMix_.setTarget(get(SpeechMix) * 0.01f);
        darkness_.setTarget(get(Darkness) * 0.01f); pauseMix_.setTarget(get(PauseMix) * 0.01f);
        contour_.setTarget(get(Contour) * 0.01f);
        const int mode = getInt(Mode), voiceMode = getInt(VoiceMode), pauseMode = getInt(PauseMode);
        const int source = getInt(Source);
        sourceMix_.setTarget(source == 0 ? 0.0f : 1.0f);
        oscillator_.setWave(source == 2 ? mpc::dsp::Wave::Square : mpc::dsp::Wave::Saw);
        oscillator_.setFrequency(get(Pitch));
        const bool voiceAgc = getBool(VoiceAgc), carrierAgc = getBool(CarrierAgc), pink = getBool(PinkNoise);
        const float threshold = get(Threshold), thresholdAmplitude = gain(threshold);
        const float deEss = 1.0f - gain(-get(DeEss));
        const float response = get(Response) * 0.01f;
        const float highRelease = coefficient(18);
        updateFormantMap(get(Formant));
        for (int i = 0; i < bandCount; ++i) {
            bands_[i].level.setTarget(gain(get(FirstBandGain + i)));
            bands_[i].speechMix.setTarget(get(FirstSpeechBand + i) * 0.01f);
            bands_[i].attack = coefficient(std::max(0.6f, 1600.0f / centres[i]) + 5 * response);
            bands_[i].release = coefficient(std::max(8.0f, 6500.0f / centres[i]) + 100 * response * response);
        }
        for (int i = 0; i < 10; ++i) pausePairs_[i].setTarget(get(FirstPausePair + i) * 0.01f);
        const float* left = ctx.numInputs > 0 ? ctx.inputs[0] : nullptr;
        const float* right = ctx.numInputs > 1 ? ctx.inputs[1] : nullptr;
        for (int frame = 0; frame < ctx.numFrames; ++frame) {
            const float mic = sanitize(right ? right[frame] : 0);
            const float external = sanitize(left ? left[frame] : 0);
            voicePeak_ = follow(voicePeak_, std::abs(mic), 1, meterDecay_);
            carrierPeak_ = follow(carrierPeak_, std::abs(external), 1, meterDecay_);
            rawVoiceLevel_ = follow(rawVoiceLevel_, std::abs(mic), 1, detectorRelease_);
            gateOpen_ = threshold <= -80 || rawVoiceLevel_ >= thresholdAmplitude * (gateOpen_ ? 0.63f : 1.0f);
            gateGain_ = follow(gateGain_, gateOpen_ ? 1.0f : 0.0f, gateAttack_, gateRelease_);
            float speech = voiceHighpass_.process(mic) * voiceGain_.process();
            const float internalMix = sourceMix_.process();
            float carrier = (external * (1 - internalMix) + 0.35f * oscillator_.process() * internalMix)
                          * carrierGain_.process();
            voiceLevel_ = follow(voiceLevel_, std::abs(speech), agcAttack_, agcRelease_);
            carrierLevel_ = follow(carrierLevel_, std::abs(carrier), agcAttack_, agcRelease_);
            // Bounded AGCs. Silence cannot turn into an arbitrarily amplified noise floor.
            speech *= voiceAgc ? std::clamp(0.18f / (voiceLevel_ + 0.004f), 0.06f, 4.0f) : 1;
            carrier *= carrierAgc ? std::clamp(0.22f / (carrierLevel_ + 0.004f), 0.06f, 4.0f) : 1;
            speech = sanitize(speech); carrier = sanitize(carrier);
            const float low = std::abs(lowVoice_.process(speech));
            const float high = std::abs(highVoice_.process(speech));
            lowLevel_ = follow(lowLevel_, low, detectorAttack_, detectorRelease_);
            highLevel_ = follow(highLevel_, high, detectorAttack_, highRelease);
            const float unvoiced = std::clamp((highLevel_ - lowLevel_ * 0.35f) /
                                             (highLevel_ + lowLevel_ * 0.65f + 0.0003f), 0.0f, 1.0f);
            const float desiredNoise = voiceMode == 2 ? 1.0f : (voiceMode == 1 ? 0.0f : unvoiced);
            noiseBlend_ += classify_ * (desiredNoise - noiseBlend_);
            const float consonants = consonants_.process();
            const float carrierAmplitude = carrierAgc ? std::min(0.35f, 0.22f * carrierLevel_ /
                                                     (carrierLevel_ + 0.004f)) : std::min(1.0f, carrierLevel_);
            const float white = noise();
            float pinkSample = 0;
            for (int i = 0; i < 8; ++i) {
                pinkStates_[i] += pinkCoefficients_[i] * (white - pinkStates_[i]);
                pinkSample += pinkStates_[i] * pinkWeights_[i];
            }
            const float hiss = (pink ? 3.0f * pinkSample : white) * carrierAmplitude * 1.73f;
            // The unvoiced carrier is analysed by the same synthesis bank, rather than
            // adding a broadband hiss above the finished vocoder output.
            const float tonalMix = pauseMode != 0 ? 1.0f : 1.0f - noiseBlend_ * consonants;
            const float excitation = voiceMode == 2 ? hiss : carrier * tonalMix + hiss * noiseBlend_ * consonants;
            const float analysed = voicePresence_.process(speech);
            for (int i = 0; i < bandCount; ++i) {
                float value = analysed;
                for (auto& f : bands_[i].analysis) value = f.process(value);
                analysisSamples_[i] = value;
                bands_[i].envelope = follow(bands_[i].envelope, std::abs(value), bands_[i].attack, bands_[i].release);
                bands_[i].envelopeLowpass = follow(bands_[i].envelopeLowpass, bands_[i].envelope,
                                                  bands_[i].attack, bands_[i].release);
            }
            float vocoded = 0, filteredSpeech = 0;
            const float dark = darkness_.process(), contour = contour_.process();
            const float pause = pauseMode == 0 ? 0.0f : pauseMix_.process() * (1 - gateGain_);
            // Advance pause smoother even when disabled.
            if (pauseMode == 0) pauseMix_.process();
            for (int pair = 0; pair < 10; ++pair) pairValues_[pair] = pausePairs_[pair].process();
            for (int i = 0; i < bandCount; ++i) {
                const auto& map = formantMap_[i];
                const float e = bands_[map.first].envelopeLowpass * (1 - map.fraction) +
                                bands_[map.second].envelopeLowpass * map.fraction;
                float control = e * 22.0f * map.edge;
                control *= (1 - contour) + contour * control / (control + 0.12f);
                control = std::min(control, 3.0f) * gateGain_;
                const float pair = pauseMode == 2 ? pairValues_[i / 2] : 1;
                control += 0.55f * pause * pair;
                float value = excitation * control;
                for (auto& f : bands_[i].synthesis) value = f.process(value);
                const float level = bands_[i].level.process();
                const float shade = 1.0f / (1 + dark * 3.5f * centres[i] / 1600.0f);
                const float ess = 1 - deEss * unvoiced * bands_[i].deEssWeight;
                vocoded += value * level * shade * ess;
                filteredSpeech += analysisSamples_[i] * bands_[i].speechMix.process();
            }
            const float clear = clarityHighpass_.process(speech) * clarity_.process() * 0.4f *
                                unvoiced * gateGain_ * (1 - deEss * unvoiced);
            const float addition = filteredSpeech * speechMix_.process() * gateGain_;
            float result = 1.8f * vocoded + addition + clear;
            if (mode == 1) result = filteredSpeech * gateGain_;
            if (mode == 2) result = speech;
            if (mode == 3) result = carrier;
            result *= outputGain_.process();
            // Unity below the ceiling. No always-active waveshaping of articulation.
            const float wanted = std::min(1.0f, 0.94f / (std::abs(result) + 1.0e-9f));
            limiterGain_ = follow(limiterGain_, wanted, 1, limiterRelease_);
            result = std::clamp(result * limiterGain_, -0.94f, 0.94f);
            if (std::abs(result) < 1.0e-20f) result = 0;
            for (int ch = 0; ch < ctx.numOutputs; ++ch) ctx.outputs[ch][frame] = result;
        }
        meterFrames_ += ctx.numFrames;
        const int period = std::max(1, static_cast<int>(sampleRate_ / 24));
        if (meterFrames_ >= period) {
            meterFrames_ %= period;
            set(CarrierMeter, meter(carrierPeak_)); set(VoiceMeter, meter(voicePeak_));
            for (int i = 0; i < bandCount; ++i) set(FirstBand + i, meter(bands_[i].envelopeLowpass * 8));
        }
    }
private:
    struct FormantMap { int first = 0, second = 0; float fraction = 0, edge = 1; };
    void updateFormantMap(float semitones)
    {
        if (semitones == formantValue_) return;
        formantValue_ = semitones;
        const float ratio = std::pow(2.0f, -semitones / 12.0f);
        for (int i = 0; i < bandCount; ++i) {
            const float f = centres[i] * ratio;
            int upper = 1;
            while (upper < bandCount - 1 && centres[upper] < f) ++upper;
            auto& m = formantMap_[i];
            m.first = upper - 1; m.second = upper;
            m.fraction = std::clamp(std::log(f / centres[m.first]) /
                                    std::log(centres[m.second] / centres[m.first]), 0.0f, 1.0f);
            m.edge = std::min({1.0f, f / centres.front(), centres.back() / f});
        }
    }
    void addPreset(const char* name, float formant, float dark, int voiceMode, int source, int mode, float pause = 0)
    {
        std::vector<float> v(numParams(), 0);
        v[Output] = -3; v[Voice] = 0; v[Consonants] = 55; v[Response] = 12;
        v[Threshold] = -58; v[DeEss] = 0; v[VoiceAgc] = v[CarrierAgc] = 1;
        v[Formant] = formant; v[Darkness] = dark; v[VoiceMode] = static_cast<float>(voiceMode);
        v[Source] = static_cast<float>(source); v[Mode] = static_cast<float>(mode);
        v[Pitch] = 85; v[Contour] = 30; v[PauseMix] = pause; v[PauseMode] = pause > 0 ? 1 : 0;
        v[PinkNoise] = voiceMode == 2 ? 1 : 0;
        for (int i = FirstSpeechBand; i < ParamCount; ++i) v[i] = 100;
        addProgram(name, std::move(v));
    }
    static mpc::PluginInfo makeInfo()
    {
        mpc::PluginInfo p;
        p.name = "Argevox Gerät"; p.vendor = "Argerax";
        p.uniqueId = MPC_FOURCC('A','w','p','t'); p.version = 10000;
        p.isInstrument = false; p.numInputs = p.numOutputs = 2; p.tailSamples = 0;
        return p;
    }
    void addMeter(const std::string& id, const std::string& name)
    {
        auto p = mpc::floatParam(id.c_str(), name.c_str(), 0, 1, 0);
        p.automatable = false; addParam(p);
    }
    float coefficient(float ms) const { return 1 - std::exp(-1000.0f / (ms * static_cast<float>(sampleRate_))); }
    static float sanitize(float x) { return std::isfinite(x) ? std::clamp(x, -8.0f, 8.0f) : 0; }
    static float meter(float x) { return std::sqrt(std::clamp(x, 0.0f, 1.0f)); }
    float noise()
    {
        noiseState_ ^= noiseState_ << 13; noiseState_ ^= noiseState_ >> 17; noiseState_ ^= noiseState_ << 5;
        return static_cast<float>(noiseState_ >> 8) * (2.0f / 16777216.0f) - 1;
    }
    std::array<FilterBankChannel, bandCount> bands_{};
    std::array<FormantMap, bandCount> formantMap_{};
    std::array<float, bandCount> analysisSamples_{};
    std::array<Smoother, 10> pausePairs_{};
    std::array<float, 10> pairValues_{};
    Biquad voiceHighpass_, voicePresence_, clarityHighpass_, lowVoice_, highVoice_;
    mpc::dsp::PolyBlepOsc oscillator_;
    Smoother voiceGain_, carrierGain_, outputGain_, consonants_, clarity_, speechMix_;
    Smoother darkness_, pauseMix_, contour_, sourceMix_;
    double sampleRate_ = 44100;
    float formantValue_ = 1000;
    float detectorAttack_ = 0, detectorRelease_ = 0, agcAttack_ = 0, agcRelease_ = 0;
    float gateAttack_ = 0, gateRelease_ = 0, classify_ = 0, limiterRelease_ = 0, meterDecay_ = 0;
    std::array<float, 8> pinkStates_{}, pinkCoefficients_{}, pinkWeights_{};
    float voicePeak_ = 0, carrierPeak_ = 0;
    float rawVoiceLevel_ = 0, voiceLevel_ = 0, carrierLevel_ = 0, lowLevel_ = 0, highLevel_ = 0;
    float noiseBlend_ = 0, gateGain_ = 0, limiterGain_ = 1;
    bool gateOpen_ = false;
    int meterFrames_ = 0;
    std::uint32_t noiseState_ = 0x6d2b79f5u;
};
}
MPC_PLUGIN_ENTRY(Argevox)
