#pragma once
#include <cmath>

class LowPassFilter
{
public:
    void prepare(double newSampleRate);
    void setCutoff(float newCutoffHz);
    void reset();

    float processSample(float input);

private:
    void updateCoefficient();

    double sampleRate = 44100.0;
    float cutoff = 1000.0f;

    float a = 0.0f;        // filter coefficient
    float z1 = 0.0f;       // previous output sample
};