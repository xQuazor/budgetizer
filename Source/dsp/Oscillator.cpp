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
    float normalisedPhase = phase / (2.0f * float(M_PI));
    float skew = 0.3f; // 0..1

    float output;

    if (normalisedPhase < skew)
        output = normalisedPhase / skew;
    else
        output = 1.0f - (normalisedPhase - skew) / (1.0f - skew);

    output = 2.0f * output - 1.0f; // scale to -1..1

    // Apply amplitude
    float finalOutput = output * amplitude;

    // Advance phase
    phase += phaseIncrement;

    if (phase >= 2.0f * float(M_PI))
        phase -= 2.0f * float(M_PI);

    return finalOutput;
}

void Oscillator::reset()
{
    phase = 0.0f;
}