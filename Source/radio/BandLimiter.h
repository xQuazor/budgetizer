#pragma once

class BandLimiter
{
public:
    void prepare (double sampleRate);
    float processSample (float input, int channel);   // channel: 0=L, 1=R

private:
    // One-pole LP at 5 kHz (main limiting filter)
    float lpState[2] = { 0.0f, 0.0f };
    float lpCoeff = 0.0f;

    // One-pole LP at 200 Hz — subtracted to form HP
    float hpState[2] = { 0.0f, 0.0f };
    float hpCoeff = 0.0f;
};
