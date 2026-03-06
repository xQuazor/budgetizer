#include "ComplexLFO.h"
#include <cmath>

void ComplexLFO::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
}

void ComplexLFO::setRateSlow(float newRate)   { rateSlow   = (newRate > 0.0f) ? newRate : 0.001f; }
void ComplexLFO::setRateMedium(float newRate) { rateMedium = (newRate > 0.0f) ? newRate : 0.001f; }
void ComplexLFO::setRateFast(float newRate)   { rateFast   = (newRate > 0.0f) ? newRate : 0.001f; }

float ComplexLFO::returnModulation()
{
    float slow   = std::sin(phaseSlow)   * 0.5f;
    float medium = std::sin(phaseMedium) * 0.3f;
    float fast   = std::sin(phaseFast)   * 0.2f;

    float inc = 1.0f / float(sampleRate);
    phaseSlow   += 2.0f * float(M_PI) * rateSlow   * inc;
    phaseMedium += 2.0f * float(M_PI) * rateMedium * inc;
    phaseFast   += 2.0f * float(M_PI) * rateFast   * inc;

    if (phaseSlow   >= 2.0f * float(M_PI)) phaseSlow   -= 2.0f * float(M_PI);
    if (phaseMedium >= 2.0f * float(M_PI)) phaseMedium -= 2.0f * float(M_PI);
    if (phaseFast   >= 2.0f * float(M_PI)) phaseFast   -= 2.0f * float(M_PI);

    return slow + medium + fast; // roughly -1..1
}
