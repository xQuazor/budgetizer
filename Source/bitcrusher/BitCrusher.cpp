#include "BitCrusher.h"
#include <cmath>

void BitCrusher::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
}

void BitCrusher::setReductionFactor(int newFactor)
{
    reductionFactor = (newFactor >= 1) ? newFactor : 1;
}

void BitCrusher::setBitRate(int newBitRate)
{
    bitRate = (newBitRate >= 1 && newBitRate <= 32) ? newBitRate : 16;
}

void BitCrusher::setDithering(bool enabled)
{
    dithering = enabled;
}

void BitCrusher::setLinearInterpolation(bool enabled)
{
    linearInterpolation = enabled;
}

void BitCrusher::setPolyBlep(bool enabled)
{
    polyBlepEnabled = enabled;
}

// Polynomial Band-Limited Step function.
// t  : normalised phase [0, 1)
// dt : phase increment per sample (= 1 / reductionFactor)
// Returns a correction value to add to the signal near a step discontinuity.
float BitCrusher::polyBlepFunc(float t, float dt)
{
    if (t < dt)
    {
        t /= dt;
        return t + t - t * t - 1.0f;   // 2t - t² - 1  (post-step rounding)
    }
    else if (t > 1.0f - dt)
    {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;   // t² + 2t + 1  (pre-step rounding)
    }
    return 0.0f;
}

void BitCrusher::reset()
{
    counter    = 0;
    heldSample = 0.0f;
    prevSample = 0.0f;
}

float BitCrusher::processSample(float signal)
{
    // On each new hold period, save the previous target and capture the new one
    if (counter == 0)
    {
        prevSample = heldSample;
        heldSample = signal;
    }

    const float t  = float(counter) / float(reductionFactor);
    const float dt = 1.0f / float(reductionFactor);

    // Base sample: linear ramp or plain hold
    float sample;
    if (linearInterpolation)
        sample = prevSample + t * (heldSample - prevSample);
    else
        sample = heldSample;

    // PolyBLEP: band-limit the step discontinuity at each hold-period boundary
    if (polyBlepEnabled)
    {
        float delta = heldSample - prevSample;
        sample += delta * 0.5f * polyBlepFunc(t, dt);
    }

    counter = (counter + 1) % reductionFactor;

    float levels = std::pow(2.0f, float(bitRate)) - 1.0f;

    if (dithering)
    {
        // TPDF dither: two uniform values summed → triangular distribution, amplitude ±1 LSB
        float r1 = (float(rand()) / float(RAND_MAX)) * 2.0f - 1.0f;
        float r2 = (float(rand()) / float(RAND_MAX)) * 2.0f - 1.0f;
        sample += (r1 + r2) * 0.5f / levels;
    }

    return std::round(sample * levels) / levels;
}
