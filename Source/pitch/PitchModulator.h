#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

class PitchModulator
{
public:
    void prepare(double newSampleRate);
    void setDelayTime(float newDelayTime);
    void applyComplexLFO(float lfoValue);
    float processSample(float input);

private:
    juce::AudioBuffer<float> delayBuffer;

    float rate      = 0.5f;
    float depth     = 0.0f;
    float baseDelay = 0.02f;

    float setBaseDelay = 0.02f;
    float maxDepth     = 0.002f; // 10 ms pitch modulation range

    double sampleRate = 44100.0;
    int writePos      = 0;

    static constexpr int bufferSize = 8192;
};
