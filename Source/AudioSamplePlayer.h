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

  
    bool loadBuffer(const juce::AudioBuffer<float>& newBuffer, double newSourceSampleRate);

    void passSampleToBuffer(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples);

    
    void trigger();      
    void start();      
    void stop();         
    void setGain(float newGain); 


    void setLoopMode(bool shouldLoop); 
    bool isLooping() const;

    
    void setRandomTriggerEnabled(bool enabled);
    void setRandomTriggerInterval(float minSeconds, float maxSeconds);
    bool isRandomTriggerEnabled() const;
    void getRandomTriggerInterval(float& minSeconds, float& maxSeconds) const;


    bool getIsPlaying() const;

    void clear();

private:
    void scheduleNextRandomTrigger();

   
    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> buffer;
    juce::CriticalSection bufferLock;  


    double sourceSampleRate = 0.0;
    double currentSampleRate = 0.0;

    
    int position = 0;
    bool isPlaying = false;
    float gain = 1.0f;

    
   std::atomic<bool> isLoaded = false;

    
    bool loopMode = true;

   
    std::atomic<bool> shouldTrigger{ false };  
    std::atomic<bool> randomTriggerEnabled{ false };  
    std::atomic<float> minRandomInterval{ 5.0f };  
    std::atomic<float> maxRandomInterval{ 10.0f }; 

    
    double samplesSinceLastTrigger = 0.0;
    double samplesToNextTrigger = 0.0;
    juce::Random random;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSamplePlayer)
};