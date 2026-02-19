#pragma once
#include <cmath>

class TripleLFO
{
public:
    void prepare(double newSampleRate);

    void setFrequencies(float slowHz, float mediumHz, float fastHz);
    void setAmount(float amount); // overall modulation depth

    float getNextSample();

private:
    double sampleRate = 44100.0;

    float slowFreq = 0.2f;
    float mediumFreq = 1.0f;
    float fastFreq = 5.0f;

    float slowPhase = 0.0f;
    float mediumPhase = 0.0f;
    float fastPhase = 0.0f;

    float amount = 1.0f;

    float phaseInc(float freq) const
    {
        return 2.0f * float(M_PI) * freq / float(sampleRate);
    }

    void wrap(float& phase)
    {
        if (phase >= 2.0f * float(M_PI))
            phase -= 2.0f * float(M_PI);
    }
};
