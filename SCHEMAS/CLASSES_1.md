# Context
This piece of software is intended to function as an audio plugin created using 
the JUCE framework using C++.

## Classes

#### LowPassFilter
Variables
- double sampleRate = 44100.0; // hz
- float cutoff = 1000.0 f; // hz
- float q = 0.0 f;
- float previousSample = 0.0 f;

Functions
- void setSampleRate (double newSampleRate)
- void setCutoff (float newCutoff)
- void setQ (float newQ)
- void reset ()
- float processSample(float signal)

#### BitCrusher
Variables
- double sampleRate = 44100.0; // hz
- int reductionFactor = 4;
- int bitRate = 16;
- int counter = 0;
- float heldSample = 0.0 f;

Functions
- void setSampleRate (double newSampleRate)
- void setReductionFactor (int newFactor)
- void setBitRate (int newBitRate )
- void reset ()
- float processSample (float signal)

#### Chorus
Variables
- AudioBuffer<float> delayBufferA;
- AudioBuffer<float> delayBufferB;

- float rateA;
- float rateB;

- float depthA;
- float depthB;

- float baseDelayA;
- float baseDelayB;

Functions
- void setDelayTimeA (float newDelayTime)
- void setDelayTimeB (float newDelayTime)

#### PitchModulator
Variables
- AudioBuffer<float> delayBuffer;
- float rate;
- float depth;
- float baseDelay;

Functions
- void setDelayTime (float newDelayTime)

#### NoiseLFO
Variables
- double sampleRate
- float phase;
- float rate;


Functions
- float returnModulation ()

#### TriangleLFO
Variables
- double sampleRate
- float phase;
- float rate;


Functions
- float returnModulation ()

#### ComplexLFO

Variables
- double sampleRate
- float phase;
- float rateSlow;
- float rateMedium;
- float rateFast;

Functions
- float returnModulation ()

## Modulation Mappings

This section explains which variables of a given class are modulated by which LFO class.
The LFO modulation values should be implemented inside the given classes.

ComplexLFO -> BitCrusher.reductionFactor;
ComplexLFO -> PitchModulator.delayBuffer;

TriangleLFO -> Chorus.baseDelayA;
TriangleLFO -> Chorus.baseDelayB;

NoiseLFO -> TriangleLFO.rate;
NoiseLFO -> ComplexLFO.rateSlow;
NoiseLFO -> ComplexLFO.rateMedium;
NoiseLFO -> ComplexLFO.rateFast;

## Signal Chain

LowPassFilter -> BitCrusher -> Chorus -> PitchModulator
