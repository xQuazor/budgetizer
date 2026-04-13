#include "Radio.h"

void Radio::setSampleRate (double sr) {
    sampleRate = sr;
    burstGen.setCurrentFrequency(200);
    noiseGen.prepare(sr);
    burstGen.prepare(sr);
    rateModulator.setSampleRate(sr);
    rateModulator.setRateSlow   (0.07f);
    rateModulator.setRateMedium (0.23f);
    rateModulator.setRateFast   (0.61f);
    formantShifter.setSampleRate(sr);
    pitchShifter.setSampleRate(sr);
    preEmphasis.prepare(sr);
    multipath.prepare(sr);
    limiter.prepare(sr);
}

void Radio::setHoldAmount (float amount) {
    holdAmount = std::clamp(amount, 0.0f, 1.0f);
}

void Radio::setHoldRate (float hz) {
    triangleRate = std::max(hz, 0.01f);
}

void Radio::setFormantShift (float amount) {
    formantShifter.setShiftAmount(amount);
}

void Radio::setPitchRatio (float r) {
    pitchShifter.setRatio(r);
}

void Radio::setPitchMode (PitchShifter::Mode m) {
    pitchShifter.setMode(m);
}

void Radio::setDropoutAmount (float amount) {
    dropoutAmount = amount < 0.0f ? 0.0f : (amount > 1.0f ? 1.0f : amount);
}

void Radio::setNoiseLevel (float level) {
    noiseGen.setLevel(level);
}

void Radio::setCurrentFrequency (float frequency) {
    burstGen.setCurrentFrequency(frequency);
}

void Radio::setTriangleDepth (float depth) {
    triangleDepth = std::clamp(depth, 0.0f, 1.0f);
}

void Radio::setBpm (float newBpm) {
    bpm = std::max(newBpm, 1.0f);
}

void Radio::setTempoSyncHold (bool enabled) {
    if (tempoSyncHold == enabled) return;
    tempoSyncHold    = enabled;
    tempoHoldCounter = holdIntervalSamples();
}

void Radio::setTempoSyncGate (bool enabled) {
    if (tempoSyncGate == enabled) return;
    tempoSyncGate = enabled;
    gateCounter   = gateIntervalSamples();
}

void Radio::setHoldNoteDivision (NoteDivision div) {
    holdDivision     = div;
    holdCustomMs     = 0.0f;          // switch to note-division mode
    tempoHoldCounter = holdIntervalSamples();
}

void Radio::setGateNoteDivision (NoteDivision div) {
    gateDivision = div;
    gateCustomMs = 0.0f;              // switch to note-division mode
    gateCounter  = gateIntervalSamples();
}

void Radio::setHoldCustomTimeMs (float ms) {
    holdCustomMs     = std::max(ms, 1.0f);
    tempoHoldCounter = holdIntervalSamples();
}

void Radio::setGateCustomTimeMs (float ms) {
    gateCustomMs = std::max(ms, 1.0f);
    gateCounter  = gateIntervalSamples();
}

int Radio::holdIntervalSamples() const {
    if (holdCustomMs > 0.0f)
        return std::max(1, static_cast<int>(sampleRate * holdCustomMs / 1000.0));
    return noteDivisionToSamples(holdDivision);
}

int Radio::gateIntervalSamples() const {
    if (gateCustomMs > 0.0f)
        return std::max(1, static_cast<int>(sampleRate * gateCustomMs / 1000.0));
    return noteDivisionToSamples(gateDivision);
}

int Radio::noteDivisionToSamples (NoteDivision div) const
{
    double beatSeconds = 60.0 / bpm;
    double noteSeconds;
    switch (div)
    {
        case NoteDivision::Whole:      noteSeconds = beatSeconds * 4.0;  break;
        case NoteDivision::Half:       noteSeconds = beatSeconds * 2.0;  break;
        case NoteDivision::Quarter:    noteSeconds = beatSeconds;         break;
        case NoteDivision::Eighth:     noteSeconds = beatSeconds * 0.5;  break;
        case NoteDivision::Sixteenth:  noteSeconds = beatSeconds * 0.25; break;
        default:                       noteSeconds = beatSeconds;         break;
    }
    return std::max(1, static_cast<int>(sampleRate * noteSeconds));
}

void Radio::setEmphasis (float amount)      { preEmphasis.setAmount(amount); }
void Radio::setMultipathMix (float mix)     { multipath.setMix(mix); }
void Radio::setMultipathDelay (float ms)    { multipath.setBaseDelay(ms); }
void Radio::setLimiterCeiling (float lin)   { limiter.setThreshold(lin); }

// Periodically cuts the signal and replaces it with noise.
// dropoutAmount controls how frequent and how long the dropouts are.
// When gateOpen: pass signal through. When closed: emit noise.
// When tempoSyncGate is true, both open and closed durations snap to
// noteDivisionToSamples(gateDivision) with no random variation.
float Radio::applyNoiseGate (float signal)
{
    if (--gateCounter <= 0)
    {
        gateOpen = !gateOpen;

        if (tempoSyncGate)
        {
            gateCounter = gateIntervalSamples();
        }
        else if (gateOpen)
        {
            // Open duration: long gap between dropouts; shorter at higher dropoutAmount
            int maxOpen = static_cast<int>(sampleRate * 2.0f);    // up to 2 s
            int minOpen = static_cast<int>(sampleRate * 0.05f);   // 50 ms minimum
            float openRange = float(maxOpen - minOpen);
            gateCounter = minOpen + static_cast<int>((1.0f - dropoutAmount) * openRange
                          * gateRng.nextFloat());
        }
        else
        {
            // Closed (noise) duration: longer at higher dropoutAmount; short burst at low
            int maxClosed = static_cast<int>(sampleRate * 0.4f);  // up to 400 ms
            int minClosed = static_cast<int>(sampleRate * 0.01f); // 10 ms minimum
            float closedRange = float(maxClosed - minClosed);
            gateCounter = minClosed + static_cast<int>(dropoutAmount * closedRange
                          * gateRng.nextFloat());
        }
    }

    return gateOpen ? signal : noiseGen.process();
}

// Triangle LFO: phase advances 0→1 per cycle.
// ComplexLFO modulates the instantaneous rate around the base, making cycle lengths uneven.
// When tempoSyncHold is true, a simple sample counter replaces the LFO; the hold toggles
// on/off every noteDivisionToSamples(holdDivision) samples with no random modulation.
void Radio::tickTriangleLFO()
{
    if (tempoSyncHold)
    {
        if (--tempoHoldCounter <= 0)
        {
            isHolding        = !isHolding && (holdAmount > 0.0f);
            tempoHoldCounter = holdIntervalSamples();
        }
        return;
    }

    // returnModulation() outputs ~-1..1; scale to ±50% of base rate
    float mod = rateModulator.returnModulation();
    float effectiveRate = triangleRate * (1.f + 1.0f * mod);
    if (effectiveRate < 0.01f) effectiveRate = 0.01f;

    trianglePhase += effectiveRate / static_cast<float>(sampleRate);
    if (trianglePhase >= 1.0f) trianglePhase -= 1.0f;

    float tri = ((trianglePhase < 0.5f) ? trianglePhase * 2.0f
                                        : (1.0f - trianglePhase) * 2.0f) * triangleDepth;

    isHolding = (tri > 0.5f) && (holdAmount > 0.0f);
}

// Captures incoming audio into a 32-sample loopBuffer when passing through.
// When isHolding, replays (loops) the captured bracket instead.
float Radio::sampleHold (float input)
{
    // Detect rising edge: decide once per bracket whether this hold gets pitch-shifted
    if (isHolding && !prevHolding)
        holdIsPitched = gateRng.nextFloat() > 0.5f;
    prevHolding = isHolding;

    if (!isHolding)
    {
        loopBuffer[loopWritePos] = input;
        loopWritePos = (loopWritePos + 1) & 8191;
        loopReadPos  = loopWritePos;
        return input;
    }

    float held = loopBuffer[loopReadPos];
    loopReadPos = (loopReadPos + 1) & 8191;
    return holdIsPitched ? pitchShifter.processSample(held) : held;
}

float Radio::processSample(float sample) {

    tickTriangleLFO();

    float sig = preEmphasis.processSample(sample);
    sig = burstGen.process(sig);
    sig = sampleHold(sig);
    sig = multipath.processSample(sig);
    sig = formantShifter.processSample(sig);
    sig = limiter.processSample(sig);
    sig = applyNoiseGate(sig);

    return sig;
}