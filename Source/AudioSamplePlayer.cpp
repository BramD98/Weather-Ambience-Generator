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
		buffer.setSize(static_cast<int>(reader->numChannels),
			static_cast<int>(reader->lengthInSamples));
		reader->read(&buffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);

		position = 0;
		isLoaded = true;
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
	if (!isLoaded || !isPlaying)
	{
		return;
	}
	const int bufferLength = buffer.getNumSamples();
	const int numChannels = juce::jmin(buffer.getNumChannels(), outputBuffer.getNumChannels());
	for (int i{ 0 }; i < numSamples; ++i)
	{
		if (position >= bufferLength)
		{
			position = 0;
		}
		for (int channel{ 0 }; channel < numChannels; ++channel)
		{
			float sample = buffer.getSample(channel, position) * gain;
			outputBuffer.addSample(channel, startSample + i, sample);
		}
		++position;
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
}