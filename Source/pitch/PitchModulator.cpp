#include "PitchModulator.h"
#include <cmath>

void PitchModulator::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    delayBuffer.setSize(1, bufferSize);
    delayBuffer.clear();
    writePos = 0;
}

void PitchModulator::setDelayTime(float newDelayTime)
{
    setBaseDelay = newDelayTime;
    baseDelay    = newDelayTime;
}

void PitchModulator::applyComplexLFO(float lfoValue)
{
    depth = lfoValue * maxDepth;
}

float PitchModulator::processSample(float input)
{
    delayBuffer.setSample(0, writePos % bufferSize, input);

    float effectiveDelay = baseDelay + depth;
    effectiveDelay = std::max(0.001f, std::min(effectiveDelay, float(bufferSize - 2) / float(sampleRate)));

    float readPos = float(writePos) - effectiveDelay * float(sampleRate);
    while (readPos < 0.0f) readPos += float(bufferSize);
    readPos = std::fmod(readPos, float(bufferSize));

    int   r0   = int(readPos) % bufferSize;
    int   r1   = (r0 + 1) % bufferSize;
    float frac = readPos - std::floor(readPos);

    float output = (1.0f - frac) * delayBuffer.getSample(0, r0)
                 + frac          * delayBuffer.getSample(0, r1);

    ++writePos;
    return output;
}
