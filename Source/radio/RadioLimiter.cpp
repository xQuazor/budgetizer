#include "RadioLimiter.h"
#include <cmath>
#include <cstdlib>

void RadioLimiter::prepare(double sr)
{
    attackCoeff  = std::exp(-1.0 / (0.001 * sr));  // ~1 ms attack
    releaseCoeff = std::exp(-1.0 / (0.050 * sr));  // ~50 ms release
}

void RadioLimiter::setThreshold(float t)
{
    threshold = t;
}

float RadioLimiter::processSample(float x)
{
    float absX = x < 0.f ? -x : x;

    // Asymmetric peak follower: fast attack, slow release
    if (absX > envelope)
        envelope = attackCoeff  * envelope + (1.f - attackCoeff)  * absX;
    else
        envelope = releaseCoeff * envelope + (1.f - releaseCoeff) * absX;

    // Target gain: unity below threshold, reduce above it
    float targetGain = (envelope > threshold) ? threshold / envelope : 1.f;

    // Smooth gain to avoid zippering
    gainSmooth += 0.01f * (targetGain - gainSmooth);

    return x * gainSmooth;
}
