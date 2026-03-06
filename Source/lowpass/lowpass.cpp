#include "lowpass.h"

void LowPassFilter::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
    updateCoefficient();
}

void LowPassFilter::setCutoff(float newCutoff)
{
    cutoff = newCutoff;
    updateCoefficient();
}

void LowPassFilter::setQ(float newQ)
{
    q = (newQ > 0.0f) ? newQ : 0.1f;
}

void LowPassFilter::reset()
{
    previousSample = 0.0f;
}

void LowPassFilter::updateCoefficient()
{
    float x = std::exp(-2.0f * float(M_PI) * cutoff / float(sampleRate));
    a = 1.0f - x;
}

float LowPassFilter::processSample(float signal)
{
    previousSample = a * signal + (1.0f - a) * previousSample;
    return previousSample;
}
