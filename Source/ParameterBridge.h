#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>

class ParameterBridge : public juce::AudioProcessorValueTreeState::Listener
{
public:
    ParameterBridge(juce::AudioProcessorValueTreeState& apvts,
                    juce::WebBrowserComponent& browser);
    ~ParameterBridge() override;

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void handleJSMessage(const juce::var& message);
    void sendFullState();

private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::WebBrowserComponent& browser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterBridge)
};
