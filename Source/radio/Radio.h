#pragma once
#include "StationBurstGenerator.h"
#include "../noise/NoiseGenerator.h"
#include "../modulation/ComplexLFO.h"
#include "FormantShifter.h"
#include "PitchShifter.h"
#include "PreEmphasisFilter.h"
#include "MultipathFilter.h"
#include "RadioLimiter.h"

class Radio
{
public:
    enum class NoteDivision { Whole, Half, Quarter, Eighth, Sixteenth };

    void setSampleRate (double sampleRate);
    void setHoldAmount    (float amount);  // 0 = no hold, 1 = max hold
    void setHoldRate      (float hz);      // triangle LFO rate in Hz
    void setFormantShift  (float amount);  // 0 = no shift, 1 = max shift
    void setPitchRatio    (float ratio);   // 0.5 = -1 oct, 1.0 = no shift, 2.0 = +1 oct
    void setPitchMode     (PitchShifter::Mode m);
    void setDropoutAmount (float amount);  // 0 = no dropouts, 1 = frequent
    void setNoiseLevel    (float level);   // noise amplitude during dropouts
    void setCurrentFrequency (float frequency);
    void setTriangleDepth    (float triangleDepth);
    void setEmphasis         (float amount);   // 0 = flat, 1 = full FM pre-emphasis
    void setMultipathMix     (float mix);      // 0-1, reflection strength
    void setMultipathDelay   (float ms);       // 0.1-3.0 ms
    void setLimiterCeiling   (float linear);   // 0.1-1.0

    // Tempo sync
    void setBpm               (float bpm);
    void setTempoSyncHold     (bool enabled);
    void setTempoSyncGate     (bool enabled);
    void setHoldNoteDivision  (NoteDivision div);  // switches hold to note-division mode
    void setGateNoteDivision  (NoteDivision div);  // switches gate to note-division mode
    void setHoldCustomTimeMs  (float ms);          // switches hold to fixed-ms mode
    void setGateCustomTimeMs  (float ms);          // switches gate to fixed-ms mode

    float processSample (float sample);

private:
    double sampleRate = 44100.0;

    NoiseGenerator         noiseGen;
    StationBurstGenerator  burstGen;
    PreEmphasisFilter      preEmphasis;
    MultipathFilter        multipath;
    RadioLimiter           limiter;

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
    float triangleDepth = 0.5f;
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

    // Tempo sync
    float        bpm             = 93.0f;
    bool         tempoSyncHold   = true;
    bool         tempoSyncGate   = true;
    NoteDivision holdDivision    = NoteDivision::Half;
    NoteDivision gateDivision    = NoteDivision::Half;
    int          tempoHoldCounter = 0;

    // Custom fixed-time mode (overrides note division when > 0)
    float holdCustomMs  = 0.0f;
    float gateCustomMs  = 0.0f;

    int noteDivisionToSamples (NoteDivision div) const;
    int holdIntervalSamples () const;   // respects custom-ms vs note-division
    int gateIntervalSamples () const;
};
