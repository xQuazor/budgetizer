#include "../noise/NoiseGenerator.h"

void NoiseGenerator::prepare (double /*sampleRate*/) {}

void NoiseGenerator::setLevel (float level)
{
    noiseLevel = level;
}

float NoiseGenerator::process()
{
    float white  = rng.nextFloat() * 2.0f - 1.0f;
    // Leaky integrator: accumulate white noise with a pole near DC (0.98).
    // Dividing by 1.02 keeps the DC gain at 1 and bounds the output to ~±1.
    brownAccum = (brownAccum + 0.02f * white) / 1.02f;
    // Scale up to compensate for the low RMS of brown noise relative to its peak.
    return brownAccum * 3.5f * noiseLevel;
}
