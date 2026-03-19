#include "lowpass.h"

void LowPassFilter::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
    updateCoefficients();
}

void LowPassFilter::setCutoff(float newCutoff)
{
    cutoff = newCutoff;
    updateCoefficients();
}

void LowPassFilter::setQ(float newQ)
{
    q = (newQ > 0.0f) ? newQ : 0.1f;
    updateCoefficients();
}

void LowPassFilter::reset()
{
    x1 = x2 = y1 = y2 = 0.0f;
}

// RBJ Audio EQ Cookbook — second-order lowpass (12 dB/oct)
void LowPassFilter::updateCoefficients()
{
    const float w0    = 2.0f * float(M_PI) * cutoff / float(sampleRate);
    const float cosW0 = std::cos(w0);
    const float alpha = std::sin(w0) / (2.0f * q);

    const float a0inv = 1.0f / (1.0f + alpha);

    b0 = (1.0f - cosW0) * 0.5f * a0inv;
    b1 = (1.0f - cosW0)        * a0inv;
    b2 = b0;
    a1 = -2.0f * cosW0         * a0inv;
    a2 = (1.0f - alpha)        * a0inv;
}

float LowPassFilter::processSample(float signal)
{
    const float out = b0 * signal + b1 * x1 + b2 * x2
                                  - a1 * y1 - a2 * y2;
    x2 = x1;  x1 = signal;
    y2 = y1;  y1 = out;
    return out;
}
