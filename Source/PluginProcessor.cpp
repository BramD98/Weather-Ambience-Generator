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
	thunderPlayer.setRandomTriggerEnabled(params.randomThunderToggle);
	thunderPlayer.setRandomTriggerInterval(params.minIntThunder, params.maxIntThunder);
	thunderPlayer.setGain(params.randomThunderGain);

	earthquakePlayer.setRandomTriggerEnabled(params.randomEarthquakeToggle);
	earthquakePlayer.setRandomTriggerInterval(params.minIntEarthquake, params.maxIntEarthquake);
	earthquakePlayer.setGain(params.randomEarthquakeGain);

	beastPlayer.setRandomTriggerEnabled(params.randomBeastToggle);
	beastPlayer.setRandomTriggerInterval(params.minIntBeast, params.maxIntBeast);
	beastPlayer.setGain(params.randomBeastGain);

	
	
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
	fileLoader.registerPlayer("Heavy Wind", &strongWindPlayer);

	fileLoader.registerPlayer("Calm Rain", &calmRainPlayer);
	fileLoader.registerPlayer("Mid Rain", &midRainPlayer);
	fileLoader.registerPlayer("Heavy Rain", &heavyRainPlayer);

	fileLoader.registerPlayer("Calm Fire", &calmFirePlayer);
	fileLoader.registerPlayer("Mid Fire", &midFirePlayer);
	fileLoader.registerPlayer("Heavy Fire", &heavyFirePlayer);

	fileLoader.registerPlayer("Calm City", &calmCityPlayer);
	fileLoader.registerPlayer("Mid City", &midCityPlayer);
	fileLoader.registerPlayer("Heavy City", &heavyCityPlayer);

	fileLoader.registerPlayer("Calm Water", &calmWaterPlayer);
	fileLoader.registerPlayer("Mid Water", &midWaterPlayer);
	fileLoader.registerPlayer("Heavy Water", &heavyWaterPlayer);

	fileLoader.registerPlayer("Thunder", &thunderPlayer);
	fileLoader.registerPlayer("Earthquake", &earthquakePlayer);
	fileLoader.registerPlayer("Beast", &beastPlayer);

}

void WeatherAmbienceGeneratorAudioProcessor::PrepareSamplePlayers(double sampleRate, int samplesPerBlock)
{
	//Wind Loading
	calmWindPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File windCalmFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Wind/Calm_Wind.wav");
	fileLoader.loadFileForPlayer("Calm Wind", windCalmFile);

	midWindPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File windMidFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Wind/Mid_Wind.wav");
	fileLoader.loadFileForPlayer("Mid Wind", windMidFile);

	strongWindPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File windHeavyFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Wind/Heavy_Wind.wav");
	fileLoader.loadFileForPlayer("Heavy Wind", windHeavyFile);

	//Rain Loading
	calmRainPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File rainCalmFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Rain/Calm_Rain.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Calm Rain", rainCalmFile);

	midRainPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File rainMidFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Rain/Mid_Rain.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Mid Rain", rainMidFile);

	heavyRainPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File rainHeavyFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Rain/Heavy_Rain.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Heavy Rain", rainHeavyFile);

	//Fire Loading
	calmFirePlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File fireCalmFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Fire/Calm_Fire.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Calm Fire", fireCalmFile);

	midFirePlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File fireMidFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Fire/Mid_Fire.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Mid Fire", fireMidFile);

	heavyFirePlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File fireHeavyFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Fire/Heavy_Fire.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Heavy Fire", fireHeavyFile);

	//City Loading
	calmCityPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File cityCalmFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Ambience/Calm_Amb.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Calm City", cityCalmFile);

	midCityPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File cityMidFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Ambience/Mid_Amb.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Mid City", cityMidFile);

	heavyCityPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File cityHeavyFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Ambience/Heavy_Amb.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Heavy City", cityHeavyFile);

	//Water Loading
	calmWaterPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File waterCalmFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Water/Calm_Water.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Calm Water", waterCalmFile);

	midWaterPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File waterMidFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Water/Mid_Water.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Mid Water", waterMidFile);

	heavyWaterPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File waterHeavyFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Water/Heavy_Water.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Heavy Water", waterHeavyFile);

	//Thunder Loading
	thunderPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File thunderFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/OneShots/Thunder_OneShot.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Thunder", thunderFile);
	thunderPlayer.setLoopMode(false);
	thunderPlayer.setRandomTriggerEnabled(params.randomThunderToggle);

	//Earthquake Loading
	earthquakePlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File earthquakeFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/OneShots/StoneRubble_OneShot.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Earthquake", earthquakeFile);
	earthquakePlayer.setLoopMode(false);
	earthquakePlayer.setRandomTriggerEnabled(params.randomEarthquakeToggle);

	// Beast Loading
	beastPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File beastFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/OneShots/Beast_Roar_OneShot.wav"); //C:\Users\bramd\Desktop\WeatherAmbGenerator\SFX\Rain
	fileLoader.loadFileForPlayer("Beast", beastFile);
	beastPlayer.setLoopMode(false);
	beastPlayer.setRandomTriggerEnabled(params.randomBeastToggle);
	

	calmWindPlayer.start();
	midWindPlayer.start();
	strongWindPlayer.start();

	calmRainPlayer.start();
	midRainPlayer.start();
	heavyRainPlayer.start();

	calmFirePlayer.start();
	midFirePlayer.start();
	heavyFirePlayer.start();

	calmCityPlayer.start();
	midCityPlayer.start();
	heavyCityPlayer.start();

	calmWaterPlayer.start();
	midWaterPlayer.start();
	heavyWaterPlayer.start();
}

void WeatherAmbienceGeneratorAudioProcessor::ProcessSamplePlayers(juce::AudioBuffer<float>& buffer)
{
	calmWindPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	midWindPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	strongWindPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());

	calmRainPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	midRainPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	heavyRainPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());

	calmFirePlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	midFirePlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	heavyFirePlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());

	calmCityPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	midCityPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	heavyCityPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());

	calmWaterPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	midWaterPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	heavyWaterPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());


	thunderPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	earthquakePlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
	beastPlayer.passSampleToBuffer(buffer, 0, buffer.getNumSamples());
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new WeatherAmbienceGeneratorAudioProcessor();
}
