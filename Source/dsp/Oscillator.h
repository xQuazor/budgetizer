//
// Created by Dovydas Vilkevicius on 12/02/2026.
//

#pragma once
#include <cmath>

class Oscillator
{
public:
    void prepare(double newSampleRate);

    void setFrequency(float newFrequency);
    void setAmplitude(float newAmplitude);

    float getNextSample();

    void reset();

private:
    double sampleRate = 44100.0;

    float frequency = 440.0f;
    float amplitude = 1.0f;

    float phase = 0.0f;
    float phaseIncrement = 0.0f;

    void updatePhaseIncrement();
};