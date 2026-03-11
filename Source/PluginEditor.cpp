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

    setupKnob (bitDepthKnob);
    setupKnob (reductionFactorKnob);
    setupKnob (cutoffKnob);
    setupKnob (dryWetKnob);
    setupKnob (chorusFeedbackKnob);
    setupKnob (chorusDelayMultKnob);
    setupKnob (masterDryWetKnob);

    setupLabel (bitDepthLabel,        "Bit Depth");
    setupLabel (reductionFactorLabel, "Sample Rate Red.");
    setupLabel (cutoffLabel,          "Cutoff");
    setupLabel (dryWetLabel,          "Dry/Wet");
    setupLabel (chorusFeedbackLabel,  "Chorus FB");
    setupLabel (chorusDelayMultLabel, "Delay Mult");
    setupLabel (masterDryWetLabel,    "Master D/W");

    bitDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "bitDepth", bitDepthKnob);

    reductionFactorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "reductionFactor", reductionFactorKnob);

    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "cutoff", cutoffKnob);

    dryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "dryWet", dryWetKnob);

    chorusFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "chorusFeedback", chorusFeedbackKnob);

    chorusDelayMultAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "chorusDelayMult", chorusDelayMultKnob);

    masterDryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "masterDryWet", masterDryWetKnob);

    ditheringAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "dithering", ditheringButton);
    addAndMakeVisible (ditheringButton);

    interpolatedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "interpolated", interpolatedButton);
    addAndMakeVisible (interpolatedButton);

    setSize (840, 240);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area      = getLocalBounds().reduced (10);
    const int labelH  = 20;
    const int buttonH = 30;
    const int colW    = area.getWidth() / 7;

    // Bottom strip: two toggles side by side
    auto bottomStrip = area.removeFromBottom (buttonH);
    const int btnW = bottomStrip.getWidth() / 2;
    ditheringButton   .setBounds (bottomStrip.removeFromLeft (btnW).reduced (4, 0));
    interpolatedButton.setBounds (bottomStrip.reduced (4, 0));

    // Top row: six knobs
    auto placeKnob = [&] (juce::Label& label, juce::Slider& knob)
    {
        auto col = area.removeFromLeft (colW);
        label.setBounds (col.removeFromTop (labelH));
        knob.setBounds  (col);
    };

    placeKnob (bitDepthLabel,        bitDepthKnob);
    placeKnob (reductionFactorLabel, reductionFactorKnob);
    placeKnob (cutoffLabel,          cutoffKnob);
    placeKnob (dryWetLabel,          dryWetKnob);
    placeKnob (chorusFeedbackLabel,  chorusFeedbackKnob);
    placeKnob (chorusDelayMultLabel, chorusDelayMultKnob);
    placeKnob (masterDryWetLabel,    masterDryWetKnob);
}
