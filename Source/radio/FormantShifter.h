#pragma once
#include <cmath>
#include "../modulation/ComplexLFO.h"

// Two-grain overlap-add pitch/formant shifter.
// Reads a delay buffer at variable speed; ratio > 1 = formants/pitch up,
// ratio < 1 = down. Triangle windows (50% overlap) satisfy COLA — no clicks.
// ComplexLFO continuously modulates the ratio around the base shift depth.
class FormantShifter {
public:
    void setSampleRate(double sr);

    void setShiftAmount(float amount); // 0 = no shift, 1 = max shift
    float processSample(float input);

private:
    double sampleRate = 44100.0;
    float baseShift = 2.5f;

    static constexpr int BUF_SIZE = 8192;
    static constexpr int BUF_MASK = BUF_SIZE - 1;
    static constexpr int GRAIN_SIZE = 256; // ~11 ms @ 44.1 kHz
    static constexpr int LATENCY = GRAIN_SIZE * 2;

    float buffer[BUF_SIZE] = {};
    int writePos = 0;
    float readPos[2] = {};
    int grainClock = 0;

    ComplexLFO lfo;
};
