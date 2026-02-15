//
// Created by Dovydas Vilkevicius on 12/02/2026.
//

#include "Oscillator.h"

void Oscillator::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    updatePhaseIncrement();
}

void Oscillator::setFrequency(float newFrequency)
{
    frequency = newFrequency;
    updatePhaseIncrement();
}

void Oscillator::setAmplitude(float newAmplitude)
{
    amplitude = newAmplitude;
}

void Oscillator::updatePhaseIncrement()
{
    phaseIncrement = 2.0f * float(M_PI) * frequency / float(sampleRate);
}

float Oscillator::getNextSample()
{
    float output = std::sin(phase) * amplitude;

    phase += phaseIncrement;

    if (phase >= 2.0f * float(M_PI))
        phase -= 2.0f * float(M_PI);

    return output;
}

void Oscillator::reset()
{
    phase = 0.0f;
}