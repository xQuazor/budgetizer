#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      browser(juce::WebBrowserComponent::Options{}
          .withNativeIntegrationEnabled()
          .withEventListener("paramChange",
              [this](juce::var msg) { paramBridge.handleJSMessage(msg); })
          .withEventListener("uiReady",
              [this](juce::var) { paramBridge.sendFullState(); })),
      paramBridge(p.apvts, browser)
{
    setSize(800, 600);
    addAndMakeVisible(browser);

#if JUCE_DEBUG
    browser.goToURL("http://localhost:5173");
#else
    browser.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
#endif
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

void AudioPluginAudioProcessorEditor::resized()
{
    browser.setBounds(getLocalBounds());
}
