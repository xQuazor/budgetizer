#pragma once

// Simulates multipath interference: a short, LFO-modulated delay line mixed
// with the dry signal, producing moving comb-filter notches characteristic of
// FM reception in urban environments.
class MultipathFilter
{
public:
    void  prepare(double sampleRate);
    void  setMix(float mix);          // 0-1, reflection strength (default 0.3)
    void  setBaseDelay(float ms);     // base delay in ms, 0.1-3.0
    float processSample(float x);

private:
    static constexpr int kBufSize = 1024;  // must be power of 2
    float  buffer[kBufSize] = {};
    int    writePos          = 0;
    double sampleRate        = 44100.0;

    float mix         = 0.3f;
    float baseDelayMs = 1.0f;

    // Internal LFO — slow wobble of the delay time
    float lfoPhase = 0.f;
    float lfoRate  = 0.3f;   // Hz
    float lfoDepth = 0.3f;   // ms deviation
};
