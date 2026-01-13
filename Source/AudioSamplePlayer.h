/*
  ==============================================================================

    SamplePlayer.h
    Created: 2 Nov 2025 10:57:22am
    Author:  bramd

  ==============================================================================
*/
#pragma once

#include "JuceHeader.h"

class AudioSamplePlayer {

public:
    AudioSamplePlayer();
    ~AudioSamplePlayer() = default;
    void prepareSamplePlayer(double sampleRate, int samplesPerBlock);
    bool loadFile(const juce::File& file);
    bool loadFile(const juce::String& filePath);
    void passSampleToBuffer(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples);
    bool loadBuffer(const juce::AudioBuffer<float>& newBuffer, double newSourceSampleRate);
    void setLoopMode(bool shouldLoop);
    bool isLooping() const;
    void trigger();  // Always restarts from beginning
    void start();    // Continues/resumes playback
    void stop();
    void setGain(float newGain);
    bool getIsPlaying() const;
    void clear();
    void setRandomTriggerEnabled(bool enabled);
    void setRandomTriggerInterval(float minSeconds, float maxSeconds);
    bool isRandomTriggerEnabled() const;
    void getRandomTriggerInterval(float& minSeconds, float& maxSeconds) const;
private:
    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> buffer;
    juce::CriticalSection bufferLock;
    juce::Random random;
    std::atomic<bool> randomTriggerEnabled{ false };
    std::atomic<float> minRandomInterval{ 5.0f };
    std::atomic<float> maxRandomInterval{ 15.0f };
    double samplesSinceLastTrigger = 0.0;
    double samplesToNextTrigger = 0.0;

    void scheduleNextRandomTrigger();
    int position = 0;
    double currentSampleRate = 44100.0f;
    double sourceSampleRate = 0.0f;
    std::atomic<float> gain = 0.0f;
    bool isLoaded = false;
    bool isPlaying = false;
    bool loopMode = true;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSamplePlayer)
};