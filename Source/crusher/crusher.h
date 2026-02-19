#ifndef MYPLUGIN_CRUSHER_H
#define MYPLUGIN_CRUSHER_H

class crusher
{
public:
    void prepare(double newSampleRate);
    void setReductionFactor(int newFactor); // e.g. 2 = half rate, 4 = quarter rate
    void reset();

    float processSample(float input);

private:
    double sampleRate = 44100.0;

    int reductionFactor = 4;   // how many samples to hold
    int counter = 0;

    float heldSample = 0.0f;
};

#endif // MYPLUGIN_CRUSHER_H
