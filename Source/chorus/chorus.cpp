#include "Chorus.h"

void Chorus::prepare(double newSampleRate, int numChannels)
{
    juce::ignoreUnused(numChannels);
    sampleRate = newSampleRate;

    delayBufferA.setSize(1, maxDelaySamples);
    delayBufferA.clear();
    delayBufferB.setSize(1, maxDelaySamples);
    delayBufferB.clear();

    writePosA = 0;
    writePosB = 0;
    phaseA    = 0.0f;
    phaseB    = float(M_PI);
}

void Chorus::setDelayTimeA(float newDelayTime)
{
    setDelayA  = newDelayTime;
    baseDelayA = newDelayTime;
}

void Chorus::setDelayTimeB(float newDelayTime)
{
    setDelayB  = newDelayTime;
    baseDelayB = newDelayTime;
}

void Chorus::applyTriangleLFO(float lfoValue)
{
    constexpr float triangleModDepth = 0.005f; // ±5 ms
    baseDelayA = setDelayA + lfoValue * triangleModDepth;
    baseDelayB = setDelayB - lfoValue * triangleModDepth; // opposite phase → stereo width
}

float Chorus::processSample(float input, int channel)
{
    if (channel == 0)
    {
        float lfo          = std::sin(phaseA) * depthA;
        float delaySamples = (baseDelayA + lfo) * float(sampleRate);
        delaySamples = std::max(1.0f, std::min(delaySamples, float(maxDelaySamples - 2)));

        delayBufferA.setSample(0, writePosA, input);

        float readPos = float(writePosA) - delaySamples;
        while (readPos < 0.0f) readPos += float(maxDelaySamples);

        int   r0   = int(readPos) % maxDelaySamples;
        int   r1   = (r0 + 1) % maxDelaySamples;
        float frac = readPos - std::floor(readPos);
        float wet  = (1.0f - frac) * delayBufferA.getSample(0, r0)
                   + frac          * delayBufferA.getSample(0, r1);

        writePosA = (writePosA + 1) % maxDelaySamples;
        phaseA   += 2.0f * float(M_PI) * rateA / float(sampleRate);
        if (phaseA >= 2.0f * float(M_PI)) phaseA -= 2.0f * float(M_PI);

        return (1.0f - mix) * input + mix * wet;
    }
    else
    {
        float lfo          = std::sin(phaseB) * depthB;
        float delaySamples = (baseDelayB + lfo) * float(sampleRate);
        delaySamples = std::max(1.0f, std::min(delaySamples, float(maxDelaySamples - 2)));

        delayBufferB.setSample(0, writePosB, input);

        float readPos = float(writePosB) - delaySamples;
        while (readPos < 0.0f) readPos += float(maxDelaySamples);

        int   r0   = int(readPos) % maxDelaySamples;
        int   r1   = (r0 + 1) % maxDelaySamples;
        float frac = readPos - std::floor(readPos);
        float wet  = (1.0f - frac) * delayBufferB.getSample(0, r0)
                   + frac          * delayBufferB.getSample(0, r1);

        writePosB = (writePosB + 1) % maxDelaySamples;
        phaseB   += 2.0f * float(M_PI) * rateB / float(sampleRate);
        if (phaseB >= 2.0f * float(M_PI)) phaseB -= 2.0f * float(M_PI);

        return (1.0f - mix) * input + mix * wet;
    }
}
