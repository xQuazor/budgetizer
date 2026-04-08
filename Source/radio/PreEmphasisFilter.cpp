#include "PreEmphasisFilter.h"
#include <cmath>

void PreEmphasisFilter::prepare(double sr)
{
    // τ = 75 µs → fc ≈ 2122 Hz
    constexpr float tau = 75e-6f;
    alpha = 1.f - std::exp(-1.f / (tau * (float)sr));
}

void PreEmphasisFilter::setAmount(float a)
{
    amount = a;
}

float PreEmphasisFilter::processSample(float x)
{
    // Subtract LPF output from dry to isolate HF content, then add back scaled.
    // y = x + amount * (x - lpf(x))
    // DC gain = 1, HF gain = 1 + amount (+6 dB at amount=1).
    lpf += alpha * (x - lpf);
    return x + amount * (x - lpf);
}
