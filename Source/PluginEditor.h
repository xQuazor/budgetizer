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
    juce::Slider dryWetKnob;
    juce::Slider chorusFeedbackKnob;
    juce::Slider chorusDelayMultKnob;
    juce::Slider masterDryWetKnob;

    juce::Label bitDepthLabel;
    juce::Label reductionFactorLabel;
    juce::Label cutoffLabel;
    juce::Label dryWetLabel;
    juce::Label chorusFeedbackLabel;
    juce::Label chorusDelayMultLabel;
    juce::Label masterDryWetLabel;

    juce::ToggleButton ditheringButton     { "Dithering" };
    juce::ToggleButton interpolatedButton  { "Interpolated" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reductionFactorAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusFeedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusDelayMultAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterDryWetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ditheringAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> interpolatedAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
