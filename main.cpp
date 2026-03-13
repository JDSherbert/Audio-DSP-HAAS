// Copyright (c) 2024 JDSherbert. All rights reserved.
 
#include <cmath>
#include <iostream>
#include <vector>
 
#include "Haas.h"
 
// ======================================================================= //
 
// Generates a single sample of a sine wave at the given frequency and time.
// frequency - Hz (e.g. 440.0 = concert A)
// time      - position in seconds (e.g. sampleIndex / sampleRate)
float GenerateInputSignal(float frequency, float time)
{
    return std::sin(2.0f * static_cast<float>(M_PI) * frequency * time);
}
 
// ======================================================================= //
 
int main()
{
    // == Signal Parameters ==============================================
    const int   sampleRate = 44100;   // 44.1 kHz
    const float frequency  = 440.0f;  // Concert A
    const float duration   = 0.1f;    // 0.1 seconds
 
    const int numSamples = static_cast<int>(sampleRate * duration);
 
    // == Generate Mono Sine Wave ========================================
    // The Haas effect is typically applied to a mono source to create
    // perceived stereo width. Both channels receive the same input signal.
    std::vector<float> monoSignal(numSamples);
    for (int i = 0; i < numSamples; ++i)
        monoSignal[i] = GenerateInputSignal(frequency, i / static_cast<float>(sampleRate));
 
    // == Demonstrate: Right Channel Delayed (Left Leads) ================
    // 20ms delay — well within the 1-40ms Haas range.
    // The perceived image will appear to come from the left.
    Sherbert::Haas haas(20, Sherbert::Haas::DelayedChannel::Right, static_cast<float>(sampleRate));
 
    const int printCount = 10;
 
    std::cout << "--- Right Channel Delayed, 20ms (sound perceived left) ---\n\n";
 
    for (int i = 0; i < printCount; ++i)
    {
        const float in = monoSignal[i];
        float leftOut  = 0.0f;
        float rightOut = 0.0f;
 
        haas.ProcessSample(in, in, leftOut, rightOut);
 
        std::cout << "Sample " << i
                  << "  |  In: "    << in
                  << "  |  L: "     << leftOut
                  << "  |  R: "     << rightOut
                  << "\n";
    }
 
    // == Demonstrate: Left Channel Delayed (Right Leads) ==============
    // Reset and flip the delayed channel — perceived image moves right.
    haas.reset();
    haas.setDelayedChannel(Sherbert::Haas::DelayedChannel::Left);
 
    std::cout << "\n--- Left Channel Delayed, 20ms (sound perceived right) ---\n\n";
 
    for (int i = 0; i < printCount; ++i)
    {
        const float in = monoSignal[i];
        float leftOut  = 0.0f;
        float rightOut = 0.0f;
 
        haas.ProcessSample(in, in, leftOut, rightOut);
 
        std::cout << "Sample " << i
                  << "  |  In: "    << in
                  << "  |  L: "     << leftOut
                  << "  |  R: "     << rightOut
                  << "\n";
    }
 
    // == Demonstrate: Delay Time Comparison ==========================
    // Show how different delay times affect the output buffer offset.
    // At 5ms the channels diverge later than at 20ms.
    haas.reset();
    haas.setDelayedChannel(Sherbert::Haas::DelayedChannel::Right);
    haas.setDelayInMilliseconds(5);
 
    std::cout << "\n--- Right Channel Delayed, 5ms (tighter image) ---\n\n";
 
    for (int i = 0; i < printCount; ++i)
    {
        const float in = monoSignal[i];
        float leftOut  = 0.0f;
        float rightOut = 0.0f;
 
        haas.ProcessSample(in, in, leftOut, rightOut);
 
        std::cout << "Sample " << i
                  << "  |  In: "    << in
                  << "  |  L: "     << leftOut
                  << "  |  R: "     << rightOut
                  << "\n";
    }
 
    return 0;
}
 
// ======================================================================= //
