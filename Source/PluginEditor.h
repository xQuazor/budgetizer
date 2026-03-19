#pragma once
#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AudioPluginAudioProcessor& processorRef;

    juce::Slider tuneSpeedKnob;
    juce::Slider staticAmountKnob;
    juce::Slider driftKnob;
    juce::Slider burstDensityKnob;
    juce::Slider bandwidthKnob;
    juce::Slider masterMixKnob;

    juce::Label tuneSpeedLabel;
    juce::Label staticAmountLabel;
    juce::Label driftLabel;
    juce::Label burstDensityLabel;
    juce::Label bandwidthLabel;
    juce::Label masterMixLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tuneSpeedAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> staticAmountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driftAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> burstDensityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bandwidthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterMixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
