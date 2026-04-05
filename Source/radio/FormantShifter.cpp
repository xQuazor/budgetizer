#include "FormantShifter.h"

void FormantShifter::setSampleRate (double sr)
{
    sampleRate = sr;
    lfo.setSampleRate(sr);
    lfo.setRateSlow   (0.09f);
    lfo.setRateMedium (0.31f);
    lfo.setRateFast   (0.77f);

    writePos     = 0;
    grainClock   = 0;
    // Position both read heads LATENCY samples behind write, staggered by half a grain
    readPos[0]   = float(BUF_SIZE - LATENCY);
    readPos[1]   = float(BUF_SIZE - LATENCY + GRAIN_SIZE / 2);
}

void FormantShifter::setShiftAmount (float amount)
{
    baseShift = amount < 0.0f ? 0.0f : (amount > 1.0f ? 1.0f : amount);
}

float FormantShifter::processSample (float input)
{
    // Write
    buffer[writePos] = input;
    writePos = (writePos + 1) & BUF_MASK;

    // Compute pitch ratio: LFO sweeps ±50% around 1.0, scaled by depth
    float mod   = lfo.returnModulation();               // -1..1
    float ratio = 1.0f + baseShift * 0.5f * mod;
    if (ratio < 0.5f) ratio = 0.5f;
    if (ratio > 2.0f) ratio = 2.0f;

    // Advance grain clock; reset each grain's read head when its window hits zero
    grainClock = (grainClock + 1) & (GRAIN_SIZE - 1);

    // At the zero-crossing of each grain's window the contribution is 0 — safe to jump
    if (grainClock == 0)
        readPos[0] = float((writePos - LATENCY + BUF_SIZE) & BUF_MASK);
    if (grainClock == GRAIN_SIZE / 2)
        readPos[1] = float((writePos - LATENCY + BUF_SIZE) & BUF_MASK);

    // Advance read heads at pitch ratio
    readPos[0] += ratio;
    readPos[1] += ratio;
    if (readPos[0] >= BUF_SIZE) readPos[0] -= BUF_SIZE;
    if (readPos[1] >= BUF_SIZE) readPos[1] -= BUF_SIZE;

    // Triangle windows — 50% overlap satisfies COLA: win0 + win1 == 1 at all times
    float phase0 = float(grainClock) / GRAIN_SIZE;
    float phase1 = float((grainClock + GRAIN_SIZE / 2) & (GRAIN_SIZE - 1)) / GRAIN_SIZE;
    float win0   = (phase0 < 0.5f) ? phase0 * 2.0f : (1.0f - phase0) * 2.0f;
    float win1   = (phase1 < 0.5f) ? phase1 * 2.0f : (1.0f - phase1) * 2.0f;

    // Linear interpolated reads
    auto readInterp = [&](float pos) -> float {
        int   i = int(pos) & BUF_MASK;
        float f = pos - float(int(pos));
        return buffer[i] * (1.0f - f) + buffer[(i + 1) & BUF_MASK] * f;
    };

    return win0 * readInterp(readPos[0]) + win1 * readInterp(readPos[1]);
}
