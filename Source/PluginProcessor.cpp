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
{
    licensed.store(LicenseValidator::checkSaved().valid);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
AudioPluginAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Bit Crusher Layout
    params.push_back (std::make_unique<juce::AudioParameterBool> ("bypass", "Bypass", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> ("noise",  "Noise",  true));
    params.push_back (std::make_unique<juce::AudioParameterBool> ("gated",  "Gated",  false));
    params.push_back (std::make_unique<juce::AudioParameterBool> ("smooth", "Smooth", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> ("radio", "Radio", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "bitDepth",    "Bit Depth",    0,    24,    8));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "sampleReductionRate",    "Sample Reduction Rate",    0,    22,    1));

    // Radio Layout
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "drift", "drift", 0.0f,   2500.0f,    500.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "depth", "depth", 0.0f,   100.0f,    25.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "interval",       "Interval",       0.0f, 2000.0f,  500.0f));

    // Master Knob
    params.push_back (std::make_unique<juce::AudioParameterBool> ("useAudioInput", "useAudioInput", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> ("sync", "Sync", false));
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

    // Mac Mini
    // /Users/dovis/CLionProjects/degrainator/music

    // Macbook air
    // /Users/dovydas/CLionProjects/demo/music
    audioFilePlayer.loadFromDirectory (juce::File ("/Users/dovydas/CLionProjects/demo/music"));

    bitCrusher.prepare (sampleRate);
    bitCrusher.setFeedback (0.0f);
    pitchModulator.prepare (sampleRate);
    radioEffect.setSampleRate(sampleRate);
    complexLFO.setSampleRate (sampleRate);
    chorus.prepare (sampleRate, 2);

    wobblePitchL.prepare (sampleRate);
    wobblePitchR.prepare (sampleRate);
    wobblePitchL.setDelayTime (0.005f);
    wobblePitchR.setDelayTime (0.005f);
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

    if (!licensed.load())
    {
        buffer.clear();
        return;
    }

    if (*apvts.getRawParameterValue ("bypass"))
        return;

    juce::ScopedNoDenormals noDenormals;

    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples             = buffer.getNumSamples();

    // Bit Crusher Chain
    const int sampleRateReduction    = *apvts.getRawParameterValue ("sampleReductionRate");
    const int bitDepth = *apvts.getRawParameterValue ("bitDepth");
    const bool smooth = *apvts.getRawParameterValue ("smooth");

    // Master Chain
    const float drive    = *apvts.getRawParameterValue ("drive");
    const float masterMix    = *apvts.getRawParameterValue ("masterMix");
    const bool useAudioInput = *apvts.getRawParameterValue ("useAudioInput");

    if (auto* playHead = getPlayHead())
        if (auto pos = playHead->getPosition())
            if (auto bpm = pos->getBpm())
                radioEffect.setBpm (static_cast<float> (*bpm));

    //Radio Chain
    const bool  radio          = *apvts.getRawParameterValue ("radio");
    const float drift          = *apvts.getRawParameterValue ("drift");
    const float depth          = *apvts.getRawParameterValue ("depth");
    const float interval       = *apvts.getRawParameterValue ("interval");
    const bool  sync           = *apvts.getRawParameterValue ("sync");
    const bool  noiseEnabled   = *apvts.getRawParameterValue ("noise");
    const bool  gated          = *apvts.getRawParameterValue ("gated");

    bitCrusher.setBitRate (bitDepth);
    bitCrusher.setReductionFactor(sampleRateReduction);
    bitCrusher.setInterpolated(smooth);
    radioEffect.setTriangleDepth   (depth);
    radioEffect.setNoiseLevel      (noiseEnabled ? depth : 0.0f);
    radioEffect.setDropoutAmount   (depth);
    radioEffect.setMultipathDelay  (3.0f);
    radioEffect.setEmphasis        (1.0f);
    radioEffect.setMultipathMix    (1.0f);
    radioEffect.setLimiterCeiling  (1.0f);

    if (sync)
    {
        static const Radio::NoteDivision kDivisions[5] = {
            Radio::NoteDivision::Sixteenth,
            Radio::NoteDivision::Eighth,
            Radio::NoteDivision::Quarter,
            Radio::NoteDivision::Half,
            Radio::NoteDivision::Whole,
        };
        const int divIndex = std::clamp (static_cast<int>(std::round(interval)), 0, 4);
        radioEffect.setHoldNoteDivision(kDivisions[divIndex]);
        radioEffect.setGateNoteDivision(kDivisions[divIndex]);
    }
    else
    {
        radioEffect.setHoldCustomTimeMs(interval);
        radioEffect.setGateCustomTimeMs(interval);
    }
    radioEffect.setTempoSyncHold(sync);
    radioEffect.setTempoSyncGate(sync);
    radioEffect.setGateEnabled(gated);

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

    const float sr = static_cast<float>(getSampleRate());

    for (int sample = 0; sample < numSamples; ++sample)
    {

        // ── Stereo signal chain ─────────────────────────────────────────────
        float srcL, srcR;
        if (useAudioInput)
        {
            srcL = buffer.getReadPointer (0)[sample];
            srcR = (numChannels > 1) ? buffer.getReadPointer (1)[sample] : srcL;
        }
        else
        {
            audioFilePlayer.getNextStereoSample (srcL, srcR);
        }

        // Advance wobble LFO phases once per sample
        wobblePitchPhase = std::fmod(wobblePitchPhase + 0.1f  / sr, 1.0f);
        wobbleVolPhase   = std::fmod(wobbleVolPhase   + 0.07f / sr, 1.0f);

        for (int ch = 0; ch < numChannels; ++ch)
        {
            const float src = (ch == 0) ? srcL : srcR;
            drySig[ch][sample] = src;
            float mod = complexLFO.returnModulation();

            // 1. BitCrusher — sample-rate and bit-depth reduction
            float sig = bitCrusher.processSample (src, ch);

            if (radio) {
                radioEffect.setCurrentFrequency(drift);
                sig = radioEffect.processSample(sig);
            }

            // AM distortion (tanh soft saturation) — accumulate pre/post RMS
            preSumSq[ch]  += sig * sig;
            sig = std::tanh (sig * drive);
            postSumSq[ch] += sig * sig;

            // Pitch wobble + volume modulation (tied to Noise button)
            if (noiseEnabled)
            {
                const float pitchSin = std::sin(wobblePitchPhase * juce::MathConstants<float>::twoPi);
                const float volSin   = std::sin(wobbleVolPhase   * juce::MathConstants<float>::twoPi);

                auto& pm = (ch == 0) ? wobblePitchL : wobblePitchR;
                pm.applyComplexLFO(pitchSin * (ch == 0 ? 1.0f : -1.0f));
                sig = pm.processSample(sig);

                sig *= std::pow(10.0f, volSin / 20.0f);  // ±1 dB
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
