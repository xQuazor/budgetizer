#include "crusher.h"

void crusher::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    reset();
}

void crusher::setReductionFactor(int newFactor)
{
    if (newFactor < 1)
        newFactor = 1;

    reductionFactor = newFactor;
}

void crusher::reset()
{
    counter = 0;
    heldSample = 0.0f;
}

float crusher::processSample(float input)
{
    // Only update the held sample every N samples
    if (counter == 0)
        heldSample = input;

    counter++;
    if (counter >= reductionFactor)
        counter = 0;

    return heldSample;
}