#pragma once

class RadioTuner
{
public:
    void prepare (double sampleRate);
    void setSweepTime (float timeMs);    // 50..1000
    float getCurrentFrequency() const;
    void advance();                      // call once per sample

private:
    double sampleRate = 44100.0;
    float sweepPosition = 0.0f;          // 0..1
    float sweepDelta    = 0.0f;
    float direction     = 1.0f;

    static constexpr float kStartFreq = 200.0f;
    static constexpr float kEndFreq   = 6000.0f;
};
