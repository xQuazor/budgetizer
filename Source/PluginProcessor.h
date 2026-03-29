#pragma once
#include "player/AudioFilePlayer.h"
#include "bitcrusher/BitCrusher.h"
#include "radio/NoiseGenerator.h"
#include "radio/RadioTuner.h"
#include "radio/MechanicalDrift.h"
#include "radio/SweepFilter.h"
#include "radio/StationBurstGenerator.h"
#include "radio/BandLimiter.h"
#include <juce_audio_processors/juce_audio_processors.h>

#include "pitch/PitchModulator.h"

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

    // Returns a gain scalar so that applying it to the post-tanh signal restores
    // the same RMS level as the pre-tanh signal had over the current block.
    static float computeTanhLevelMatchGain (float preSumSq, float postSumSq);

private:
    AudioFilePlayer        audioFilePlayer;

    // Radio DSP chain
    NoiseGenerator         noiseGen;
    RadioTuner             tuner;
    MechanicalDrift        drift;
    SweepFilter            sweepFilter;
    StationBurstGenerator  burstGen;
    BandLimiter            bandLimiter;

    BitCrusher             bitCrusher;
    PitchModulator         pitchModulator;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
