#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    auto setupKnob = [this] (juce::Slider& knob)
    {
        knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        knob.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 20);
        addAndMakeVisible (knob);
    };

    auto setupLabel = [this] (juce::Label& label, const juce::String& text)
    {
        label.setText (text, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
        label.setFont (juce::Font (13.0f));
        addAndMakeVisible (label);
    };

    setupKnob (tuneSpeedKnob);
    setupKnob (staticAmountKnob);
    setupKnob (driftKnob);
    setupKnob (burstDensityKnob);
    setupKnob (bandwidthKnob);
    setupKnob (masterMixKnob);

    setupLabel (tuneSpeedLabel,    "Tune Speed");
    setupLabel (staticAmountLabel, "Static");
    setupLabel (driftLabel,        "Drift");
    setupLabel (burstDensityLabel, "Burst Density");
    setupLabel (bandwidthLabel,    "Bandwidth");
    setupLabel (masterMixLabel,    "Mix");

    tuneSpeedAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "tuneSpeed",    tuneSpeedKnob);
    staticAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "staticAmount", staticAmountKnob);
    driftAttachment        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "drift",        driftKnob);
    burstDensityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "burstDensity", burstDensityKnob);
    bandwidthAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "bandwidth",    bandwidthKnob);
    masterMixAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "masterMix",    masterMixKnob);

    setSize (720, 200);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area        = getLocalBounds().reduced (10);
    const int labelH = 20;
    const int colW   = area.getWidth() / 6;

    auto placeKnob = [&] (juce::Label& label, juce::Slider& knob)
    {
        auto col = area.removeFromLeft (colW);
        label.setBounds (col.removeFromTop (labelH));
        knob.setBounds  (col);
    };

    placeKnob (tuneSpeedLabel,    tuneSpeedKnob);
    placeKnob (staticAmountLabel, staticAmountKnob);
    placeKnob (driftLabel,        driftKnob);
    placeKnob (burstDensityLabel, burstDensityKnob);
    placeKnob (bandwidthLabel,    bandwidthKnob);
    placeKnob (masterMixLabel,    masterMixKnob);
}
