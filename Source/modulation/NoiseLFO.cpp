#include "NoiseLFO.h"
#include <cstdlib>
#include <cmath>

void NoiseLFO::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
    updateSmoothing();
}

void NoiseLFO::setRate(float newRate)
{
    rate = (newRate > 0.0f) ? newRate : 0.001f;
    updateSmoothing();
}

void NoiseLFO::updateSmoothing()
{
    smoothCoeff = 1.0f - std::exp(-2.0f * float(M_PI) * rate / float(sampleRate));
}

float NoiseLFO::returnModulation()
{
    phase += rate / float(sampleRate);
    if (phase >= 1.0f)
    {
        phase -= 1.0f;
        targetValue = (float(rand()) / float(RAND_MAX)) * 2.0f - 1.0f;
        updateSmoothing();
    }

    currentValue += smoothCoeff * (targetValue - currentValue);
    return currentValue;
}
