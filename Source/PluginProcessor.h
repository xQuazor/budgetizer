#pragma once
#include "dsp/Oscillator.h"
#include "player/AudioFilePlayer.h"
#include "modulation/NoiseLFO.h"
#include "modulation/TriangleLFO.h"
#include "modulation/ComplexLFO.h"
#include "lowpass/lowpass.h"
#include "bitcrusher/BitCrusher.h"
#include "chorus/Chorus.h"
#include "pitch/PitchModulator.h"
#include <juce_audio_processors/juce_audio_processors.h>

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

private:
    // Source
    Oscillator      oscillator;     // kept for reference; commented out in processBlock
    AudioFilePlayer audioFilePlayer;

    // LFO hierarchy: NoiseLFO → TriangleLFO / ComplexLFO → effect targets
    NoiseLFO    noiseLFO;
    TriangleLFO triangleLFO;
    ComplexLFO  complexLFO;

    // Signal chain: LowPassFilter → BitCrusher → Chorus → PitchModulator
    LowPassFilter  lowPassFilter;
    BitCrusher     bitCrusher;
    Chorus         chorus;
    PitchModulator pitchModulatorL;
    PitchModulator pitchModulatorR;

    // Base LFO rates (NoiseLFO modulates these each sample)
    static constexpr float baseTriangleRate = 0.3f;
    static constexpr float baseSlowRate     = 0.1f;
    static constexpr float baseMedRate      = 0.5f;
    static constexpr float baseFastRate     = 2.0f;
    static constexpr float baseCutoff = 5000.0f;

    // Base sample-rate-reduction depth before ComplexLFO modulation
    static constexpr int baseReduction = 4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
