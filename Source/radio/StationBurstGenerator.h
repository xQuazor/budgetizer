#pragma once

class StationBurstGenerator
{
public:
    void prepare (double sampleRate);
    void setCurrentFrequency (float freq);   // updates envelope per sample
    void setDensity (float density);         // 0..1 — widens station proximity threshold
    float process (float input);             // gates input by current envelope

private:
    double sampleRate  = 44100.0;
    float  density     = 0.5f;
    float  envValue    = 0.0f;
    float  attackCoeff = 0.0f;
    float  decayCoeff  = 0.0f;
};
