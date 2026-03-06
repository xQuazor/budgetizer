#include "AudioFilePlayer.h"

AudioFilePlayer::AudioFilePlayer()
{
    formatManager.registerBasicFormats(); // WAV, AIFF, FLAC, OGG
}

void AudioFilePlayer::prepare (double newSampleRate)
{
    pluginSampleRate = newSampleRate;
    reset();
}

bool AudioFilePlayer::loadFile (const juce::File& file)
{
    if (! file.existsAsFile())
        return false;

    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));
    if (reader == nullptr)
        return false;

    fileSampleRate = reader->sampleRate;

    fileBuffer.setSize ((int) reader->numChannels, (int) reader->lengthInSamples);
    reader->read (&fileBuffer, 0, (int) reader->lengthInSamples, 0, true, true);

    loaded = true;
    fractionalPos = 0.0;
    return true;
}

bool AudioFilePlayer::loadFromDirectory (const juce::File& directory)
{
    if (! directory.isDirectory())
        return false;

    juce::Array<juce::File> files;
    directory.findChildFiles (files, juce::File::findFiles, false,
                              "*.wav;*.aif;*.aiff;*.flac;*.ogg;*.mp3");
    files.sort();

    for (auto& f : files)
        if (loadFile (f))
            return true;

    return false;
}

float AudioFilePlayer::getNextSample()
{
    if (! loaded || fileBuffer.getNumSamples() == 0)
        return 0.0f;

    const int numSamples  = fileBuffer.getNumSamples();
    const int numChannels = fileBuffer.getNumChannels();

    // Linear interpolation between adjacent samples
    const int   pos0 = int (fractionalPos) % numSamples;
    const int   pos1 = (pos0 + 1) % numSamples;
    const float frac = float (fractionalPos - std::floor (fractionalPos));

    // Average all channels → mono
    float sample = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch)
        sample += (1.0f - frac) * fileBuffer.getSample (ch, pos0)
                + frac          * fileBuffer.getSample (ch, pos1);
    sample /= float (numChannels);

    // Advance position at the file's sample rate relative to the plugin rate
    fractionalPos += fileSampleRate / pluginSampleRate;
    if (fractionalPos >= numSamples)
        fractionalPos -= numSamples; // loop

    return sample;
}

void AudioFilePlayer::reset()
{
    fractionalPos = 0.0;
}
