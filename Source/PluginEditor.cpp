#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <BinaryData.h>

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      browser(juce::WebBrowserComponent::Options{}
          .withNativeIntegrationEnabled()
          .withResourceProvider([](const juce::String& url) -> std::optional<juce::WebBrowserComponent::Resource>
          {
              auto filename = juce::File(url).getFileName();
              if (filename.isEmpty())
                  filename = "index.html";

              // Use originalFilenames to avoid reimplementing JUCE's name-mangling rules
              const char* resourceName = nullptr;
              for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
              {
                  if (filename == BinaryData::originalFilenames[i])
                  {
                      resourceName = BinaryData::namedResourceList[i];
                      break;
                  }
              }
              if (resourceName == nullptr)
                  return std::nullopt;

              int size = 0;
              const auto* data = BinaryData::getNamedResource(resourceName, size);
              if (data == nullptr)
                  return std::nullopt;

              juce::String mime = "application/octet-stream";
              if      (filename.endsWith(".html")) mime = "text/html";
              else if (filename.endsWith(".js"))   mime = "application/javascript";
              else if (filename.endsWith(".css"))  mime = "text/css";
              else if (filename.endsWith(".webp")) mime = "image/webp";
              else if (filename.endsWith(".png"))  mime = "image/png";
              else if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) mime = "image/jpeg";
              else if (filename.endsWith(".svg"))  mime = "image/svg+xml";

              return juce::WebBrowserComponent::Resource {
                  std::vector<std::byte>(reinterpret_cast<const std::byte*>(data),
                                         reinterpret_cast<const std::byte*>(data) + size),
                  mime
              };
          })
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
