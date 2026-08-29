// Copyright (c) 2024 JDSherbert. All rights reserved.
 
#pragma once
 
#include <vector>
 
// ======================================================================= //
// Haas
//
// Implements the Haas effect (also known as the precedence effect), a
// psychoacoustic phenomenon that creates a sense of stereo width by
// introducing a short delay between the left and right audio channels.
//
// When the same sound reaches both ears with a delay of 1-40ms, the brain
// perceives the sound as coming from the direction of the ear that heard it
// first, even if the delayed channel is equal or slightly louder. This is
// exploited in audio production to create the impression of a wider, more
// spacious stereo image from a mono source.
//
// This implementation delays one channel relative to the other. The leading
// channel passes through unmodified; the delayed channel is held in a
// circular buffer and output after the configured delay time.
//
// Effective delay range: 1ms ~ 40ms.
// Below 1ms: perceived as a comb filter/phase effect rather than space.
// Above 40ms: perceived as a distinct echo rather than spatial widening.
//
// Usage:
//   Sherbert::Haas haas(20, Sherbert::Haas::DelayedChannel::Right, 44100.0f);
//   float leftOut, rightOut;
//   haas.ProcessSample(leftIn, rightIn, leftOut, rightOut);
//
// ======================================================================= //
 
namespace Sherbert
{
    class Haas
    {

    public:
 
        // ------------------------------------------------------------------
        // DelayedChannel
        //
        // Controls which channel receives the delay.
        //
        // Right: Left channel leads -> sound perceived as coming from the left.
        // Left:  Right channel leads -> sound perceived as coming from the right.
        // ------------------------------------------------------------------
 
        enum class DelayedChannel { Left, Right };
 
        // ------------------------------------------------------------------
        // Constructor
        //
        // delayInMilliseconds - Delay time applied to the delayed channel.
        //                       Effective range: 1 – 40ms.
        //                       Must be > 0.
        //
        // delayedChannel      - Which channel receives the delay.
        //                       See DelayedChannel enum above.
        //
        // sampleRate          - Audio sample rate in Hz (e.g. 44100).
        //                       Used to convert milliseconds to samples.
        //                       Must be > 0.
        // ------------------------------------------------------------------
 
        Haas(int delayInMilliseconds, DelayedChannel delayedChannel, float sampleRate);
 
        // ------------------------------------------------------------------
        // ProcessSample
        //
        // Processes one stereo sample pair through the Haas effect.
        // The leading channel passes through unmodified.
        // The delayed channel is output after the configured delay time.
        //
        // leftIn   - Input sample for the left channel.
        // rightIn  - Input sample for the right channel.
        // leftOut  - Output sample for the left channel (written by this method).
        // rightOut - Output sample for the right channel (written by this method).
        // ------------------------------------------------------------------
 
        void ProcessSample(float leftIn, float rightIn,
                           float& leftOut, float& rightOut);
 
        // ------------------------------------------------------------------
        // reset
        //
        // Clears the delay buffer and resets the write index.
        // Call when playback stops or the effect is bypassed.
        // ------------------------------------------------------------------
 
        void reset();
 
        // ------------------------------------------------------------------
        // Getters / Setters
        // ------------------------------------------------------------------
 
        int            getDelayInMilliseconds() const noexcept { return delayInMilliseconds; }
        DelayedChannel getDelayedChannel()      const noexcept { return delayedChannel; }
        float          getSampleRate()          const noexcept { return sampleRate; }
 
        // Note: setDelayInMilliseconds calls reset() to avoid artefacts
        // from the buffer containing samples at the old delay length.
        void setDelayInMilliseconds(int newDelayInMilliseconds);
        void setDelayedChannel(DelayedChannel newDelayedChannel) noexcept;
 
    private:
 
        int            delayInMilliseconds;
        size_t         delayInSamples;
        DelayedChannel delayedChannel;
        float          sampleRate;
 
        std::vector<float> delayBuffer;
        size_t             writeIndex = 0;
 
    };
 
}
 
// ======================================================================= //
