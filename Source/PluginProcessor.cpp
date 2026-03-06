#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
      apvts (*this, nullptr, "Parameters", createParameters())
{}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
AudioPluginAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        "bitDepth", "Bit Depth", 1, 16, 8));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        "reductionFactor", "Sample Rate Reduction", 1, 32, 4));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "cutoff", "Cutoff", 0.0f, 20000.0f, 5000.0f));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "dithering", "Dithering", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "linearInterp", "Linear Interpolation", true));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "polyBlep", "PolyBLEP", false));
    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const { return JucePlugin_Name; }

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}
bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}
bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}
double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int  AudioPluginAudioProcessor::getNumPrograms()      { return 1; }
int  AudioPluginAudioProcessor::getCurrentProgram()   { return 0; }
void AudioPluginAudioProcessor::setCurrentProgram (int index)         { juce::ignoreUnused (index); }
const juce::String AudioPluginAudioProcessor::getProgramName (int index)  { juce::ignoreUnused (index); return {}; }
void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName) { juce::ignoreUnused (index, newName); }

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);

    // Source oscillator (kept for reference)
    // oscillator.prepare (sampleRate);
    // oscillator.setFrequency (440.0f);
    // oscillator.setAmplitude (0.3f);
    // oscillator.reset();

    // Audio file player — set the directory path below
    audioFilePlayer.prepare (sampleRate);
    audioFilePlayer.loadFromDirectory (juce::File ("/Users/dovis/CLionProjects/degrainator/music"));

    // LFO hierarchy
    noiseLFO.setSampleRate (sampleRate);
    noiseLFO.setRate (0.5f);

    triangleLFO.setSampleRate (sampleRate);
    triangleLFO.setRate (baseTriangleRate);

    complexLFO.setSampleRate (sampleRate);
    complexLFO.setRateSlow   (baseSlowRate);
    complexLFO.setRateMedium (baseMedRate);
    complexLFO.setRateFast   (baseFastRate);

    // Signal chain
    lowPassFilter.setSampleRate (sampleRate);
    lowPassFilter.setCutoff (4000.0f);
    lowPassFilter.setQ (0.707f);
    lowPassFilter.reset();

    bitCrusher.setSampleRate (sampleRate);
    bitCrusher.setReductionFactor (baseReduction);
    bitCrusher.setBitRate (8);
    bitCrusher.reset();

    chorus.prepare (sampleRate, 2);
    chorus.setDelayTimeA (0.015f);
    chorus.setDelayTimeB (0.020f);

    pitchModulatorL.prepare (sampleRate);
    pitchModulatorL.setDelayTime (0.01f);

    pitchModulatorR.prepare (sampleRate);
    pitchModulatorR.setDelayTime (0.01f);
}

void AudioPluginAudioProcessor::releaseResources() {}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif
    return true;
  #endif
}

//==============================================================================
void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples             = buffer.getNumSamples();

    // Read UI parameters once per block
    bitCrusher.setBitRate   (static_cast<int>  (*apvts.getRawParameterValue ("bitDepth")));
    bitCrusher.setDithering            (*apvts.getRawParameterValue ("dithering")    >= 0.5f);
    bitCrusher.setLinearInterpolation  (*apvts.getRawParameterValue ("linearInterp") >= 0.5f);
    bitCrusher.setPolyBlep             (*apvts.getRawParameterValue ("polyBlep")     >= 0.5f);
    const int   uiReduction = static_cast<int> (*apvts.getRawParameterValue ("reductionFactor"));
    const float uiCutoff    = *apvts.getRawParameterValue ("cutoff");

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // ── 1. NoiseLFO → modulate TriangleLFO and ComplexLFO rates ──────────
        const float noiseVal = noiseLFO.returnModulation();

        triangleLFO.setRate (baseTriangleRate * (1.0f + 0.5f * noiseVal));
        complexLFO.setRateSlow   (baseSlowRate  * (1.0f + 0.5f * noiseVal));
        complexLFO.setRateMedium (baseMedRate   * (1.0f + 0.5f * noiseVal));
        complexLFO.setRateFast   (baseFastRate  * (1.0f + 0.5f * noiseVal));
        // lowPassFilter.setCutoff (uiCutoff + (1000.0f * noiseVal));
        lowPassFilter.setCutoff (uiCutoff);

        const float triangleVal = triangleLFO.returnModulation();
        const float complexVal  = complexLFO.returnModulation();

        // ── 2. Apply modulation to effect targets ─────────────────────────────
        chorus.applyTriangleLFO (triangleVal);                              // → baseDelayA/B

        const int modulatedReduction = juce::jlimit (1, 32, uiReduction + int (complexVal * 1.0f));
        bitCrusher.setReductionFactor (modulatedReduction);                 // → reductionFactor

        pitchModulatorL.applyComplexLFO (complexVal);                       // → depth
        pitchModulatorR.applyComplexLFO (complexVal);

        // ── 3. Signal chain: AudioFilePlayer → LPF → BitCrusher ─────────────
        // const float oscSample = oscillator.getNextSample();
        const float oscSample = audioFilePlayer.getNextSample();
        const float filtered  = lowPassFilter.processSample (oscSample);
        const float crushed   = bitCrusher.processSample (filtered);

        // ── 4. Per-channel: Chorus → PitchModulator → output ─────────────────
        const float outL = pitchModulatorL.processSample (chorus.processSample (crushed, 0));
        const float outR = (totalNumOutputChannels > 1)
                         ? pitchModulatorR.processSample (chorus.processSample (crushed, 1))
                         : outL;

        buffer.getWritePointer (0)[sample] = outL;
        if (totalNumOutputChannels > 1)
            buffer.getWritePointer (1)[sample] = outR;
    }

    // Clear any extra output channels beyond stereo
    for (int ch = 2; ch < totalNumOutputChannels; ++ch)
        buffer.clear (ch, 0, numSamples);
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)   { juce::ignoreUnused (destData); }
void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes) { juce::ignoreUnused (data, sizeInBytes); }

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
