#pragma once
#include <cmath>

class LowPassFilter
{
public:
    void setSampleRate(double newSampleRate);
    void setCutoff(float newCutoff);
    void setQ(float newQ);
    void reset();
    float processSample(float signal);

private:
    void updateCoefficient();

    double sampleRate    = 44100.0;
    float cutoff         = 1000.0f;
    float q              = 0.707f;
    float a              = 0.0f;
    float previousSample = 0.0f;
};
