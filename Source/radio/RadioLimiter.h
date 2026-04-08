#pragma once

// Brick-wall peak limiter mimicking the heavy limiting applied by FM broadcast
// stations before transmission. Fast attack, medium release, hard ceiling.
class RadioLimiter
{
public:
    void  prepare(double sampleRate);
    void  setThreshold(float linear);  // default 0.7 (~-3 dBFS)
    float processSample(float x);

private:
    float threshold    = 0.7f;
    float envelope     = 0.f;
    float gainSmooth   = 1.f;
    float attackCoeff  = 0.f;
    float releaseCoeff = 0.f;
};
