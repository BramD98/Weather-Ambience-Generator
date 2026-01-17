/*
  ==============================================================================

    SamplePlayer.h
    Created: 2 Nov 2025 10:57:22am
    Author:  bramd

  ==============================================================================
*/
#pragma once
#include "JuceHeader.h"

class AudioSamplePlayer
{
public:
    AudioSamplePlayer();

    void prepareSamplePlayer(double sampleRate, int samplesPerBlock);

    bool loadFile(const juce::File& file);
    bool loadFile(const juce::String& filePath);
    bool loadBuffer(const juce::AudioBuffer<float>& newBuffer, double newSourceSampleRate);

    void passSampleToBuffer(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples);

    // Thread-safe control methods
    void trigger();      // Can be called from UI thread - sets atomic flag
    void start();        // Called from audio thread (or initialization)
    void stop();         // Called from audio thread (or initialization)
    void setGain(float newGain);  // Called from audio thread (processBlock)

    // Loop mode
    void setLoopMode(bool shouldLoop);  // Usually set during initialization
    bool isLooping() const;

    // Random triggering - these ARE called from UI thread
    void setRandomTriggerEnabled(bool enabled);
    void setRandomTriggerInterval(float minSeconds, float maxSeconds);
    bool isRandomTriggerEnabled() const;
    void getRandomTriggerInterval(float& minSeconds, float& maxSeconds) const;

    // State queries
    bool getIsPlaying() const;

    void clear();

private:
    void scheduleNextRandomTrigger();

    // Audio data - protected by lock during loading
    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> buffer;
    juce::CriticalSection bufferLock;  // Only for loading, not for playback

    // Sample rates
    double sourceSampleRate = 0.0;
    double currentSampleRate = 0.0;

    // =========================================================================
    // AUDIO THREAD ONLY - NO ATOMICS NEEDED
    // These are only modified in passSampleToBuffer() or processBlock()
    // =========================================================================
    int position = 0;
    bool isPlaying = false;
    float gain = 1.0f;

    // File loading state (only modified during loading, safe to read during playback)
    bool isLoaded = false;

    // Loop mode (set once during init, rarely changed)
    bool loopMode = true;

    // =========================================================================
    // CROSS-THREAD COMMUNICATION - ATOMICS REQUIRED
    // UI thread writes, audio thread reads
    // =========================================================================
    std::atomic<bool> shouldTrigger{ false };  // UI → Audio: trigger request
    std::atomic<bool> randomTriggerEnabled{ false };  // UI → Audio: random mode
    std::atomic<float> minRandomInterval{ 5.0f };  // UI → Audio: min interval
    std::atomic<float> maxRandomInterval{ 10.0f };  // UI → Audio: max interval

    // =========================================================================
    // AUDIO THREAD ONLY - Random triggering state
    // =========================================================================
    double samplesSinceLastTrigger = 0.0;
    double samplesToNextTrigger = 0.0;
    juce::Random random;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSamplePlayer)
};