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

        // Store the original sample rate
        sourceSampleRate = reader->sampleRate;

        // If currentSampleRate hasn't been set, use a default
        if (currentSampleRate <= 0.0)
        {
            currentSampleRate = 44100.0;
            DBG("Warning: prepareSamplePlayer not called, defaulting to 44100 Hz");
        }

        // Create a temporary buffer for loading/resampling
        juce::AudioBuffer<float> newBuffer;

        // Check if we need resampling
        if (std::abs(sourceSampleRate - currentSampleRate) > 0.1)
        {
            // Calculate resampled length
            double ratio = currentSampleRate / sourceSampleRate;
            int resampledLength = static_cast<int>(std::ceil(reader->lengthInSamples * ratio));

            // Add safety margin to prevent buffer overruns
            resampledLength += 16;

            // Load original samples into temporary buffer
            juce::AudioBuffer<float> tempBuffer;
            tempBuffer.setSize(static_cast<int>(reader->numChannels),
                static_cast<int>(reader->lengthInSamples));
            reader->read(&tempBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);

            // Resample to match current sample rate
            newBuffer.setSize(static_cast<int>(reader->numChannels), resampledLength);
            newBuffer.clear();

            juce::LagrangeInterpolator interpolator;
            for (int channel = 0; channel < tempBuffer.getNumChannels(); ++channel)
            {
                interpolator.reset();
                int samplesWritten = interpolator.process(ratio,
                    tempBuffer.getReadPointer(channel),
                    newBuffer.getWritePointer(channel),
                    resampledLength - 16);

                DBG("Channel " + juce::String(channel) + " wrote " + juce::String(samplesWritten) + " samples");
            }

            DBG("Resampled audio from " + juce::String(sourceSampleRate) +
                " Hz to " + juce::String(currentSampleRate) + " Hz");
            DBG("Original length: " + juce::String(reader->lengthInSamples) +
                ", Resampled length: " + juce::String(resampledLength));
        }
        else
        {
            // No resampling needed - direct load
            newBuffer.setSize(static_cast<int>(reader->numChannels),
                static_cast<int>(reader->lengthInSamples));
            reader->read(&newBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        }

        // NOW swap the buffer atomically with thread safety
        {
            const juce::ScopedLock lock(bufferLock);
            buffer = std::move(newBuffer);
            position = 0;
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
    if (!isLoaded || !isPlaying || buffer.getNumSamples() == 0)
    {
        return;
    }

    const int bufferLength = buffer.getNumSamples();
    const int numChannels = juce::jmin(buffer.getNumChannels(), outputBuffer.getNumChannels());

    // Safety check
    if (bufferLength == 0 || numChannels == 0)
    {
        return;
    }

    for (int i = 0; i < numSamples; ++i)
    {
        if (position >= bufferLength)
        {
            position = 0;
        }

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

    // Create a copy of the buffer to swap in
    juce::AudioBuffer<float> bufferCopy;
    bufferCopy.makeCopyOf(newBuffer);

    // Now swap the buffer atomically with thread safety
    {
        const juce::ScopedLock lock(bufferLock);
        buffer = std::move(bufferCopy);
        sourceSampleRate = newSourceSampleRate;
        position = 0;
        isLoaded = true;
    }

    DBG("Buffer loaded: " + juce::String(buffer.getNumChannels()) + " channels, " +
        juce::String(buffer.getNumSamples()) + " samples at " +
        juce::String(sourceSampleRate) + " Hz");

    return true;
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
	gain = juce::jmax(0.0f, newGain);
}

bool AudioSamplePlayer::getIsPlaying()const
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
}