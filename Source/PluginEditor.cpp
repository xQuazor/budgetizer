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
              [this](juce::var) {
                  paramBridge.sendFullState();
                  sendLicenseStatus();
              })
          .withEventListener("submitLicense",
              [this](juce::var msg) {
                  const auto key = msg["key"].toString().trim();
                  auto lic = LicenseValidator::validate(key);
                  if (lic.valid)
                      LicenseValidator::saveKey(key);
                  currentLicense = lic;
                  processorRef.setLicensed(lic.valid);
                  sendLicenseStatus();
              })
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
    currentLicense = LicenseValidator::checkSaved();

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

void AudioPluginAudioProcessorEditor::sendLicenseStatus()
{
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();
    obj->setProperty("valid", currentLicense.valid);
    obj->setProperty("email", currentLicense.email);
    browser.emitEventIfBrowserIsVisible("licenseStatus", juce::var(obj.get()));
}
