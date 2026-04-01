#include "BitCrusher.h"
#include <cmath>

// ── Setup ─────────────────────────────────────────────────────────────────────

void BitCrusher::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    errorChorus.prepare(newSampleRate, 1);
    computeOversamplingCoeffs();
}

void BitCrusher::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
}
void BitCrusher::setReductionFactor(int newFactor) { reductionFactor = (newFactor >= 1) ? newFactor : 1; }
void BitCrusher::setBitRate(int newBitRate)         { bitRate = (newBitRate >= 1 && newBitRate <= 32) ? newBitRate : 16; }
void BitCrusher::setDithering(bool enabled)         { dithering      = enabled; }
void BitCrusher::setInterpolated(bool enabled)      { isInterpolated = enabled; }
void BitCrusher::setFeedback(float amount)            { feedbackAmount    = amount; }
void BitCrusher::setSaturation(float amount)          { saturationAmount  = amount; }
void BitCrusher::setOversamplingEnabled(bool enabled) { oversamplingEnabled = enabled; }
void BitCrusher::setDecayHold(bool enabled, float coeff)
{
    decayHoldEnabled = enabled;
    holdDecayCoeff   = coeff;
}

void BitCrusher::reset()
{
    for (int ch = 0; ch < 2; ++ch)
    {
        counter      [ch] = 0;
        heldSample   [ch] = 0.0f;
        prevSample   [ch] = 0.0f;
        quantError   [ch] = 0.0f;
        prevSampleOS [ch] = 0.0f;

        for (int s = 0; s < 2; ++s)
        {
            interpZ1[ch][s] = 0.0f;
            interpZ2[ch][s] = 0.0f;
            reconZ1 [ch][s] = 0.0f;
            reconZ2 [ch][s] = 0.0f;
        }
    }
}

// ── Oversampling filter design ────────────────────────────────────────────────
// 4th-order Butterworth lowpass as two cascaded DF2T biquad sections.
// Cutoff at original Nyquist relative to the oversampled rate:
//   fc_normalised = 0.5 / oversampleFactor = 0.125  (for 4×)
//   K = tan(π · fc) = tan(π/8) ≈ 0.4142
// Butterworth pole-Q values for 4th order: Q[0]=0.5412, Q[1]=1.3066
// Each section (symmetric lowpass): b0=b2=K²/denom, b1=2·b0, a1=2(K²−1)/denom, a2=(1−K/Q+K²)/denom
void BitCrusher::computeOversamplingCoeffs()
{
    const float K  = std::tan (float (M_PI) / float (2 * oversampleFactor)); // tan(π/8)
    const float K2 = K * K;

    // 4th-order Butterworth Q values for the two quadratic factors
    const float Q[2] = { 0.5412f, 1.3066f };

    for (int s = 0; s < 2; ++s)
    {
        const float denom = 1.0f + K / Q[s] + K2;
        osB0[s] = K2 / denom;
        osA1[s] = 2.0f * (K2 - 1.0f) / denom;
        osA2[s] = (1.0f - K / Q[s] + K2) / denom;
    }
}

// ── Helpers ───────────────────────────────────────────────────────────────────

// DF2T biquad: symmetric lowpass (b1 = 2·b0, b2 = b0)
static inline float biquad (float x, float b0, float a1, float a2, float& z1, float& z2)
{
    const float y = b0 * x + z1;
    z1 = 2.0f * b0 * x - a1 * y + z2;
    z2 = b0 * x - a2 * y;
    return y;
}

// PolyBLEP correction for S&H step discontinuities
float BitCrusher::polyBlepFunc(float t, float dt)
{
    if (t < dt)
    {
        t /= dt;
        return t + t - t * t - 1.0f;
    }
    if (t > 1.0f - dt)
    {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}

// TPDF dither + mid-tread quantisation
float BitCrusher::quantizeSample(float sample) const
{
    const float levels = std::pow (2.0f, float (bitRate)) - 1.0f;
    float s = sample;

    if (dithering)
    {
        const float r1 = (float (rand()) / float (RAND_MAX)) * 2.0f - 1.0f;
        const float r2 = (float (rand()) / float (RAND_MAX)) * 2.0f - 1.0f;
        s += (r1 + r2) * 0.5f / levels;
    }

    return std::round (s * levels) / levels;
}

// ── Main DSP ──────────────────────────────────────────────────────────────────
float BitCrusher::processSample(float signal, int channel)
{
    // ── 1. Quantisation-error feedback via internal Chorus ────────────────────
    const float fedback = errorChorus.processSample (quantError[channel], channel) * feedbackAmount;
    const float input   = signal + fedback;

    // ── 2. Tape saturation (before interpolator) ──────────────────────────────
    // tanh soft-clip with drive 1..10; normalised by tanh(drive) so unity at ±1.
    float saturated = input;
    if (saturationAmount > 0.0f)
    {
        const float drive = 1.0f + saturationAmount * 9.0f;
        saturated = std::tanh (input * drive) / std::tanh (drive);
    }

    float output = 0.0f;

    if (oversamplingEnabled)
    {
        for (int i = 0; i < oversampleFactor; ++i)
        {
            // ── Interpolator: linear ramp from prevSampleOS to saturated input ─
            const float frac = float (i + 1) / float (oversampleFactor);
            float up = prevSampleOS[channel] + (saturated - prevSampleOS[channel]) * frac;

            // ── Anti-imaging filter ───────────────────────────────────────────
            up = biquad (up, osB0[0], osA1[0], osA2[0], interpZ1[channel][0], interpZ2[channel][0]);
            up = biquad (up, osB0[1], osA1[1], osA2[1], interpZ1[channel][1], interpZ2[channel][1]);

            // ── Quantise at 4× rate ───────────────────────────────────────────
            const float q = quantizeSample (up);

            // ── Sample-and-hold on quantised signal ───────────────────────────
            // Counter advances at original rate; only update state on first
            // upsampled iteration so decay/latch happen exactly once per original sample.
            if (i == 0)
            {
                if (counter[channel] == 0)
                {
                    prevSample[channel] = heldSample[channel];
                    heldSample[channel] = q;
                }
                else if (decayHoldEnabled)
                {
                    heldSample[channel] *= holdDecayCoeff;
                }
            }

            // ── Optional linear interpolation + PolyBLEP on held value ───────
            const float t  = float (counter[channel]) / float (reductionFactor);
            const float dt = 1.0f / float (reductionFactor);
            float held = heldSample[channel];

            if (isInterpolated)
            {
                held  = prevSample[channel] + t * (heldSample[channel] - prevSample[channel]);
                held += (heldSample[channel] - prevSample[channel]) * 0.5f * polyBlepFunc (t, dt);
            }

            // ── Reconstruction filter → decimator ─────────────────────────────
            float recon = biquad (held, osB0[0], osA1[0], osA2[0], reconZ1[channel][0], reconZ2[channel][0]);
            recon       = biquad (recon, osB0[1], osA1[1], osA2[1], reconZ1[channel][1], reconZ2[channel][1]);

            output = recon; // keep last of the 4 filtered outputs (decimation)
        }

        prevSampleOS[channel] = saturated;
    }
    else
    {
        // ── Non-oversampled path: saturate → quantise → S&H ──────────────────
        const float q = quantizeSample (saturated);

        if (counter[channel] == 0)
        {
            prevSample[channel] = heldSample[channel];
            heldSample[channel] = q;
        }
        else if (decayHoldEnabled)
        {
            heldSample[channel] *= holdDecayCoeff;
        }

        const float t  = float (counter[channel]) / float (reductionFactor);
        const float dt = 1.0f / float (reductionFactor);
        float held = heldSample[channel];

        if (isInterpolated)
        {
            held  = prevSample[channel] + t * (heldSample[channel] - prevSample[channel]);
            held += (heldSample[channel] - prevSample[channel]) * 0.5f * polyBlepFunc (t, dt);
        }

        output = held;
    }

    counter[channel] = (counter[channel] + 1) % reductionFactor;

    // ── Track quantisation error for next sample's feedback ───────────────────
    quantError[channel] = output - input;

    return output;
}
