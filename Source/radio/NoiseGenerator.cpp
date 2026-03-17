#include "NoiseGenerator.h"

void NoiseGenerator::prepare (double /*sampleRate*/) {}

void NoiseGenerator::setLevel (float level)
{
    noiseLevel = level;
}

float NoiseGenerator::process()
{
    return (rng.nextFloat() * 2.0f - 1.0f) * noiseLevel;
}
