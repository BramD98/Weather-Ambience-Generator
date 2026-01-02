/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ProtectYourEars.h"

//==============================================================================
WeatherAmbienceGeneratorAudioProcessor::WeatherAmbienceGeneratorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	), apvts(*this, nullptr, "Parameters", Parameters::createParameterLayout()), params(apvts)
#endif
{
	weatherManager = std::make_unique<WeatherManager>(m_apiKey);
	FileLoaderRegisterPlayers();
	
}

WeatherAmbienceGeneratorAudioProcessor::~WeatherAmbienceGeneratorAudioProcessor()
{
}

//==============================================================================
const juce::String WeatherAmbienceGeneratorAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool WeatherAmbienceGeneratorAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool WeatherAmbienceGeneratorAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool WeatherAmbienceGeneratorAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double WeatherAmbienceGeneratorAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int WeatherAmbienceGeneratorAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int WeatherAmbienceGeneratorAudioProcessor::getCurrentProgram()
{
	return 0;
}

void WeatherAmbienceGeneratorAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String WeatherAmbienceGeneratorAudioProcessor::getProgramName(int index)
{
	return {};
}

void WeatherAmbienceGeneratorAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void WeatherAmbienceGeneratorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	params.prepareToPlay(sampleRate);
	params.reset();

	fileLoader.setSampleRate(sampleRate);
	PrepareSamplePlayers(sampleRate,samplesPerBlock);
	
}

void WeatherAmbienceGeneratorAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WeatherAmbienceGeneratorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void WeatherAmbienceGeneratorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	params.update();
	
	for (int channel = 0; channel < totalNumInputChannels; ++channel)
	{
		auto* channelData = buffer.getWritePointer(channel);

		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
		{
			params.smoothen();
		}
	}
	static int counter = 0;
	if (++counter % 100 == 0)  // Print every 100 blocks to avoid spam
	{
		//DBG("windGain: " << params.windGain);
	}
	
	ProcessSamplePlayers(buffer); //Puts samples of all sampleplayers on the buffer
	


	protectYourEars(buffer);
}

//==============================================================================
bool WeatherAmbienceGeneratorAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WeatherAmbienceGeneratorAudioProcessor::createEditor()
{
	return new WeatherAmbienceGeneratorAudioProcessorEditor(*this);
	//return new juce::GenericAudioProcessorEditor(this);
}

//==============================================================================
void WeatherAmbienceGeneratorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void WeatherAmbienceGeneratorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

void WeatherAmbienceGeneratorAudioProcessor::FileLoaderRegisterPlayers()
{
	fileLoader.registerPlayer("Calm Wind", &calmWindPlayer);
	fileLoader.registerPlayer("Mid Wind", &midWindPlayer);
	fileLoader.registerPlayer("Strong Wind", &strongWindPlayer);

	fileLoader.registerPlayer("Calm Fire", &calmFirePlayer);
	fileLoader.registerPlayer("Strong Fire", &strongFirePlayer);

	fileLoader.registerPlayer("Thunder", &thunderPlayer);
}

void WeatherAmbienceGeneratorAudioProcessor::PrepareSamplePlayers(double sampleRate, int samplesPerBlock)
{
	calmWindPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File file("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Wind_Calm.wav");
	fileLoader.loadFileForPlayer("Calm Wind", file);
	midWindPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);

	strongWindPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	calmFirePlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	strongFirePlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);

	thunderPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	thunderPlayer.setLoopMode(false);


	calmWindPlayer.start();
	midWindPlayer.start();
	strongWindPlayer.start();
	calmFirePlayer.start();
	strongFirePlayer.start();
}

void WeatherAmbienceGeneratorAudioProcessor::ProcessSamplePlayers(juce::AudioBuffer<float>& buffer)
{
	calmWindPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	midWindPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	strongWindPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	calmFirePlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	strongFirePlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());

	thunderPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new WeatherAmbienceGeneratorAudioProcessor();
}
