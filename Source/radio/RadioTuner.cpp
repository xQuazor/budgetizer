#include "RadioTuner.h"

void RadioTuner::prepare (double sr)
{
    sampleRate = sr;
    setSweepTime (500.0f);
}

void RadioTuner::setSweepTime (float timeMs)
{
    sweepDelta = 1.0f / (float)(sampleRate * (double)(timeMs / 1000.0f));
}

float RadioTuner::getCurrentFrequency() const
{
    return kStartFreq + (kEndFreq - kStartFreq) * sweepPosition;
}

void RadioTuner::advance()
{
    sweepPosition += sweepDelta * direction;
    if (sweepPosition >= 1.0f) { sweepPosition = 1.0f; direction = -1.0f; }
    if (sweepPosition <= 0.0f) { sweepPosition = 0.0f; direction =  1.0f; }
}
