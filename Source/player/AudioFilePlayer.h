#pragma once
#include <juce_audio_formats/juce_audio_formats.h>

class AudioFilePlayer
{
public:
    AudioFilePlayer();

    void prepare (double newSampleRate);

    /** Load a specific audio file. Returns true on success. */
    bool loadFile (const juce::File& file);

    /** Finds and loads the first supported audio file inside a directory. */
    bool loadFromDirectory (const juce::File& directory);

    /** Returns the next mono sample and advances the read position.
        Loops back to the start when the file ends. Returns 0 if no file is loaded. */
    float getNextSample();

    void reset();
    bool isLoaded() const { return loaded; }

private:
    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> fileBuffer;

    double fileSampleRate   = 44100.0;
    double pluginSampleRate = 44100.0;
    double fractionalPos    = 0.0;

    bool loaded = false;
};
