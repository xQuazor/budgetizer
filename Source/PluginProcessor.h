#pragma once
#include "player/AudioFilePlayer.h"
#include "bitcrusher/BitCrusher.h"
#include <juce_audio_processors/juce_audio_processors.h>

#include "pitch/PitchModulator.h"
#include "radio/Radio.h"
#include "modulation/ComplexLFO.h"
#include "chorus/chorus.h"
#include "license/LicenseValidator.h"
#include <atomic>

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    void setLicensed(bool value) { licensed.store(value); }
    bool isLicensed() const      { return licensed.load(); }

    // Returns a gain scalar so that applying it to the post-tanh signal restores
    // the same RMS level as the pre-tanh signal had over the current block.
    static float computeTanhLevelMatchGain (float preSumSq, float postSumSq);

private:
    std::atomic<bool> licensed { false };
    AudioFilePlayer        audioFilePlayer;

    BitCrusher             bitCrusher;
    PitchModulator         pitchModulator;
    ComplexLFO             complexLFO;
    Radio                  radioEffect;
    Chorus                 chorus;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
