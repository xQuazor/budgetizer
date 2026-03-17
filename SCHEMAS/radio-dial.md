# Radio Shuffle DSP Plugin (JUCE) – Implementation Plan

## Overview

This project implements a **Lo-Fi Radio Tuning Effect** as an audio plugin using the JUCE framework.

The effect recreates the sound of turning a vintage radio dial between stations:

- Static noise
- Frequency sweeping
- Brief station fragments
- AM-style distortion
- Band-limited radio tone
- Mechanical tuning instability

The plugin is designed as a modular DSP chain so each radio characteristic can be tuned independently.

---

## Project Structure

```
RadioShufflePlugin/
│
├── Source/
│   ├── PluginProcessor.cpp
│   ├── PluginProcessor.h
│   ├── PluginEditor.cpp
│   ├── PluginEditor.h
│
│   ├── dsp/
│   │   ├── RadioTuner.h / .cpp
│   │   ├── NoiseGenerator.h / .cpp
│   │   ├── StationBurstGenerator.h / .cpp
│   │   ├── SweepFilter.h / .cpp
│   │   ├── AMDetector.h / .cpp
│   │   └── MechanicalDrift.h / .cpp
│
│   └── util/
│       ├── RandomWalk.h
│       └── Envelope.h
│
└── CMakeLists.txt
```

---

## DSP Signal Chain

```
Input
  ↓
Noise Generator
  ↓
Station Burst Generator
  ↓
Sweeping Bandpass Filter
  ↓
AM Distortion / Detection
  ↓
LoFi Band Limiting
  ↓
Output
```

---

## DSP Modules

### 1. Noise Generator

Simulates radio static between stations.

**Responsibilities**
- Generate white or pink noise
- Provide adjustable amplitude
- Allow modulation based on station strength

**Interface**

```cpp
class NoiseGenerator
{
public:
    void prepare(double sampleRate);
    float process();

private:
    float noiseLevel;
};
```

**Implementation**

```cpp
float NoiseGenerator::process()
{
    return randomFloat(-1.0f, 1.0f) * noiseLevel;
}
```

---

### 2. Frequency Sweep Engine

Controls tuning dial motion.

**Behavior**

The sweep simulates rotating a radio knob across frequencies:

```
freq = startFreq + (endFreq - startFreq) * sweepPosition
```

`sweepPosition` is driven by:
- Automation
- Envelope trigger
- Random jitter

**Interface**

```cpp
class RadioTuner
{
public:
    void startSweep(float startFreq, float endFreq, float timeMs);
    float getCurrentFrequency();

private:
    float currentFreq;
};
```

---

### 3. Mechanical Drift Simulation

Real radios do not tune perfectly.

**Add:**
- Slow LFO drift
- Random walk jitter

**Algorithm**

```
freq += randomWalk * driftAmount
freq += sin(lfoPhase) * lfoDepth
```

**Interface**

```cpp
class MechanicalDrift
{
public:
    float process();

private:
    float randomState;
};
```

---

### 4. Sweeping Bandpass Filter

Represents the radio tuning circuit.

**Parameters**

| Parameter | Value |
|---|---|
| `centerFrequency` | 200 Hz – 6000 Hz |
| `bandwidth` | ~3 kHz |
| `Q` | 3 – 8 |

**JUCE Implementation**

Use `juce::dsp::StateVariableTPTFilter`:

```cpp
filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
filter.setCutoffFrequency(freq);
```

---

### 5. Station Burst Generator

Simulates passing radio stations.

**Behavior**

When the tuning frequency crosses a station:
- Short envelope burst
- Inject audio fragment

**Detection**

```
if abs(currentFreq - stationFreq) < threshold
    triggerBurst()
```

**Envelope**

| Stage | Duration |
|---|---|
| Attack | 5 ms |
| Decay | 50 ms |

**Interface**

```cpp
class StationBurstGenerator
{
public:
    float process(float input);

private:
    Envelope burstEnv;
};
```

---

### 6. AM Detection / Distortion

Classic AM radio distortion.

**Rectification**

```cpp
output = abs(signal);
output = lowpass(output, 3kHz);
```

---

### 7. Band Limiting

Vintage radios have limited bandwidth.

**Typical Range**

| Filter | Frequency |
|---|---|
| Highpass | 200 Hz |
| Lowpass | 4–6 kHz |

**JUCE filters:** `juce::dsp::IIR::Filter`
---

## Plugin Parameters

| Parameter | Range | Description |
|---|---|---|
| Tune Speed | 50 ms – 1000 ms | Sweep speed |
| Static Amount | 0 – 1 | Noise level |
| Drift | 0 – 1 | Frequency instability |
| Burst Density | 0 – 1 | Station probability |
| Bandwidth | 2 – 5 kHz | Radio bandwidth |
| LoFi Amount | 0 – 1 | Bit/sample degradation |

---

## Example Processing Loop

```cpp
for each sample
{
    noise = noiseGen.process();

    freq = tuner.getCurrentFrequency();
    freq += drift.process();

    bandSignal = bandpass.process(input + noise, freq);

    station = burstGen.process(stationAudio);

    radio = bandSignal + station;

    distorted = tanh(radio * drive);

    output = bandLimit.process(distorted);
}
```

---


## Advanced Enhancements

### FFT Spectral Station Simulation

Instead of noise:
1. Generate multiple narrowband carriers
2. Randomly place them across spectrum
3. Sweep through them

This produces extremely realistic station passing artifacts.

### Impulse Clicks

Add clicks when crossing station thresholds:

```
if crossingDetected:
    output += impulse * clickLevel
```




