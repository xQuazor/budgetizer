#pragma once
#include "PluginProcessor.h"
#include "ParameterBridge.h"
#include "license/LicenseValidator.h"
#include <juce_gui_extra/juce_gui_extra.h>

class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void resized() override;

private:
    void sendLicenseStatus();

    AudioPluginAudioProcessor& processorRef;

    LicenseValidator::License currentLicense;

    // browser must be declared before paramBridge (initialized first)
    juce::WebBrowserComponent browser;
    ParameterBridge paramBridge;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
