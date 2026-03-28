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
              [this](juce::var) { paramBridge.sendFullState(); })
          .withEventListener("resize",
              [this](juce::var msg) {
                  int w = (int) msg["width"];
                  int h = (int) msg["height"];
                  if (w > 0 && h > 0) {
                      juce::Component::SafePointer<AudioPluginAudioProcessorEditor> safe(this);
                      juce::MessageManager::callAsync([safe, w, h] {
                          if (safe != nullptr) safe->setSize(w, h);
                      });
                  }
              })),
      paramBridge(p.apvts, browser)
{
    setSize(900, 600);
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
