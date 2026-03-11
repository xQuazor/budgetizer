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
    void updateCoefficients();

    double sampleRate = 44100.0;
    float cutoff      = 1000.0f;
    float q           = 0.707f;

    // Biquad coefficients (normalised, a0 = 1)
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
    float a1 = 0.0f, a2 = 0.0f;

    // Delay-line state
    float x1 = 0.0f, x2 = 0.0f;
    float y1 = 0.0f, y2 = 0.0f;
};
