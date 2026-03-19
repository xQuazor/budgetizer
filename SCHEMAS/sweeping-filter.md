
Morphing Filter Bank – JUCE Implementation Plan

Overview

This module implements a morphing filter bank for a JUCE audio plugin.

Instead of sweeping a single bandpass filter, the system creates a bank of narrow bandpass filters distributed across the frequency spectrum.
A control parameter called SweepPosition crossfades between neighbouring filters.

This creates smoother transitions and avoids zipper artifacts typical of moving filter coefficients.

Primary goals:
•	Smooth frequency sweep
•	Station-like peaks
•	Stable DSP performance
•	Low CPU overhead
•	Good modulation behaviour

⸻

High-Level Architecture

Signal Flow:

Input
↓
Filter Bank (N bandpass filters)
↓
Weighting / Morphing System
↓
Weighted Summation
↓
Output

Each filter contributes to the output based on its proximity to the sweep position.

⸻

Core Concepts

Filter Bank

A bank of N bandpass filters spaced across the frequency range.

Typical configuration:

Filter Count: 16–32
Frequency Range: 200 Hz – 6000 Hz
Spacing: Logarithmic
Q Factor: 5–10

Logarithmic spacing better matches human perception and radio tuning behavior.

Example frequencies:

200 Hz
320 Hz
500 Hz
800 Hz
1200 Hz
1800 Hz
2700 Hz
4000 Hz
6000 Hz


⸻

Data Structures

Filter Node

struct FilterNode
{
float centerFreq;
float weight;

    juce::dsp::StateVariableTPTFilter<float> filter;
};


⸻

Filter Bank Container

class MorphingFilterBank
{
public:
void prepare(double sampleRate, int maxBlockSize);
void setSweepPosition(float position);
float process(float input);

private:
std::vector<FilterNode> filters;

    float sweepPosition;
    double sampleRate;
};


⸻

Initialization

Filter Creation

Filters should be created during prepare().

for i in 0..N
{
freq = logSpace(minFreq, maxFreq, i / N)

    filter.setType(BANDPASS)
    filter.setCutoffFrequency(freq)
    filter.setResonance(Q)

    store in filter bank
}


⸻

Logarithmic Frequency Spacing

Use logarithmic interpolation:

freq = minFreq * pow(maxFreq / minFreq, t)

Where:

t = i / (N - 1)

This ensures filters are evenly spaced perceptually.

⸻

Sweep Position

SweepPosition ranges:

0 → start of filter bank
1 → end of filter bank

Convert to filter index:

float index = sweepPosition * (N - 1)

Example:

index = 4.2

Meaning:

filter 4 weight ≈ 0.8
filter 5 weight ≈ 0.2


⸻

Weight Calculation

Crossfade between nearest filters.

i0 = floor(index)
i1 = i0 + 1

w1 = index - i0
w0 = 1 - w1

Set weights:

filters[i0].weight = w0
filters[i1].weight = w1

All others:

weight = 0


⸻

Processing Algorithm

Per sample:

output = 0

for each filter
{
filtered = filter.processSample(input)

    output += filtered * weight
}

Return summed result.

⸻

Optional: Smooth Weighting

Instead of linear crossfade, use a Gaussian window:

weight = exp(-distance² / width)

Distance:

distance = abs(filterIndex - sweepIndex)

This produces smoother spectral motion.

⸻

Analog Drift

Add instability to center frequencies.

freq += randomWalk * driftAmount

Update coefficients periodically.

Example:

every 32 samples


⸻

Station Simulation

Randomly boost certain filters.

if random < stationProbability
gain *= stationBoost

This produces the feeling of passing radio stations.

⸻

Performance Optimizations

Block Processing

Use block processing where possible:

processBlock(AudioBlock<float>)

Coefficient Updates

Avoid updating filter coefficients every sample.

Instead:

update every 16–64 samples

SIMD Potential

Filter bank processing can be SIMD optimized later.

⸻

Parameter Mapping

Parameter	Range	Description
Sweep	0–1	Current sweep position
Resonance	1–12	Filter Q
Filter Count	8–32	Number of filters
Drift	0–1	Frequency instability
Width	0–1	Crossfade width


⸻

Example Processing Loop

for sample in buffer
{
index = sweepPosition * (numFilters - 1)

    i0 = floor(index)
    i1 = i0 + 1

    w1 = index - i0
    w0 = 1 - w1

    output =
        filters[i0].process(input) * w0 +
        filters[i1].process(input) * w1
}
