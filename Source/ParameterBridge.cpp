#include "ParameterBridge.h"

ParameterBridge::ParameterBridge(juce::AudioProcessorValueTreeState& apvts,
                                  juce::WebBrowserComponent& browser)
    : apvts(apvts), browser(browser)
{
    for (auto* param : apvts.processor.getParameters())
    {
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*>(param))
            apvts.addParameterListener(p->paramID, this);
    }
}

ParameterBridge::~ParameterBridge()
{
    for (auto* param : apvts.processor.getParameters())
    {
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*>(param))
            apvts.removeParameterListener(p->paramID, this);
    }
}

void ParameterBridge::parameterChanged(const juce::String& parameterID, float newValue)
{
    juce::MessageManager::callAsync([this, parameterID, newValue]()
    {
        juce::DynamicObject::Ptr obj = new juce::DynamicObject();
        obj->setProperty("id", parameterID);
        obj->setProperty("value", newValue);
        browser.emitEventIfBrowserIsVisible("parameterUpdate", juce::var(obj.get()));
    });
}

void ParameterBridge::handleJSMessage(const juce::var& message)
{
    auto paramID = message["id"].toString();
    auto value   = static_cast<float>(message["value"]);

    if (auto* param = apvts.getParameter(paramID))
        param->setValueNotifyingHost(param->convertTo0to1(value));
}

void ParameterBridge::sendFullState()
{
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();
    for (auto* param : apvts.processor.getParameters())
    {
        if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(param))
            obj->setProperty(rp->paramID, rp->convertFrom0to1(rp->getValue()));
    }
    browser.emitEventIfBrowserIsVisible("fullState", juce::var(obj.get()));
}
