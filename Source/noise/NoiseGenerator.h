#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class NoiseGenerator
{
public:
    void prepare (double sampleRate);
    void setLevel (float level);   // 0..1
    float process();

private:
    juce::Random rng;
    float noiseLevel  = 0.5f;
    float brownAccum  = 0.0f;
};
