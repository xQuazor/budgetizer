#include "MechanicalDrift.h"
#include <cmath>

void MechanicalDrift::prepare (double sr)
{
    sampleRate = sr;
}

void MechanicalDrift::setAmount (float a)
{
    amount = a;
}

float MechanicalDrift::process()
{
    // Random walk with leak
    randomState += (rng.nextFloat() * 2.0f - 1.0f) * 0.01f;
    randomState *= 0.995f;

    // Slow sine LFO at 0.3 Hz
    const float lfoRate = 0.3f;
    lfoPhase += juce::MathConstants<float>::twoPi * lfoRate / (float)sampleRate;
    if (lfoPhase > juce::MathConstants<float>::twoPi)
        lfoPhase -= juce::MathConstants<float>::twoPi;

    // max ±300 Hz drift
    return (randomState + std::sin (lfoPhase) * 0.5f) * 300.0f * amount;
}
