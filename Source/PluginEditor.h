#pragma once
#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;



private:
    AudioPluginAudioProcessor& processorRef;

    juce::Slider bitDepthKnob;
    juce::Slider reductionFactorKnob;
    juce::Slider cutoffKnob;

    juce::Label bitDepthLabel;
    juce::Label reductionFactorLabel;
    juce::Label cutoffLabel;

    juce::ToggleButton ditheringButton     { "Dithering" };
    juce::ToggleButton linearInterpButton  { "Linear Interp" };
    juce::ToggleButton polyBlepButton      { "PolyBLEP" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reductionFactorAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ditheringAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> linearInterpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> polyBlepAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
