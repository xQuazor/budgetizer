#pragma once
#include <cmath>
#include <cstdlib>

class BitCrusher
{
public:
    void setSampleRate(double newSampleRate);
    void setReductionFactor(int newFactor);
    void setBitRate(int newBitRate);
    void setDithering(bool enabled);
    void setLinearInterpolation(bool enabled);
    void setPolyBlep(bool enabled);
    void reset();
    float processSample(float signal);

private:
    double sampleRate    = 44100.0;
    int reductionFactor  = 4;
    int bitRate          = 16;
    int counter          = 0;
    float heldSample     = 0.0f;
    float prevSample     = 0.0f;
    bool dithering            = false;
    bool linearInterpolation  = true;
    bool polyBlepEnabled      = false;

    static float polyBlepFunc(float t, float dt);
};
