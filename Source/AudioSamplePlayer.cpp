/*
  ==============================================================================

    SamplePlayer.cpp
    Created: 2 Nov 2025 10:57:22am
    Author:  bramd

  ==============================================================================
*/

/*
  ==============================================================================
    AudioSamplePlayer.cpp - Minimally atomic implementation
  ==============================================================================
*/

#include "AudioSamplePlayer.h"

AudioSamplePlayer::AudioSamplePlayer()
{
    formatManager.registerBasicFormats();
}

void AudioSamplePlayer::prepareSamplePlayer(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
}

bool AudioSamplePlayer::loadFile(const juce::File& file)
{
    auto* reader = formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReader> readerPtr(reader);

        sourceSampleRate = reader->sampleRate;

        if (currentSampleRate <= 0.0)
        {
            currentSampleRate = 44100.0;
            DBG("Warning: prepareSamplePlayer not called, defaulting to 44100 Hz");
        }

        juce::AudioBuffer<float> newBuffer;

        // Resampling logic (unchanged)
        if (std::abs(sourceSampleRate - currentSampleRate) > 0.1)
        {
            double ratio = currentSampleRate / sourceSampleRate;
            int resampledLength = static_cast<int>(std::ceil(reader->lengthInSamples * ratio));
            resampledLength += 16;

            juce::AudioBuffer<float> tempBuffer;
            tempBuffer.setSize(static_cast<int>(reader->numChannels),
                static_cast<int>(reader->lengthInSamples));
            reader->read(&tempBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);

            newBuffer.setSize(static_cast<int>(reader->numChannels), resampledLength);
            newBuffer.clear();

            juce::LagrangeInterpolator interpolator;
            for (int channel = 0; channel < tempBuffer.getNumChannels(); ++channel)
            {
                interpolator.reset();
                interpolator.process(ratio,
                    tempBuffer.getReadPointer(channel),
                    newBuffer.getWritePointer(channel),
                    resampledLength - 16);
            }

            DBG("Resampled audio from " + juce::String(sourceSampleRate) +
                " Hz to " + juce::String(currentSampleRate) + " Hz");
        }
        else
        {
            newBuffer.setSize(static_cast<int>(reader->numChannels),
                static_cast<int>(reader->lengthInSamples));
            reader->read(&newBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        }

        // Swap buffer with thread safety
        {
            const juce::ScopedLock lock(bufferLock);
            buffer = std::move(newBuffer);
            position = 0;  // Safe: happens during loading, before playback
            isLoaded = true;
        }

        return true;
    }
    return false;
}

bool AudioSamplePlayer::loadFile(const juce::String& filePath)
{
    juce::File file(filePath);

    if (!file.exists())
    {
        DBG("File not found: " + filePath);
        return false;
    }

    return loadFile(file);
}

void AudioSamplePlayer::passSampleToBuffer(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    // =========================================================================
    // HANDLE TRIGGER REQUEST - Read atomic flag from UI thread
    // =========================================================================
    if (shouldTrigger.load())
    {
        shouldTrigger.store(false);  // Clear the flag
        position = 0;     // ✅ Safe: we're on audio thread
        isPlaying = true; // ✅ Safe: we're on audio thread
    }

    // =========================================================================
    // HANDLE RANDOM TRIGGERING - Read atomic flags from UI thread
    // =========================================================================
    if (randomTriggerEnabled.load() && isLoaded)
    {
        samplesSinceLastTrigger += numSamples;

        if (samplesSinceLastTrigger >= samplesToNextTrigger)
        {
            // Trigger directly (we're on audio thread)
            position = 0;
            isPlaying = true;
            scheduleNextRandomTrigger();
        }
    }

    // =========================================================================
    // AUDIO PROCESSING - All audio thread only from here
    // =========================================================================
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
                position = 0;  // Loop back to start
            }
            else
            {
                // One-shot mode: stop playing
                isPlaying = false;
                position = 0;
                return;
            }
        }

        // Mix audio into output buffer
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float sample = buffer.getSample(channel, position) * gain;
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
        position = 0;
        isLoaded = true;
    }

    DBG("Buffer loaded: " + juce::String(buffer.getNumChannels()) + " channels, " +
        juce::String(buffer.getNumSamples()) + " samples");

    return true;
}

void AudioSamplePlayer::setLoopMode(bool shouldLoop)
{
    loopMode = shouldLoop;  // Usually set during init, rarely changed
}

bool AudioSamplePlayer::isLooping() const
{
    return loopMode;
}

// =========================================================================
// CONTROL METHODS
// =========================================================================

void AudioSamplePlayer::trigger()
{
    // Called from UI thread (button click)
    if (isLoaded)
    {
        shouldTrigger.store(true);  // ✅ Atomic: UI thread → Audio thread
    }
}

void AudioSamplePlayer::start()
{
    // Usually called from audio thread or initialization
    isPlaying = true;  // No atomic needed - audio thread only
}

void AudioSamplePlayer::stop()
{
    // Usually called from audio thread or initialization
    isPlaying = false;
    position = 0;
}

void AudioSamplePlayer::setGain(float newGain)
{
    // Called from processBlock() - audio thread only
    gain = juce::jmax(0.0f, newGain);  // No atomic needed
}

// =========================================================================
// RANDOM TRIGGERING - UI thread sets these, audio thread reads
// =========================================================================

void AudioSamplePlayer::setRandomTriggerEnabled(bool enabled)
{
    // Called from UI thread (toggle button)
    bool wasEnabled = randomTriggerEnabled.load();
    randomTriggerEnabled.store(enabled);  // ✅ Atomic: UI → Audio

    // Schedule next trigger when enabling
    if (enabled && !wasEnabled)
    {
        scheduleNextRandomTrigger();
    }
}

void AudioSamplePlayer::setRandomTriggerInterval(float minSeconds, float maxSeconds)
{
    // Called from UI thread (sliders)
    if (minSeconds < 0.1f) minSeconds = 0.1f;
    if (maxSeconds < minSeconds) maxSeconds = minSeconds;

    float oldMin = minRandomInterval.load();
    float oldMax = maxRandomInterval.load();

    minRandomInterval.store(minSeconds);  // ✅ Atomic: UI → Audio
    maxRandomInterval.store(maxSeconds);  // ✅ Atomic: UI → Audio

    // Reschedule if intervals changed and random triggering is enabled
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
    // This can be called from UI thread (when enabling) or audio thread
    // Read the atomic values
    float minSec = minRandomInterval.load();
    float maxSec = maxRandomInterval.load();

    float intervalSeconds = minSec + random.nextFloat() * (maxSec - minSec);
    samplesToNextTrigger = intervalSeconds * currentSampleRate;
    samplesSinceLastTrigger = 0.0;
}

bool AudioSamplePlayer::getIsPlaying() const
{
    return isPlaying;  // Read-only, safe enough for display
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
