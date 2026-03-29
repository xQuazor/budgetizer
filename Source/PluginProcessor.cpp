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


    params.push_back (std::make_unique<juce::AudioParameterBool> ("smooth", "Smooth", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> ("radio", "Radio", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "bitDepth",    "Bit Depth",    0,    24,    8));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "sampleReductionRate",    "Sample Reduction Rate",    0,    22,    1));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "drive",    "Drive",    1.0f,    10.0f,    2.0f));
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
    bitCrusher.setFeedback (0.0f);
    pitchModulator.prepare (sampleRate);

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
    const float drive    = *apvts.getRawParameterValue ("drive");
    const int sampleRateReduction    = *apvts.getRawParameterValue ("sampleReductionRate");
    const int bitDepth = *apvts.getRawParameterValue ("bitDepth");
    const bool radio = *apvts.getRawParameterValue ("radio");
    const bool smooth = *apvts.getRawParameterValue ("smooth");

    bitCrusher.setBitRate (bitDepth);
    bitCrusher.setReductionFactor(sampleRateReduction);
    bitCrusher.setInterpolated(smooth);

    tuner.setSweepTime (tuneSpeed);
    noiseGen.setLevel  (staticAmount * 0.4f);
    drift.setAmount    (driftAmount);
    burstGen.setDensity (burstDensity);

    // bandwidth 2000..5000 Hz → Q 8..3
    const float q = juce::jmap (bandwidth, 2000.0f, 5000.0f, 8.0f, 3.0f);
    sweepFilter.setQ (q);

    // Temporary per-channel storage so we can level-match tanh after the block
    const int numChannels = std::min (totalNumOutputChannels, 2);
    std::vector<float> drySig[2], wetSig[2];
    for (int ch = 0; ch < numChannels; ++ch)
    {
        drySig[ch].resize (numSamples);
        wetSig[ch].resize (numSamples);
    }
    float preSumSq[2]  = {0.0f, 0.0f};   // RMS accumulator before tanh
    float postSumSq[2] = {0.0f, 0.0f};   // RMS accumulator after tanh

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

        for (int ch = 0; ch < numChannels; ++ch)
        {
            const float src = (ch == 0) ? srcL : srcR;
            drySig[ch][sample] = src;

            // 1. BitCrusher — sample-rate and bit-depth reduction
            float sig = bitCrusher.processSample (src, ch);

            // 2. Station burst (gate the crushed signal + noise through an envelope)
            if (radio)
                sig = burstGen.process (sig);

            // 3. AM distortion (tanh soft saturation) — accumulate pre/post RMS
            preSumSq[ch]  += sig * sig;
            sig = std::tanh (sig * drive);
            postSumSq[ch] += sig * sig;

            if (radio)
            {
                // 4. Band limit (200 Hz – 5 kHz)
                sig = bandLimiter.processSample (sig, ch);
            }

            wetSig[ch][sample] = sig;
        }
    }

    // 5. Apply block-level RMS compensation then master mix
    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float gain = computeTanhLevelMatchGain (preSumSq[ch], postSumSq[ch]);
        auto* out = buffer.getWritePointer (ch);
        for (int s = 0; s < numSamples; ++s)
            out[s] = drySig[ch][s] * (1.0f - masterMix) + wetSig[ch][s] * gain * masterMix;
    }

    for (int ch = 2; ch < totalNumOutputChannels; ++ch)
        buffer.clear (ch, 0, numSamples);
}

//==============================================================================
float AudioPluginAudioProcessor::computeTanhLevelMatchGain (float preSumSq, float postSumSq)
{
    if (postSumSq < 1.0e-12f) return 1.0f;
    return std::sqrt (preSumSq / postSumSq);
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
