# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Degradizer** is a JUCE-based audio effect plugin that degrades audio quality. It builds as VST3 and Standalone formats. The project uses CMake with JUCE as a subdirectory dependency.

## Build Commands

JUCE is expected at `/Users/dovis/Documents/JUCE`. Configure and build via CMake (CLion uses `cmake-build-debug/`):

```bash
# Configure
cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build cmake-build-debug

# The built plugin is auto-copied after build (COPY_PLUGIN_AFTER_BUILD TRUE)
# VST3: cmake-build-debug/MyPlugin_artefacts/Debug/VST3/MyPlugin.vst3
# Standalone: cmake-build-debug/MyPlugin_artefacts/Debug/Standalone/MyPlugin.app
```

There are no automated tests.

## Architecture

### Signal chain (per sample in `processBlock`)
```
Oscillator → LowPassFilter → BitCrusher → Chorus → PitchModulator → output
```

### Modulation hierarchy
```
NoiseLFO → TriangleLFO.rate
NoiseLFO → ComplexLFO.rateSlow / rateMedium / rateFast

TriangleLFO → Chorus.baseDelayA / baseDelayB   (via applyTriangleLFO)
ComplexLFO  → BitCrusher.reductionFactor
ComplexLFO  → PitchModulator.depth              (via applyComplexLFO)
```

All three LFOs advance once per sample before the signal chain runs.

### DSP Components (`Source/`)

| Directory | Class | Description |
|---|---|---|
| `dsp/` | `Oscillator` | Skewed sawtooth oscillator (audio source) |
| `modulation/` | `NoiseLFO` | Smoothed random noise; drives TriangleLFO and ComplexLFO rates |
| `modulation/` | `TriangleLFO` | Triangle wave; modulates Chorus base delay times |
| `modulation/` | `ComplexLFO` | Three summed sines (slow/medium/fast); modulates BitCrusher and PitchModulator |
| `lowpass/` | `LowPassFilter` | One-pole IIR; `setSampleRate` / `setCutoff` / `setQ` / `processSample` |
| `bitcrusher/` | `BitCrusher` | Sample-rate reduction + bit-depth quantisation; `setSampleRate` / `setReductionFactor` / `setBitRate` |
| `chorus/` | `Chorus` | Two-voice stereo chorus (bufferA = ch0, bufferB = ch1) with internal per-voice LFO; `applyTriangleLFO` updates base delays |
| `pitch/` | `PitchModulator` | Variable delay-line pitch modulator; `applyComplexLFO` sets depth |

### Plugin Entry Points

- `PluginProcessor.cpp` — owns all DSP objects; `prepareToPlay` initialises them; `processBlock` runs the modulation hierarchy then signal chain
- `PluginEditor.cpp` — single `bitDepth` knob wired to APVTS (`"bitDepth"` param, range 1–16)

## CMake Notes

- Project CMake name is `MyPlugin`; product name is `MyPlugin`
- C++20 is required
- JUCE path is hardcoded to `/Users/dovydas/CLionProjects/JUCE` — update in `CMakeLists.txt` when working on a different machine
- `Chorus` and `PitchModulator` headers include `<juce_audio_basics/juce_audio_basics.h>` directly for `AudioBuffer<float>`
