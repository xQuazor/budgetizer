#pragma once

// First-order FM pre-emphasis (75 µs time constant, US standard).
// Boosts high-frequency content above ~2.1 kHz to mimic the characteristic
// brightness of an FM signal received without receiver de-emphasis.
class PreEmphasisFilter
{
public:
    void  prepare(double sampleRate);
    void  setAmount(float amount);  // 0 = bypass, 1 = full FM pre-emphasis
    float processSample(float x);

private:
    float alpha  = 0.f;  // 1-pole LPF coefficient
    float lpf    = 0.f;  // LPF state
    float amount = 1.0f;
};
