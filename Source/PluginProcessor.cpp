#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

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
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "tuneSpeed",    "Tune Speed",    50.0f,  2000.0f, 1000.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "staticAmount", "Static Amount", 0.0f,   1.0f,    0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "drift",        "Drift",         0.0f,   1.0f,    0.08f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "burstDensity", "Burst Density", 0.0f,   1.0f,    0.25f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "bandwidth",    "Bandwidth",     2000.0f, 5000.0f, 3500.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "masterMix",    "Master Mix",    0.0f,    1.0f,    1.0f));
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

    audioFilePlayer.prepare (sampleRate);
    audioFilePlayer.loadFromDirectory (juce::File ("/Users/dovis/CLionProjects/degrainator/music"));

    bitCrusher.prepare (sampleRate);
    bitCrusher.setReductionFactor (8);
    bitCrusher.setBitRate (8);
    bitCrusher.setFeedback (0.0f);

    noiseGen.prepare    (sampleRate);
    tuner.prepare       (sampleRate);
    drift.prepare       (sampleRate);
    sweepFilter.prepare (sampleRate);
    burstGen.prepare    (sampleRate);
    bandLimiter.prepare (sampleRate);
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

    // Read parameters once per block
    const float tuneSpeed    = *apvts.getRawParameterValue ("tuneSpeed");
    const float staticAmount = *apvts.getRawParameterValue ("staticAmount");
    const float driftAmount  = *apvts.getRawParameterValue ("drift");
    const float burstDensity = *apvts.getRawParameterValue ("burstDensity");
    const float bandwidth    = *apvts.getRawParameterValue ("bandwidth");
    const float masterMix    = *apvts.getRawParameterValue ("masterMix");

    tuner.setSweepTime (tuneSpeed);
    noiseGen.setLevel  (staticAmount * 0.4f);
    drift.setAmount    (driftAmount);
    burstGen.setDensity (burstDensity);

    // bandwidth 2000..5000 Hz → Q 8..3
    const float q = juce::jmap (bandwidth, 2000.0f, 5000.0f, 8.0f, 3.0f);
    sweepFilter.setQ (q);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // ── Advance modulation ──────────────────────────────────────────────
        tuner.advance();
        const float noise = noiseGen.process();

        float freq = tuner.getCurrentFrequency() + drift.process() + noise;
        freq = juce::jlimit (200.0f, 6000.0f, freq);

        sweepFilter.setFrequency (freq);
        burstGen.setCurrentFrequency (freq);


        // ── Stereo signal chain ─────────────────────────────────────────────
        float srcL, srcR;
        audioFilePlayer.getNextStereoSample (srcL, srcR);

        for (int ch = 0; ch < std::min (totalNumOutputChannels, 2); ++ch)
        {
            const float src = (ch == 0) ? srcL : srcR;

            // 1. BitCrusher — sample-rate and bit-depth reduction
            float sig = bitCrusher.processSample (src, ch);

            // 2. Station burst (gate the crushed signal + noise through an envelope)
            sig = burstGen.process (sig  );

            // 3. AM distortion (tanh soft saturation, drive = 2)
            sig = std::tanh (sig * 2.0f);

            // 4. Band limit (200 Hz – 5 kHz)
            sig = bandLimiter.processSample (sig, ch);

            // 5. Master mix: blend dry source with fully processed signal
            buffer.getWritePointer (ch)[sample] = src * (1.0f - masterMix) + sig * masterMix;
        }
    }

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
