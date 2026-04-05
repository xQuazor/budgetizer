#include "PitchShifter.h"

void PitchShifter::setSampleRate (double sr)
{
    sampleRate = sr;
    writePos        = 0;
    grainClock      = 0;
    grainReadPos[0] = float(BUF_SIZE - LATENCY);
    grainReadPos[1] = float(BUF_SIZE - LATENCY + GRAIN_SIZE / 2);
    tsReadPos       = float(BUF_SIZE - LATENCY);
    tsFadeFromPos   = 0.0f;
    tsCrossfadeCount = 0;
}

void PitchShifter::setMode (Mode m)  { mode  = m; }

void PitchShifter::setRatio (float r)
{
    ratio = r < 0.25f ? 0.25f : (r > 4.0f ? 4.0f : r);
}

// ── shared helper ──────────────────────────────────────────────────────────────

float PitchShifter::readInterp (float pos) const
{
    int   i = int(pos) & BUF_MASK;
    float f = pos - float(int(pos));
    return buffer[i] * (1.0f - f) + buffer[(i + 1) & BUF_MASK] * f;
}

// ── Granular mode (two-grain OLA) ─────────────────────────────────────────────

float PitchShifter::processGranular (float input)
{
    buffer[writePos] = input;
    writePos = (writePos + 1) & BUF_MASK;

    grainClock = (grainClock + 1) & (GRAIN_SIZE - 1);

    if (grainClock == 0)
        grainReadPos[0] = float((writePos - LATENCY + BUF_SIZE) & BUF_MASK);
    if (grainClock == GRAIN_SIZE / 2)
        grainReadPos[1] = float((writePos - LATENCY + BUF_SIZE) & BUF_MASK);

    grainReadPos[0] += ratio;
    grainReadPos[1] += ratio;
    if (grainReadPos[0] >= BUF_SIZE) grainReadPos[0] -= BUF_SIZE;
    if (grainReadPos[1] >= BUF_SIZE) grainReadPos[1] -= BUF_SIZE;

    float phase0 = float(grainClock) / GRAIN_SIZE;
    float phase1 = float((grainClock + GRAIN_SIZE / 2) & (GRAIN_SIZE - 1)) / GRAIN_SIZE;
    float win0   = (phase0 < 0.5f) ? phase0 * 2.0f : (1.0f - phase0) * 2.0f;
    float win1   = (phase1 < 0.5f) ? phase1 * 2.0f : (1.0f - phase1) * 2.0f;

    return win0 * readInterp(grainReadPos[0]) + win1 * readInterp(grainReadPos[1]);
}

// ── TimeStretch mode ──────────────────────────────────────────────────────────
//
// Single read head advancing at `ratio` speed — reads faster than write = pitch up,
// slower = pitch down. At 0.5 / 1.0 / 2.0 / 3.0 every harmonic partial scales by
// the same factor so the harmonic series stays coherent.
//
// When the read-write distance drifts out of the safe window, a 64-sample linear
// crossfade repositions the read head to avoid a hard click.

float PitchShifter::processTimeStretch (float input)
{
    buffer[writePos] = input;
    writePos = (writePos + 1) & BUF_MASK;

    // Distance = how many samples behind the write head we are
    int distance = (writePos - int(tsReadPos) + BUF_SIZE) & BUF_MASK;

    if (distance < MIN_DISTANCE || distance > MAX_DISTANCE)
    {
        tsFadeFromPos    = tsReadPos;
        tsReadPos        = float((writePos - LATENCY + BUF_SIZE) & BUF_MASK);
        tsCrossfadeCount = CROSSFADE_LEN;
    }

    float output = readInterp(tsReadPos);

    if (tsCrossfadeCount > 0)
    {
        float fade = float(tsCrossfadeCount) / CROSSFADE_LEN;   // 1→0
        output = output * (1.0f - fade) + readInterp(tsFadeFromPos) * fade;

        tsFadeFromPos += ratio;
        if (tsFadeFromPos >= BUF_SIZE) tsFadeFromPos -= BUF_SIZE;
        --tsCrossfadeCount;
    }

    tsReadPos += ratio;
    if (tsReadPos >= BUF_SIZE) tsReadPos -= BUF_SIZE;

    return output;
}

// ── dispatch ──────────────────────────────────────────────────────────────────

float PitchShifter::processSample (float input)
{
    return (mode == Mode::TimeStretch) ? processTimeStretch(input)
                                       : processGranular(input);
}
