#include "lowpass.h"

void LowPassFilter::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    updateCoefficient();
    reset();
}

void LowPassFilter::setCutoff(float newCutoffHz)
{
    cutoff = newCutoffHz;
    updateCoefficient();
}

void LowPassFilter::reset()
{
    z1 = 0.0f;
}

void LowPassFilter::updateCoefficient()
{
    // One-pole lowpass coefficient
    float x = std::exp(-2.0f * float(M_PI) * cutoff / float(sampleRate));
    a = 1.0f - x;
}

float LowPassFilter::processSample(float input)
{
    z1 = a * input + (1.0f - a) * z1;
    return z1;
}