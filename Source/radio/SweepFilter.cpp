#include "SweepFilter.h"

void SweepFilter::prepare (double sampleRate)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = 2048;
    spec.numChannels      = 2;

    filter.prepare (spec);
    filter.setType (juce::dsp::StateVariableTPTFilterType::bandpass);
    filter.setCutoffFrequency (1000.0f);
    filter.setResonance (5.0f);
}

void SweepFilter::setFrequency (float freq)
{
    filter.setCutoffFrequency (juce::jlimit (20.0f, 20000.0f, freq));
}

void SweepFilter::setQ (float q)
{
    filter.setResonance (juce::jlimit (0.1f, 20.0f, q));
}

float SweepFilter::processSample (float input, int channel)
{
    return filter.processSample (channel, input);
}
