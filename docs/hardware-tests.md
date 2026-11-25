# Hardware test notes

## 0.2.1

MPC One+, MPC OS 3.10.1. Author confirmed manual installation and subsequently installation from OpenMPC Plugin Manager. The earlier engine produced audio but needed improvement in intelligibility and level setup.

## 1.0.0 beta 1 — 2026-11-23

MPC One+, MPC OS 3.10.1. Author reported successful playback, approved the new sound, confirmed chords and dark-voice modes, and specifically highlighted Klar 201. This is subjective author feedback. A later author report measured approximately 34% total project CPU active versus 20% bypassed with Mini D in the project. The difference is approximately 14 percentage points; it is not an isolated plugin benchmark.

## 1.0.0 beta 2

Identical DSP binaries to beta 1, reorganized into five pages with all twenty synthesis bands on one page. The author approved publication of this interface. Installation of the new beta from the OpenMPC repository remains to be confirmed. The project CPU comparison is recorded above.

## Version mapping and packaging patch 1.0.1

The hardware-tested beta engine reports numeric version 1.0.0. Its tested.json entry now uses that value so the catalog can associate the author report. The 1.0.1 patch changes only the compiled release identifier and installer templates, retaining the audio processing and five-page layout. No 1.0.1 device-test entry is claimed before the author tests that installation.
