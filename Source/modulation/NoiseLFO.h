#pragma once

class NoiseLFO
{
public:
    void setSampleRate(double newSampleRate);
    void setRate(float newRate);
    float returnModulation();

private:
    double sampleRate = 44100.0;
    float phase = 0.0f;
    float rate = 0.5f;

    float currentValue = 0.0f;
    float targetValue  = 0.0f;
    float smoothCoeff  = 0.01f;

    void updateSmoothing();
};
