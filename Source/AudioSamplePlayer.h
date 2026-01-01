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
    void start();
    void stop();
    void setGain(float newGain);
    bool getIsPlaying() const;
    void clear();
private:
    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> buffer;
    juce::CriticalSection bufferLock;
    int position = 0;
    double currentSampleRate = 44100.0f;
    double sourceSampleRate = 0.0f;
    std::atomic<float> gain = 1.0f;
    bool isLoaded = false;
    bool isPlaying = false;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSamplePlayer)
};