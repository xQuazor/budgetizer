#pragma once
#include "BandLimiter.h"
#include "StationBurstGenerator.h"
#include "SweepFilter.h"
#include "../noise/NoiseGenerator.h"
#include "../modulation/ComplexLFO.h"
#include "FormantShifter.h"
#include "PitchShifter.h"

class Radio
{
public:
    void setSampleRate (double sampleRate);
    void setHoldAmount    (float amount);  // 0 = no hold, 1 = max hold
    void setHoldRate      (float hz);      // triangle LFO rate in Hz
    void setFormantShift  (float amount);  // 0 = no shift, 1 = max shift
    void setPitchRatio    (float ratio);   // 0.5 = -1 oct, 1.0 = no shift, 2.0 = +1 oct
    void setPitchMode     (PitchShifter::Mode m);
    void setDropoutAmount (float amount);  // 0 = no dropouts, 1 = frequent
    void setNoiseLevel    (float level);   // noise amplitude during dropouts
    float processSample (float sample);

private:
    double sampleRate = 44100.0;

    NoiseGenerator         noiseGen;
    SweepFilter            sweepFilter;
    StationBurstGenerator  burstGen;
    BandLimiter            bandLimiter;

    // Sample hold / looper (10-sample bracket)
    float loopBuffer[8192] = {};
    int   loopWritePos   = 0;
    int   loopReadPos    = 0;
    bool  isHolding      = false;
    bool  prevHolding    = false;  // edge detection
    bool  holdIsPitched  = false;  // decided once per bracket
    float holdAmount     = 0.01f;

    // Triangle LFO driving the hold on/off
    float trianglePhase = 0.0f;
    float triangleRate  = 0.5f;  // Hz base rate
    ComplexLFO          rateModulator;  // modulates triangleRate for unpredictability
    FormantShifter      formantShifter;
    PitchShifter        pitchShifter;

    float sampleHold (float input);
    void  tickTriangleLFO ();

    // Noise gate
    float         dropoutAmount = 0.5f;
    bool          gateOpen      = true;
    int           gateCounter   = 0;
    juce::Random  gateRng;

    float applyNoiseGate (float signal);
};
