#include "TriangleLFO.h"

void TriangleLFO::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
}

void TriangleLFO::setRate(float newRate)
{
    rate = (newRate > 0.0f) ? newRate : 0.001f;
}

float TriangleLFO::returnModulation()
{
    float output;
    if (phase < 0.5f)
        output = 4.0f * phase - 1.0f;   // -1 → 1
    else
        output = 3.0f - 4.0f * phase;   //  1 → -1

    phase += rate / float(sampleRate);
    if (phase >= 1.0f) phase -= 1.0f;

    return output;
}
