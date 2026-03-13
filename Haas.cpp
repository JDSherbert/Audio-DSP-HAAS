// Copyright (c) 2024 JDSherbert. All rights reserved.
 
#include "Haas.h"
 
#include <cassert>
#include <cmath>
 
// ======================================================================= //
 
Sherbert::Haas::Haas(int delayInMilliseconds, DelayedChannel delayedChannel, float sampleRate)
    : delayInMilliseconds(delayInMilliseconds)
    , delayInSamples(static_cast<size_t>(delayInMilliseconds * sampleRate / 1000.0f))
    , delayedChannel(delayedChannel)
    , sampleRate(sampleRate)
    , writeIndex(0)
{
    assert(delayInMilliseconds > 0);
    assert(sampleRate > 0.0f);
 
    delayBuffer.resize(delayInSamples, 0.0f);
}
 
// ======================================================================= //
 
void Sherbert::Haas::ProcessSample(float leftIn, float rightIn,
                                    float& leftOut, float& rightOut)
{
    // === HOW THE HAAS EFFECT WORKS =====================================
    //
    // The leading channel passes through to output immediately — no delay.
    // The delayed channel reads from the circular buffer (which holds samples
    // from 'delayInSamples' frames ago) and then writes the current input
    // into the buffer for future playback.
    //
    // The result is that both channels carry the same signal, but one lags
    // behind by the configured delay time. The brain interprets this timing
    // difference as spatial information — the sound appears to originate
    // from the side of the leading (undelayed) channel.
    //
    // The circular buffer works identically to the reverb implementation:
    // a fixed-size array with a write index that wraps at the buffer end,
    // so no memory is allocated or freed during processing.
    // ====================================================================
 
    const float delayedSample = delayBuffer[writeIndex];
 
    if (delayedChannel == DelayedChannel::Right)
    {
        // Left channel leads — right channel is delayed
        leftOut  = leftIn;
        delayBuffer[writeIndex] = rightIn;
        rightOut = delayedSample;
    }
    else
    {
        // Right channel leads — left channel is delayed
        rightOut = rightIn;
        delayBuffer[writeIndex] = leftIn;
        leftOut  = delayedSample;
    }
 
    writeIndex = (writeIndex + 1) % delayInSamples;
}
 
// ======================================================================= //
 
void Sherbert::Haas::reset()
{
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);
    writeIndex = 0;
}
 
// ======================================================================= //
 
void Sherbert::Haas::setDelayInMilliseconds(int newDelayInMilliseconds)
{
    assert(newDelayInMilliseconds > 0);
    delayInMilliseconds = newDelayInMilliseconds;
    delayInSamples = static_cast<size_t>(delayInMilliseconds * sampleRate / 1000.0f);
    delayBuffer.resize(delayInSamples, 0.0f);
    reset();
}
 
// ======================================================================= //
 
void Sherbert::Haas::setDelayedChannel(DelayedChannel newDelayedChannel) noexcept
{
    delayedChannel = newDelayedChannel;
}
 
// ======================================================================= //
