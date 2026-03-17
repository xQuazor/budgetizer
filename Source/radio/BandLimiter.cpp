#include "BandLimiter.h"
#include <cmath>

void BandLimiter::prepare (double sampleRate)
{
    const float pi = 3.14159265f;
    // one-pole LP coefficients
    lpCoeff = 1.0f - std::exp (-2.0f * pi * 5000.0f / (float)sampleRate);
    hpCoeff = 1.0f - std::exp (-2.0f * pi *  200.0f / (float)sampleRate);
}

float BandLimiter::processSample (float input, int ch)
{
    // LP at 5 kHz
    lpState[ch] += lpCoeff * (input    - lpState[ch]);
    // LP at 200 Hz — subtract to get highpass character
    hpState[ch] += hpCoeff * (lpState[ch] - hpState[ch]);
    // Result: band 200 Hz – 5 kHz
    return lpState[ch] - hpState[ch];
}
