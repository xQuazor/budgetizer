#include "StationBurstGenerator.h"
#include <cmath>
#include <array>

static constexpr std::array<float, 5> kStationFreqs = { 500.0f, 1100.0f, 2000.0f, 3400.0f, 5000.0f };

void StationBurstGenerator::prepare (double sr)
{
    sampleRate = sr;
    // attack 5 ms, decay 50 ms
    attackCoeff = 1.0f - std::exp (-1.0f / (float)(sampleRate * 0.005));
    decayCoeff  = 1.0f - std::exp (-1.0f / (float)(sampleRate * 0.050));
}

void StationBurstGenerator::setCurrentFrequency (float freq)
{
    // density 0..1 → threshold 100..500 Hz
    const float threshold = 100.0f + density * 400.0f;

    bool nearStation = false;
    for (float sf : kStationFreqs)
        if (std::abs (freq - sf) < threshold) { nearStation = true; break; }

    if (nearStation)
        envValue += attackCoeff * (1.0f - envValue);
    else
        envValue += decayCoeff  * (0.0f  - envValue);
}

void StationBurstGenerator::setDensity (float d)
{
    density = d;
}

float StationBurstGenerator::process (float input)
{
    return input * envValue;
}
