#pragma once

class TriangleLFO
{
public:
    void setSampleRate(double newSampleRate);
    void setRate(float newRate);
    float returnModulation();

private:
    double sampleRate = 44100.0;
    float phase = 0.0f;
    float rate = 0.3f;
};
