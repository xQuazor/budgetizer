#pragma once
#include <juce_dsp/juce_dsp.h>

class SweepFilter
{
public:
    void prepare (double sampleRate);
    void setFrequency (float freq);
    void setQ (float q);
    float processSample (float input, int channel);

private:
    juce::dsp::StateVariableTPTFilter<float> filter;
};
