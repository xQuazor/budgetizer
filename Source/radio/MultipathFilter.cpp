#include "MultipathFilter.h"
#include <cmath>

void MultipathFilter::prepare(double sr) {
    sampleRate = sr;
}

void MultipathFilter::setMix(float m) {
    mix = m;
}

void MultipathFilter::setBaseDelay(float ms) {
    baseDelayMs = ms;
}

float MultipathFilter::processSample(float x) {
    // Advance LFO
    lfoPhase += lfoRate / (float) sampleRate;
    if (lfoPhase >= 1.f) lfoPhase -= 1.f;
    float lfo = std::sin(lfoPhase * 6.283185f);

    // Modulated delay time in samples
    float delayMs = baseDelayMs + lfoDepth * lfo;
    if (delayMs < 0.1f) delayMs = 0.1f;
    float delaySamples = delayMs * 0.001f * (float) sampleRate;

    // Linear-interpolated read
    int d0 = (int) delaySamples;
    float frac = delaySamples - (float) d0;
    int r0 = (writePos - d0 - 1 + kBufSize) & (kBufSize - 1);
    int r1 = (r0 - 1 + kBufSize) & (kBufSize - 1);
    float delayed = buffer[r0] + frac * (buffer[r1] - buffer[r0]);

    // Write then advance
    buffer[writePos] = x;
    writePos = (writePos + 1) & (kBufSize - 1);

    float finalMix = 0.8;

    if (mix > finalMix) finalMix = mix;

    return x + finalMix * delayed;
}
