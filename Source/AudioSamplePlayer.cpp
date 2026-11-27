/*
  ==============================================================================

    SamplePlayer.cpp
    Created: 2 Nov 2025 10:57:22am
    Author:  bramd

  ==============================================================================
*/

#include "AudioSamplePlayer.h"

AudioSamplePlayer::AudioSamplePlayer()
{
    
}

void AudioSamplePlayer::prepareSamplePlayer(double sampleRate, int samplesPerBlock)
{
    gainSmoother.reset(sampleRate, 0.05);
    currentSampleRate = sampleRate;
}



void AudioSamplePlayer::passSampleToBuffer(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    
    if (shouldTrigger.load())
    {
        shouldTrigger.store(false);  
        position = 0;     
        isPlaying = true; 
    }

    if (randomTriggerEnabled.load() && isLoaded)
    {
        samplesSinceLastTrigger += numSamples;

        if (samplesSinceLastTrigger >= samplesToNextTrigger)
        {
            
            position = 0;
            isPlaying = true;
            scheduleNextRandomTrigger();
        }
    }

   
    const juce::ScopedLock lock(bufferLock); // Lock for entire buffer reading operation

    if (!isLoaded || !isPlaying || buffer.getNumSamples() == 0)
    {
        return;
    }

    const int bufferLength = buffer.getNumSamples();
    const int numChannels = juce::jmin(buffer.getNumChannels(), outputBuffer.getNumChannels());

    if (bufferLength == 0 || numChannels == 0)
    {
        return;
    }

    for (int i = 0; i < numSamples; ++i)
    {
        if (position >= bufferLength)
        {
            if (loopMode)
            {
                position = 0;  
            }
            else
            {
                
                isPlaying = false;
                position = 0;
                return;
            }
        }

        const float g = gainSmoother.getNextValue();
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float sample = buffer.getSample(channel, position) * g;
            outputBuffer.addSample(channel, startSample + i, sample);
        }

        ++position;
    }
}
bool AudioSamplePlayer::loadBuffer(const juce::AudioBuffer<float>& newBuffer, double newSourceSampleRate)
{
    if (newBuffer.getNumSamples() == 0 || newBuffer.getNumChannels() == 0)
        return false;

    juce::AudioBuffer<float> bufferCopy;
    bufferCopy.makeCopyOf(newBuffer);

    {
        const juce::ScopedLock lock(bufferLock);
        buffer = std::move(bufferCopy);
        sourceSampleRate = newSourceSampleRate;
        
    }

    isLoaded.store(true);
    DBG("Buffer loaded: " + juce::String(buffer.getNumChannels()) + " channels, " +
        juce::String(buffer.getNumSamples()) + " samples");

    return true;
}

void AudioSamplePlayer::setLoopMode(bool shouldLoop)
{
    loopMode = shouldLoop;  
}

bool AudioSamplePlayer::isLooping() const
{
    return loopMode;
}



void AudioSamplePlayer::trigger()
{
    
    if (isLoaded)
    {
        shouldTrigger.store(true);  
    }
}

void AudioSamplePlayer::start()
{
    
    isPlaying = true;  
}

void AudioSamplePlayer::stop()
{
    
    isPlaying = false;
    position = 0;
}

void AudioSamplePlayer::setGain(float newGain)
{
    
    gainSmoother.setTargetValue(juce::jmax(0.0f, newGain));
}



void AudioSamplePlayer::setRandomTriggerEnabled(bool enabled)
{
    
    bool wasEnabled = randomTriggerEnabled.load();
    randomTriggerEnabled.store(enabled);  

    
    if (enabled && !wasEnabled)
    {
        scheduleNextRandomTrigger();
    }
}

void AudioSamplePlayer::setRandomTriggerInterval(float minSeconds, float maxSeconds)
{
   
    if (minSeconds < 0.1f) minSeconds = 0.1f;
    if (maxSeconds < minSeconds) maxSeconds = minSeconds;

    float oldMin = minRandomInterval.load();
    float oldMax = maxRandomInterval.load();

    minRandomInterval.store(minSeconds);  
    maxRandomInterval.store(maxSeconds);  

    
    if (randomTriggerEnabled.load() && (oldMin != minSeconds || oldMax != maxSeconds))
    {
        scheduleNextRandomTrigger();
    }
}

bool AudioSamplePlayer::isRandomTriggerEnabled() const
{
    return randomTriggerEnabled.load();
}

void AudioSamplePlayer::getRandomTriggerInterval(float& minSeconds, float& maxSeconds) const
{
    minSeconds = minRandomInterval.load();
    maxSeconds = maxRandomInterval.load();
}

void AudioSamplePlayer::scheduleNextRandomTrigger()
{
    
    float minSec = minRandomInterval.load();
    float maxSec = maxRandomInterval.load();

    float intervalSeconds = minSec + random.nextFloat() * (maxSec - minSec);
    samplesToNextTrigger = intervalSeconds * currentSampleRate;
    samplesSinceLastTrigger = 0.0;
}

bool AudioSamplePlayer::getIsPlaying() const
{
    return isPlaying;  
}

void AudioSamplePlayer::clear()
{
    buffer.clear();
    position = 0;
    isLoaded = false;
    isPlaying = false;
    sourceSampleRate = 0.0;
    randomTriggerEnabled.store(false);
    samplesSinceLastTrigger = 0.0;
    samplesToNextTrigger = 0.0;
}
