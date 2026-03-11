#pragma once
#include <cmath>
#include <cstdlib>
#include "../chorus/Chorus.h"

class BitCrusher
{
public:
    // Call prepare() so the internal chorus and oversampling filters are initialised
    void prepare(double newSampleRate);
    void setSampleRate(double newSampleRate);   // kept for back-compat; does NOT init chorus
    void setReductionFactor(int newFactor);
    void setBitRate(int newBitRate);
    void setDithering(bool enabled);
    void setInterpolated(bool enabled);
    void setFeedback(float amount);
    void setSaturation(float amount);   // 0 = clean, 1 = fully saturated
    void setDecayHold(bool enabled, float coeff = 0.95f);
    void setOversamplingEnabled(bool enabled);
    void reset();
    // channel: 0 = left, 1 = right — each channel has independent internal state
    float processSample(float signal, int channel);

private:
    double sampleRate   = 44100.0;
    int reductionFactor = 4;
    int bitRate         = 16;

    // Per-channel S&H state
    int   counter   [2] = { 0, 0 };
    float heldSample[2] = { 0.0f, 0.0f };
    float prevSample[2] = { 0.0f, 0.0f };
    float quantError[2] = { 0.0f, 0.0f };

    bool  dithering      = false;
    bool  isInterpolated = false;

    // Quantisation error feedback via internal Chorus
    float  feedbackAmount  = 0.0f;
    Chorus errorChorus;

    // Tape saturation (applied before interpolator)
    float saturationAmount = 0.0f;

    // Exponential decay hold
    bool  decayHoldEnabled = false;
    float holdDecayCoeff   = 0.95f;

    // ── Oversampling (4×) ─────────────────────────────────────────────────────
    bool oversamplingEnabled = true;
    static constexpr int oversampleFactor = 4;

    // Previous sample fed into the upsampler (linear interpolation source)
    float prevSampleOS[2] = { 0.0f, 0.0f };

    // DF2T biquad state: [channel][stage]
    // interpFilter = anti-imaging (between upsampler and processing)
    // reconFilter  = reconstruction (between processing and decimator)
    float interpZ1[2][2] = {};
    float interpZ2[2][2] = {};
    float reconZ1 [2][2] = {};
    float reconZ2 [2][2] = {};

    // 4th-order Butterworth coefficients: two cascaded 2nd-order sections
    // Cutoff = sampleRate/2 normalised to oversampleFactor×sampleRate → fc = 0.5/4 = 0.125
    // Computed in prepare(). Each section: b1 = 2*b0, b2 = b0 (symmetric lowpass)
    float osB0[2] = {};   // feedforward gain for each stage
    float osA1[2] = {};   // first feedback coefficient for each stage
    float osA2[2] = {};   // second feedback coefficient for each stage

    void  computeOversamplingCoeffs();
    static float polyBlepFunc(float t, float dt);
    float quantizeSample(float sample) const;
};
