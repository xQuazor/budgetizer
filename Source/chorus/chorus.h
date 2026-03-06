//
// Created by Dovydas Vilkevicius on 02/03/2026.
//
#include "vector"
#ifndef MYPLUGIN_CHORUS_H
#define MYPLUGIN_CHORUS_H


class Chorus
{
public:
    void prepare(double sampleRate, int channels);
    float processSample(float input, int channel);

    void setRate(float r) { rate = r; }
    void setDepth(float d) { depth = d; }
    void setMix(float m) { mix = m; }

private:
    juce::AudioBuffer<float> delayBuffer;

    std::vector<ChorusVoice> voices;

    int writePos = 0;
    double sampleRate = 44100.0;

    float rate = 0.3f;
    float depth = 0.004f;
    float baseDelay = 0.015f;

    float mix = 0.5f;
};

#endif //MYPLUGIN_CHORUS_H