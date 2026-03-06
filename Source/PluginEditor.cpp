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

    setupLabel (bitDepthLabel,        "Bit Depth");
    setupLabel (reductionFactorLabel, "Sample Rate Red.");
    setupLabel (cutoffLabel,          "Cutoff");

    bitDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "bitDepth", bitDepthKnob);

    reductionFactorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "reductionFactor", reductionFactorKnob);

    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "cutoff", cutoffKnob);

    ditheringAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "dithering", ditheringButton);
    addAndMakeVisible (ditheringButton);

    linearInterpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "linearInterp", linearInterpButton);
    addAndMakeVisible (linearInterpButton);

    polyBlepAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "polyBlep", polyBlepButton);
    addAndMakeVisible (polyBlepButton);

    setSize (480, 240);
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
    const int colW    = area.getWidth() / 3;

    // Bottom strip: three toggles side by side
    auto bottomStrip = area.removeFromBottom (buttonH);
    const int btnW = bottomStrip.getWidth() / 3;
    ditheringButton   .setBounds (bottomStrip.removeFromLeft (btnW).reduced (4, 0));
    linearInterpButton.setBounds (bottomStrip.removeFromLeft (btnW).reduced (4, 0));
    polyBlepButton    .setBounds (bottomStrip.reduced (4, 0));

    // Top rows: three knobs
    auto placeKnob = [&] (juce::Label& label, juce::Slider& knob)
    {
        auto col = area.removeFromLeft (colW);
        label.setBounds (col.removeFromTop (labelH));
        knob.setBounds  (col);
    };

    placeKnob (bitDepthLabel,        bitDepthKnob);
    placeKnob (reductionFactorLabel, reductionFactorKnob);
    placeKnob (cutoffLabel,          cutoffKnob);
}
