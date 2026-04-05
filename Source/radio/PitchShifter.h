#pragma once

class PitchShifter
{
public:
    enum class Mode { Granular, TimeStretch };

    void  setSampleRate (double sr);
    void  setMode       (Mode m);
    // Granular: any ratio. TimeStretch: use 0.5 / 1.0 / 2.0 / 3.0 for clean harmonics.
    void  setRatio      (float ratio);
    float processSample (float input);

private:
    double sampleRate = 44100.0;
    float  ratio      = 2.0f;
    Mode   mode       = Mode::TimeStretch;

    static constexpr int BUF_SIZE      = 8192;
    static constexpr int BUF_MASK      = BUF_SIZE - 1;
    static constexpr int GRAIN_SIZE    = 1024;
    static constexpr int LATENCY       = GRAIN_SIZE * 2;
    static constexpr int CROSSFADE_LEN = 256;
    static constexpr int MIN_DISTANCE  = 256;
    static constexpr int MAX_DISTANCE  = BUF_SIZE - 256;

    float buffer[BUF_SIZE] = {};
    int   writePos         = 0;

    // Granular state
    float grainReadPos[2]  = {};
    int   grainClock       = 0;

    // TimeStretch state
    float tsReadPos         = 0.0f;
    float tsFadeFromPos     = 0.0f;
    int   tsCrossfadeCount  = 0;

    float readInterp (float pos) const;
    float processGranular    (float input);
    float processTimeStretch (float input);
};
