#include "TripleLFO.h"

#include <cmath>

void TripleLFO::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
}

void TripleLFO::setFrequencies(float slowHz, float mediumHz, float fastHz)
{
    slowFreq = slowHz;
    mediumFreq = mediumHz;
    fastFreq = fastHz;
}

void TripleLFO::setAmount(float newAmount)
{
    amount = newAmount;
}

float TripleLFO::getNextSample()
{
    // Fast LFO (small influence)
    float fast = std::sin(fastPhase) * 0.1f;

    // Medium LFO modulated slightly by fast
    float mediumModFreq = mediumFreq * (1.0f + fast);
    float medium = std::sin(mediumPhase) * 0.3f;

    // Slow LFO modulated by medium
    float slowModFreq = slowFreq * (1.0f + medium);
    float slow = std::sin(slowPhase);

    // Advance phases with modulated frequencies
    slowPhase += phaseInc(slowModFreq);
    mediumPhase += phaseInc(mediumModFreq);
    fastPhase += phaseInc(fastFreq);

    wrap(slowPhase);
    wrap(mediumPhase);
    wrap(fastPhase);

    return slow * amount;
}