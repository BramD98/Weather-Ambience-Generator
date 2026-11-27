/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ProtectYourEars.h"

static juce::String loadApiKey()
{
	juce::File apiTxtFile{"C:/Users/bramd/Desktop/WeatherAmbGenerator/Weather Ambience Generator/apikey.txt"};
	if (!apiTxtFile.existsAsFile())
	{
		DBG("Api file not found");
		return {};
	}
	return apiTxtFile.loadFileAsString().trim();
}

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
	weatherManager = std::make_unique<WeatherManager>(loadApiKey());
	FileLoaderRegisterPlayers();
	lowPassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
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

	windCrossfade.setPlayers(&calmWindPlayer, &midWindPlayer, &strongWindPlayer);
	rainCrossfade.setPlayers(&calmRainPlayer, &midRainPlayer, &heavyRainPlayer);
	fireCrossfade.setPlayers(&calmFirePlayer, &midFirePlayer, &heavyFirePlayer);
	cityCrossfade.setPlayers(&calmCityPlayer, &midCityPlayer, &heavyCityPlayer);
	waterCrossfade.setPlayers(&calmWaterPlayer, &midWaterPlayer, &heavyWaterPlayer);

	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = juce::uint32(samplesPerBlock);
	spec.numChannels = 2;
	lowPassFilter.reset();
	lowPassFilter.prepare(spec);
	lowPassFilter.setCutoffFrequency(lowpassCutoffFreq);
	limiter.setThreshold(0.0f);
	limiter.setRelease(50.0f);
	limiter.prepare(spec);
	
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
	buffer.clear();

	params.update();
	for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
		params.smoothen();
	

	// Weather control override
	if (weatherControlEnabled.load())
	{
		

		WeatherAudioMapper::AudioParameters currentWeatherParams;
		{
			juce::SpinLock::ScopedLockType lock(weatherParamsLock);
			currentWeatherParams = weatherParams;
		}

		// Use weather data
		windCrossfade.setMasterGain(currentWeatherParams.windGain);
		windCrossfade.setCrossfadeValue(currentWeatherParams.windIntensity);

		rainCrossfade.setMasterGain(currentWeatherParams.rainGain);
		rainCrossfade.setCrossfadeValue(currentWeatherParams.rainIntensity);
	}
	else
	{

		// Use manual controls
		windCrossfade.setMasterGain(params.windGain);
		windCrossfade.setCrossfadeValue(params.windCrossfade);

		rainCrossfade.setMasterGain(params.rainGain);
		rainCrossfade.setCrossfadeValue(params.rainCrossfade);
	}

	windCrossfade.applyGains();
	rainCrossfade.applyGains();

	
	fireCrossfade.setMasterGain(params.fireGain);
	fireCrossfade.setCrossfadeValue(params.fireCrossfade);
	fireCrossfade.applyGains();

	cityCrossfade.setMasterGain(params.cityGain);
	cityCrossfade.setCrossfadeValue(params.cityCrossfade);
	cityCrossfade.applyGains();

	waterCrossfade.setMasterGain(params.waterGain);
	waterCrossfade.setCrossfadeValue(params.waterCrossfade);
	waterCrossfade.applyGains();

	thunderPlayer.setRandomTriggerEnabled(params.randomThunderToggle);
	thunderPlayer.setRandomTriggerInterval(params.minIntThunder, params.maxIntThunder);
	thunderPlayer.setGain(params.randomThunderGain);

	earthquakePlayer.setRandomTriggerEnabled(params.randomEarthquakeToggle);
	earthquakePlayer.setRandomTriggerInterval(params.minIntEarthquake, params.maxIntEarthquake);
	earthquakePlayer.setGain(params.randomEarthquakeGain);

	beastPlayer.setRandomTriggerEnabled(params.randomBeastToggle);
	beastPlayer.setRandomTriggerInterval(params.minIntBeast, params.maxIntBeast);
	beastPlayer.setGain(params.randomBeastGain);

	ProcessSamplePlayers(buffer);

	float compensation = calculateSmartGainCompensation();
	if (compensation < 1.0f)
	{
		buffer.applyGain(compensation);
	}
	juce::dsp::AudioBlock<float> block(buffer);
	
	juce::dsp::ProcessContextReplacing<float> context(block);
	if (params.lowpassToggle)
	{
		
		lowPassFilter.setCutoffFrequency(lowpassCutoffFreq); 
		lowPassFilter.process(context);
	}
	limiter.process(context);
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
	// Create a ValueTree to store the plugin state
	juce::ValueTree state("PluginState");

	// Store all parameters from the APVTS
	auto parametersState = apvts.copyState();
	state.addChild(parametersState, -1, nullptr);

	// Convert the ValueTree to XML
	auto xml = state.createXml();

	// Copy the XML into the binary data block
	if (xml != nullptr)
		copyXmlToBinary(*xml, destData);
}

void WeatherAmbienceGeneratorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// Convert the binary data back to XML
	auto xmlState = getXmlFromBinary(data, sizeInBytes);

	if (xmlState != nullptr)
	{
		// Convert XML back to ValueTree
		auto state = juce::ValueTree::fromXml(*xmlState);

		if (state.isValid())
		{
			// Look for the parameters child
			auto parametersState = state.getChildWithName("Parameters");

			if (parametersState.isValid())
			{
				// Restore the APVTS state
				apvts.replaceState(parametersState);
			}
		}
	}
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
	juce::File rainCalmFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Rain/Calm_Rain.wav");
	fileLoader.loadFileForPlayer("Calm Rain", rainCalmFile);

	midRainPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File rainMidFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Rain/Mid_Rain.wav");
	fileLoader.loadFileForPlayer("Mid Rain", rainMidFile);

	heavyRainPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File rainHeavyFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Rain/Heavy_Rain.wav");
	fileLoader.loadFileForPlayer("Heavy Rain", rainHeavyFile);

	//Fire Loading
	calmFirePlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File fireCalmFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Fire/Calm_Fire.wav");
	fileLoader.loadFileForPlayer("Calm Fire", fireCalmFile);

	midFirePlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File fireMidFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Fire/Mid_Fire.wav"); 
	fileLoader.loadFileForPlayer("Mid Fire", fireMidFile);

	heavyFirePlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File fireHeavyFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Fire/Heavy_Fire.wav"); 
	fileLoader.loadFileForPlayer("Heavy Fire", fireHeavyFile);

	//City Loading
	calmCityPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File cityCalmFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Ambience/Calm_Amb.wav");
	fileLoader.loadFileForPlayer("Calm City", cityCalmFile);

	midCityPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File cityMidFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Ambience/Mid_Amb.wav");
	fileLoader.loadFileForPlayer("Mid City", cityMidFile);

	heavyCityPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File cityHeavyFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Ambience/Heavy_Amb.wav"); 
	fileLoader.loadFileForPlayer("Heavy City", cityHeavyFile);

	//Water Loading
	calmWaterPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File waterCalmFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Water/Calm_Water.wav"); 
	fileLoader.loadFileForPlayer("Calm Water", waterCalmFile);

	midWaterPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File waterMidFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Water/Mid_Water.wav"); 
	fileLoader.loadFileForPlayer("Mid Water", waterMidFile);

	heavyWaterPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File waterHeavyFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/Water/Heavy_Water.wav");
	fileLoader.loadFileForPlayer("Heavy Water", waterHeavyFile);

	//Thunder Loading
	thunderPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File thunderFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/OneShots/Thunder_OneShot.wav");
	fileLoader.loadFileForPlayer("Thunder", thunderFile);
	thunderPlayer.setLoopMode(false);
	thunderPlayer.setRandomTriggerEnabled(params.randomThunderToggle);

	//Earthquake Loading
	earthquakePlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File earthquakeFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/OneShots/StoneRubble_OneShot.wav");
	fileLoader.loadFileForPlayer("Earthquake", earthquakeFile);
	earthquakePlayer.setLoopMode(false);
	earthquakePlayer.setRandomTriggerEnabled(params.randomEarthquakeToggle);

	// Beast Loading
	beastPlayer.prepareSamplePlayer(sampleRate, samplesPerBlock);
	juce::File beastFile("C:/Users/bramd/Desktop/WeatherAmbGenerator/SFX/OneShots/Beast_Roar_OneShot.wav");
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

void WeatherAmbienceGeneratorAudioProcessor::updateWeatherParameters(const WeatherFetcher::WeatherData& data)
{
	DBG("=== updateWeatherParameters called ===");

	if (!data.isValid)
	{
		DBG("ERROR: Data is invalid!");
		return;
	}

	
	auto audioParams = weatherMapper.mapWeatherToAudio(data);
	// Convert linear gain (0-1) to decibels for the gain parameters
	float windGainDB = juce::Decibels::gainToDecibels(audioParams.windGain, -80.0f);
	float rainGainDB = juce::Decibels::gainToDecibels(audioParams.rainGain, -80.0f);


	// Update APVTS parameters directly (this is called from message thread, so it's safe)
	auto* windCrossfadeParam = apvts.getParameter(windCrossfadeParamID.getParamID());
	auto* windGainParam = apvts.getParameter(windGainParamID.getParamID());
	auto* rainCrossfadeParam = apvts.getParameter(rainCrossfadeParamID.getParamID());
	auto* rainGainParam = apvts.getParameter(rainGainParamID.getParamID());

	if (windCrossfadeParam && windGainParam && rainCrossfadeParam && rainGainParam)
	{
		// Crossfade: 0-2 range, needs to be normalized to 0-1 for APVTS
		float normalizedWindCrossfade = audioParams.windIntensity / 2.0f;
		float normalizedRainCrossfade = audioParams.rainIntensity / 2.0f;

		// Gain: convert dB to normalized range (-80 to 24 dB)
		// Using the NormalisableRange from your parameter layout
		juce::NormalisableRange<float> gainRange{ -80.0f, 24.0f, 0.1f, 2.65f };
		float normalizedWindGain = gainRange.convertTo0to1(windGainDB);
		float normalizedRainGain = gainRange.convertTo0to1(rainGainDB);


		// Set the values
		windCrossfadeParam->setValueNotifyingHost(normalizedWindCrossfade);
		windGainParam->setValueNotifyingHost(normalizedWindGain);
		rainCrossfadeParam->setValueNotifyingHost(normalizedRainCrossfade);
		rainGainParam->setValueNotifyingHost(normalizedRainGain);

		
	}
	else
	{
		DBG("ERROR: Could not get parameters!");
	}
}

float WeatherAmbienceGeneratorAudioProcessor::calculateSmartGainCompensation()
{
	
	int activeCount = 0;
	const float activeThreshold = juce::Decibels::decibelsToGain(- 40.0f); // dB as linear gain

	if (params.windGain > activeThreshold) activeCount++;
	if (params.rainGain > activeThreshold) activeCount++;
	if (params.fireGain > activeThreshold) activeCount++;
	if (params.cityGain > activeThreshold) activeCount++;
	if (params.waterGain > activeThreshold) activeCount++;

	
	if (activeCount >= 3)
	{
		// Gentle reduction: -1.5 dB for 3 elements, -3 dB for 4, -4.5 dB for 5
		float compensationDb = -1.5f * (activeCount - 2);
		return juce::Decibels::decibelsToGain(compensationDb);
	}

	return 1.0f; // No compensation needed
}

