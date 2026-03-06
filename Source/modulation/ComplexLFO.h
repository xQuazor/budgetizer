#pragma once
#include <cmath>

class ComplexLFO
{
public:
    void setSampleRate(double newSampleRate);
    void setRateSlow(float newRate);
    void setRateMedium(float newRate);
    void setRateFast(float newRate);
    float returnModulation();

private:
    double sampleRate = 44100.0;
    float phase = 0.0f; // kept for schema compatibility; actual state split below
    float rateSlow   = 0.1f;
    float rateMedium = 0.5f;
    float rateFast   = 2.0f;

    float phaseSlow   = 0.0f;
    float phaseMedium = 0.0f;
    float phaseFast   = 0.0f;
};
