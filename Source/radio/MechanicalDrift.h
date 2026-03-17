#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class MechanicalDrift
{
public:
    void prepare (double sampleRate);
    void setAmount (float amount);   // 0..1
    float process();                 // returns Hz offset

private:
    double sampleRate  = 44100.0;
    float  amount      = 0.0f;
    float  lfoPhase    = 0.0f;
    float  randomState = 0.0f;
    juce::Random rng;
};
