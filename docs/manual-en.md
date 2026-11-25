# Argevox Gerät — 1.0.1 beta 1

Developed by Argerax. Internal plugin version 1.0.1, distributed as beta 1.

[Español](manual-es.md) · [Deutsch](manual-de.md)

## Packaging patch 1.0.1

The audio processing and parameter layout are retained from 1.0.0 beta 2. This patch refreshes the OpenMPC installer templates and release identifiers. The existing hardware test is keyed as 1.0.0, matching that build's numeric manifest version. Installation of 1.0.1 on hardware still needs confirmation.

The author measured the whole project at approximately 34% CPU with Mini D and Argevox active, versus 20% with Argevox bypassed. The difference is approximately 14 percentage points in that project, not an isolated CPU benchmark of the plugin.

## Install and start

Requires MPC OS 3.10.1 or later. Enable Preferences → Enhancements (Beta) → Enable Enhancements and External Plugin Support. For manual installation, extract the manual ZIP and copy `Argerax - VST - Argevox Gerät` into `VST2 Plugins` at the root of your drive. Restart the MPC or rescan external plugins as supported by your device. The per-architecture ZIPs are intended for OpenMPC Plugin Manager.

This beta uses the same plugin identity as 0.2.1 and replaces its installation. Keep the earlier installer and a copy of your projects for comparison. The first 49 parameter identifiers and their order are preserved. The new engine changes the sound, so old input gains and threshold settings may need adjustment. Beta 2 uses the same DSP binaries as beta 1 and reorganizes its interface into five pages.

## Audio routing

Pan the synthesizer or native oscillator fully left, and the microphone fully right. Route both to the same stereo submix and insert Argevox on that submix. Set microphone monitoring to In. L is the carrier and R is the speech modulator. The processed output is centered mono, written to both output channels.

Start with the following comparison

1. Load **Klar 201**. Set Stimme and Träger to 0 dB, synthesis bands to 0 dB, and Entzischung to 0. Hold a low sawtooth note on Mini D with its filter open. A native MPC oscillator can also supply the carrier.
2. Set **Schwelle to −80 dBFS** while checking the signal. This disables gate closure. Then raise the threshold until background noise stops opening the gate, while preserving the beginning of syllables.
3. Speak normally. **Stimme AGC** and **Träger AGC** adjust internal levels with bounded gain. Use Ausgang to set the listening level.
4. Compare **Dunkle Stimme**. Negative Formanten values shift spectral resonances downward while keeping the synthesizer's notes. Dunkelheit progressively reduces upper-band levels.
5. If consonants need help, increase Konsonanten gradually. Klarheit adds a small amount of high-pass-filtered microphone audio during unvoiced sounds. Its initial value is 0, so the first comparison uses vocoder synthesis alone.

## Interface and controls

The parameter names remain German in all language editions of this manual.

| Page | Controls and purpose |
| --- | --- |
| STIMME | Stimme (speech gain), Träger (carrier gain), Konsonanten (automatic noise contribution), Reaktion (envelope response); Schwelle (gate threshold), Entzischung (de-essing), Stimme AGC and Träger AGC; Betriebsart (operating mode), Stimmart (voicing mode), Tonquelle (tonal source), Tonhöhe (internal source frequency). |
| KLANG | Ausgang (output gain), Formanten (formant shift), Dunkelheit (darkness), Klarheit (speech clarity addition); Kontur (spectral contrast), Rosa Rauschen (pink noise), Sprachanteil (filtered speech addition), Pausenpegel (pause-fill level). |
| BÄNDER | All 20 synthesis gains on one page, arranged as two rows of ten, with center frequencies and activity meters. Gain range −18 to +18 dB. |
| SPRACHE | All 20 filtered-speech amounts on one page, arranged as two rows of ten. Used by Multifilter and Sprachanteil. |
| PAUSEN | Pausenmodus, Pausenpegel, Schwelle and Ausgang, followed by ten Pausenpaar controls, one per pair of adjacent bands. |

Each page has at least four assigned Q-Links. Controls are grouped by rows. On BÄNDER and SPRACHE, four groups of four Q-Links control bands 1–8 and 11–18. Bands 9, 10, 19 and 20 use the touch screen. All 20 controls remain visible together. A double tap opens the larger value editor. Knob-bank selection follows the mapped control you touch.

### Operating mode — Betriebsart

- **Vocoder** applies the spectral envelopes to the excitation and adds the selected speech contributions.
- **Multifilter** processes speech through the analysis filters and the 20 Sprache controls. It works without a carrier.
- **Stimme** monitors the conditioned microphone signal for level checks.
- **Träger** monitors the conditioned carrier.

The two monitor modes pass the selected input to the output. Output gain and peak protection remain active.

### Voicing and sources — Stimmart, Tonquelle

- **Automatik** blends tonal and noise excitation according to the spectral balance of speech. Konsonanten sets the automatic noise contribution.
- **Tonhaft** uses tonal excitation.
- **Rauschen** uses noise across the synthesis bank for articulated whispers.
- **Tonquelle** selects Extern L, Säge intern (internal saw), or Puls intern (internal pulse). The internal oscillator has a fixed frequency controlled by Tonhöhe. As an effect insert, the plugin receives no MIDI notes.
- **Rosa Rauschen** replaces white noise with a digital approximation of pink noise.

### Formants, darkness and articulation

**Formanten** ranges from −12 to +12 semitones and remaps the analysis envelopes between synthesis bands. Negative values emphasize lower resonances. **Dunkelheit** reduces upper-band levels progressively. **Kontur** reduces weaker analysis bands relative to dominant ones. **Reaktion** controls envelope attack and release, with shorter settings tracking changes faster. **Entzischung** reduces excessive upper-band energy during unvoiced sounds. **Klarheit** adds filtered real speech and may be left at 0 when evaluating the fully synthesized voice.

### Pause filling — Pausenmodus

Choose **Aus** (off), **Global**, or **Paarweise** (per pair). Global applies Pausenpegel to the whole synthesis bank. Paarweise also applies the ten Pausenpaar amounts. The carrier can intentionally remain audible when the speech detector closes. An active carrier is required. At Schwelle −80, the detector remains open, so pause filling does not engage. Use Aus when checking normal silence behavior.

### Presets

- **Klar 201** — reference articulation, external carrier, darkness 0.
- **Dunkle Stimme** — formants −4 semitones, darkness 45%, external carrier.
- **Tiefe Maschine** — internal pulse at 85 Hz, formants −7, darkness 65%.
- **Flüstern** — articulated pink noise. Initially uses the external source; select an internal source to try it without a synthesizer.
- **Multifilter** — filtered speech with touch-adjustable amounts per band.
- **Pausenbrücke** — global pause filling with a sustained external carrier.

## Troubleshooting

If Vocoder is silent, check both input meters and the L/R routing. Temporarily set Schwelle to −80 and select Extern L when using Mini D. If you only hear a steady note, confirm the microphone is on R and that Pausenmodus is Aus. If the voice sounds dull, start with Klar 201, Entzischung 0, Dunkelheit 0 and flat band gains. Save the successful setup as a preset or MPC project.

## VSM-201 reference and implementation scope

The engine is a functional digital interpretation of the manufacturer's blocks. It includes 20 channels, envelope analysis, AGCs, tonal/noise excitation, automatic voicing selection, filtered-speech addition, multifilter operation, input monitors and global or paired pause filling.

Approximate center frequencies, rounded from the manufacturer's Illustration 8, are 140, 230, 330, 450, 580, 700, 850, 1000, 1170, 1360, 1570, 1830, 2140, 2500, 2900, 3400, 4000, 4800, 5800 and 7500 Hz.

Each analysis and synthesis channel uses three cascaded digital resonators. Rectification is followed by two smoothing stages with band-dependent timing. Modulation precedes the synthesis filters. Voicing detection, AGCs, time constants, pink noise and Q values are approximate digital designs by Argerax. A component-level emulation and a calibrated comparison against a physical VSM-201 have not been performed.

The stereo adaptation has two inputs and two outputs. It does not expose the original's CV connectors, 20 physical envelope outputs, independent unvoiced-excitation input or external hardware connections. Envelopes have on-screen activity meters. Darkness and Klarheit are Argerax extensions.

## Validation and hardware testing

Automated checks cover loading, 93-parameter state, six presets, 32–512-sample blocks, finite output, in-place processing, silence, restoration of the 49 published parameters, monitors, multifilter, articulated noise, pause filling and spectral movement under formant shifting. Both ARM packages pass the current OpenMPC catalog validator without warnings.

The author reported successful beta 1 playback on MPC One+ with MPC OS 3.10.1, including chords and dark voices, and highlighted Klar 201. Beta 2 retains those exact DSP binaries. The project CPU comparison is recorded above. Installation of this packaging patch through OpenMPC remains to be confirmed. Hardware feedback is an author's test report, rather than a calibrated comparison with the original instrument.

## Sources and license

- [Sennheiser original manual and schematics](https://rubidium.se/~magnus/synths/companies/sennheiser/)
- [Illustration 8, channel-filter responses](https://rubidium.se/~magnus/synths/companies/sennheiser/Sennheiser_VSM201_manual_il8_200.jpg)
- [B101–B110, spectral-channel schematic](https://rubidium.se/~magnus/synths/companies/sennheiser/Sennheiser_VSM201_schematic_b101_b110_200.jpg)

Copyright © 2026 Argerax for original contributions. MIT. Reused MPC Plugin Builder SDK components retain their inMusic notices. See LICENSE.
