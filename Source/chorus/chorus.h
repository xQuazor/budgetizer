#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

class Chorus
{
public:
    void prepare(double newSampleRate, int numChannels);
    void setDelayTimeA(float newDelayTime);
    void setDelayTimeB(float newDelayTime);
    void applyTriangleLFO(float lfoValue);
    float processSample(float input, int channel);

private:
    juce::AudioBuffer<float> delayBufferA;
    juce::AudioBuffer<float> delayBufferB;

    float rateA = 0.4f;
    float rateB = 0.5f;

    float depthA = 0.003f;
    float depthB = 0.003f;

    float baseDelayA = 0.015f;
    float baseDelayB = 0.020f;

    float setDelayA = 0.015f;
    float setDelayB = 0.020f;

    float phaseA = 0.0f;
    float phaseB = float(M_PI); // opposite phase for stereo spread

    int writePosA = 0;
    int writePosB = 0;

    double sampleRate = 44100.0;
    float mix = 0.5f;

    static constexpr int maxDelaySamples = 8192;
};
