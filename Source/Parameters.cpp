/*
  ==============================================================================

	Parameters.cpp
	Created: 1 Nov 2025 11:45:02am
	Author:  bramd

  ==============================================================================
*/

#include "Parameters.h"
#include "Pluginprocessor.h"

template<typename T>
static void castParameter(juce::AudioProcessorValueTreeState& apvts,
	const juce::ParameterID& id, T& destination)
{
	destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
	jassert(destination); // parameter does not exist or wrong type

}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
	//Loops
	castParameter(apvts, windGainParamID, windGainParam);
	castParameter(apvts, windCrossfadeParamID, windCrossfadeParam);
	castParameter(apvts, rainGainParamID, rainGainParam);
	castParameter(apvts, rainCrossfadeParamID, rainCrossfadeParam);
	castParameter(apvts, fireGainParamID, fireGainParam);
	castParameter(apvts, fireCrossfadeParamID, fireCrossfadeParam);
	castParameter(apvts, cityGainParamID, cityGainParam);
	castParameter(apvts, cityCrossfadeParamID, cityCrossfadeParam);
	castParameter(apvts, waterGainParamID, waterGainParam);
	castParameter(apvts, waterCrossfadeParamID, waterCrossfadeParam);

	//One Shots
	castParameter(apvts, randomThunderToggleParamID, randomThunderToggleParam);
	castParameter(apvts, minIntervalThunderParamID, minIntervalThunderParam);
	castParameter(apvts, maxIntervalThunderParamID, maxIntervalThunderParam);
	castParameter(apvts, randomThunderGainParamID, randomThunderGainParam);

	castParameter(apvts, randomEarthquakeToggleParamID, randomEarthquakeToggleParam);
	castParameter(apvts, minIntervalEarthquakeParamID, minIntervalEarthquakeParam);
	castParameter(apvts, maxIntervalEarthquakeParamID, maxIntervalEarthquakeParam);
	castParameter(apvts, randomEarthquakeGainParamID, randomEarthquakeGainParam);

	castParameter(apvts, randomBeastToggleParamID, randomBeastToggleParam);
	castParameter(apvts, minIntervalBeastParamID, minIntervalBeastParam);
	castParameter(apvts, maxIntervalBeastParamID, maxIntervalBeastParam);
	castParameter(apvts, randomBeastGainParamID, randomBeastGainParam);

	castParameter(apvts, lowpassToggleParamID, lowpassToggleParam);


}
juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{

	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	layout.add(std::make_unique<juce::AudioParameterBool>(lowpassToggleParamID, "lowpassToggle", false));

	//Loops 

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		windGainParamID.getParamID(), "windGain",
		juce::NormalisableRange<float>{ -80.0f, 24.f, 0.1f, 2.65f},
		-80.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		windCrossfadeParamID.getParamID(), "windCrossfade",
		juce::NormalisableRange<float>{ 0.0f, 2.0f, 0.1f, 1.0f},
		0.0f));
	layout.add(std::make_unique<juce::AudioParameterFloat>(
		rainGainParamID.getParamID(), "rainGain",
		juce::NormalisableRange<float>{ -80.0f, 24.f, 0.1f, 2.65f},
		-80.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		rainCrossfadeParamID.getParamID(), "rainCrossfade",
		juce::NormalisableRange<float>{ 0.0f, 2.0f, 0.1f, 1.0f},
		0.0f));
	layout.add(std::make_unique<juce::AudioParameterFloat>(
		fireGainParamID.getParamID(), "fireGain",
		juce::NormalisableRange<float>{ -80.0f, 24.f, 0.1f, 2.65f},
		-80.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		fireCrossfadeParamID.getParamID(), "fireCrossfade",
		juce::NormalisableRange<float>{ 0.0f, 2.0f, 0.1f, 1.0f},
		0.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		cityGainParamID.getParamID(), "cityGain",
		juce::NormalisableRange<float>{ -80.0f, 24.f, 0.1f, 2.65f},
		-80.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		cityCrossfadeParamID.getParamID(), "cityCrossfade",
		juce::NormalisableRange<float>{ 0.0f, 2.0f, 0.1f, 1.0f},
		0.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		waterGainParamID.getParamID(), "waterGain",
		juce::NormalisableRange<float>{ -80.0f, 24.f, 0.1f, 2.65f},
		-80.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		waterCrossfadeParamID.getParamID(), "waterCrossfade",
		juce::NormalisableRange<float>{ 0.0f, 2.0f, 0.1f, 1.0f},
		0.0f));

	//One Shots

	//Thunder
	layout.add(std::make_unique<juce::AudioParameterBool>(
		randomThunderToggleParamID.getParamID(), "thunderRandomToggle", false));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		minIntervalThunderParamID.getParamID(), "minIntervalThunder",
		juce::NormalisableRange<float>{ 0.0f, 600.0f, 1.0f, 1.0f},
		5.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		maxIntervalThunderParamID.getParamID(), "maxIntervalThunder",
		juce::NormalisableRange<float>{ 0.0f, 600.0f, 1.0f, 1.0f},
		15.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		randomThunderGainParamID.getParamID(), "randomThunderGain",
		juce::NormalisableRange<float>{ -24.f, 24.f, 0.1f, 1.0f},
		0.0f));

	//Earthquake

	layout.add(std::make_unique<juce::AudioParameterBool>(
		randomEarthquakeToggleParamID.getParamID(), "earthquakeRandomToggle", false));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		minIntervalEarthquakeParamID.getParamID(), "minIntervalEarthquake",
		juce::NormalisableRange<float>{ 0.0f, 600.0f, 1.0f, 1.0f},
		5.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		maxIntervalEarthquakeParamID.getParamID(), "maxIntervalEarthquake",
		juce::NormalisableRange<float>{ 0.0f, 600.0f, 1.0f, 1.0f},
		15.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		randomEarthquakeGainParamID.getParamID(), "randomEarthquakeGain",
		juce::NormalisableRange<float>{ -24.f, 24.f, 0.1f, 1.0f},
		0.0f));

	// Beast 
	layout.add(std::make_unique<juce::AudioParameterBool>(
		randomBeastToggleParamID.getParamID(), "beastRandomToggle", false));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		minIntervalBeastParamID.getParamID(), "minIntervalBeast",
		juce::NormalisableRange<float>{ 0.0f, 600.0f, 1.0f, 1.0f},
		5.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		maxIntervalBeastParamID.getParamID(), "maxIntervalBeast",
		juce::NormalisableRange<float>{ 0.0f, 600.0f, 1.0f, 1.0f},
		15.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		randomBeastGainParamID.getParamID(), "randomBeastGain",
		juce::NormalisableRange<float>{ -24.f, 24.f, 0.1f, 1.0f},
		0.0f));


	return layout;

}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
	double duration = 0.02;
	//Loops
	windGainSmoother.reset(sampleRate, duration);
	windCrossfadeSmoother.reset(sampleRate, duration);
	rainGainSmoother.reset(sampleRate, duration);
	rainCrossfadeSmoother.reset(sampleRate, duration);
	fireGainSmoother.reset(sampleRate, duration);
	fireCrossfadeSmoother.reset(sampleRate, duration);
	cityGainSmoother.reset(sampleRate, duration);
	cityCrossfadeSmoother.reset(sampleRate, duration);
	waterGainSmoother.reset(sampleRate, duration);
	waterCrossfadeSmoother.reset(sampleRate, duration);

	//One Shots
	randomThunderGainSmoother.reset(sampleRate, duration);
	randomEarthquakeGainSmoother.reset(sampleRate, duration);
	randomBeastGainSmoother.reset(sampleRate, duration);


}

void Parameters::reset() noexcept
{
	windGain = 0.0f;
	windGainSmoother.setCurrentAndTargetValue(
		juce::Decibels::decibelsToGain(windGainParam->get()));

	windCrossfade = 0.0f;
	windCrossfadeSmoother.setCurrentAndTargetValue(windCrossfadeParam->get());

	rainGain = 0.0f;
	rainGainSmoother.setCurrentAndTargetValue(
		juce::Decibels::decibelsToGain(rainGainParam->get()));

	rainCrossfade = 0.0f;
	rainCrossfadeSmoother.setCurrentAndTargetValue(rainCrossfadeParam->get());

	fireGain = 0.0f;
	fireGainSmoother.setCurrentAndTargetValue(
		juce::Decibels::decibelsToGain(fireGainParam->get()));

	fireCrossfade = 0.0f;
	fireCrossfadeSmoother.setCurrentAndTargetValue(fireCrossfadeParam->get());

	cityGain = 0.0f;
	cityGainSmoother.setCurrentAndTargetValue(
		juce::Decibels::decibelsToGain(cityGainParam->get()));

	cityCrossfade = 0.0f;
	cityCrossfadeSmoother.setCurrentAndTargetValue(cityCrossfadeParam->get());

	waterGain = 0.0f;
	waterGainSmoother.setCurrentAndTargetValue(
		juce::Decibels::decibelsToGain(waterGainParam->get()));

	waterCrossfade = 0.0f;
	waterCrossfadeSmoother.setCurrentAndTargetValue(waterCrossfadeParam->get());

	minIntThunder = 5.0f;
	maxIntThunder = 15.0f;

	randomThunderGain = 0.0f;
	randomThunderGainSmoother.setCurrentAndTargetValue(
		juce::Decibels::decibelsToGain(randomThunderGainParam->get()));

	minIntEarthquake = 5.0f;
	maxIntEarthquake = 15.0f;

	randomEarthquakeGain = 0.0f;
	randomEarthquakeGainSmoother.setCurrentAndTargetValue(
		juce::Decibels::decibelsToGain(randomEarthquakeGainParam->get()));

	minIntBeast = 5.0f;
	maxIntBeast = 15.0f;

	randomBeastGain = 0.0f;
	randomBeastGainSmoother.setCurrentAndTargetValue(
		juce::Decibels::decibelsToGain(randomBeastGainParam->get()));

}
void Parameters::update() noexcept
{
	windGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(windGainParam->get()));
	windCrossfadeSmoother.setTargetValue(windCrossfadeParam->get());

	rainGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(rainGainParam->get()));
	rainCrossfadeSmoother.setTargetValue(rainCrossfadeParam->get());

	fireGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(fireGainParam->get()));
	fireCrossfadeSmoother.setTargetValue(fireCrossfadeParam->get());

	cityGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(cityGainParam->get()));
	cityCrossfadeSmoother.setTargetValue(cityCrossfadeParam->get());

	waterGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(waterGainParam->get()));
	waterCrossfadeSmoother.setTargetValue(waterCrossfadeParam->get());

	randomThunderToggle = randomThunderToggleParam->get();

	minIntThunder = minIntervalThunderParam->get();
	maxIntThunder = maxIntervalThunderParam->get();
	randomThunderGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(randomThunderGainParam->get()));

	randomEarthquakeToggle = randomEarthquakeToggleParam->get();

	minIntEarthquake = minIntervalEarthquakeParam->get();
	maxIntEarthquake = maxIntervalEarthquakeParam->get();
	randomEarthquakeGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(randomEarthquakeGainParam->get()));

	randomBeastToggle = randomBeastToggleParam->get();

	minIntBeast = minIntervalBeastParam->get();
	maxIntBeast = maxIntervalBeastParam->get();
	randomBeastGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(randomBeastGainParam->get()));

	lowpassToggle = lowpassToggleParam->get();
}


void Parameters::smoothen() noexcept
{
	windGain = windGainSmoother.getNextValue();
	windCrossfade = windCrossfadeSmoother.getNextValue();
	rainGain = rainGainSmoother.getNextValue();
	rainCrossfade = rainCrossfadeSmoother.getNextValue();
	fireGain = fireGainSmoother.getNextValue();
	fireCrossfade = fireCrossfadeSmoother.getNextValue();
	cityGain = cityGainSmoother.getNextValue();
	cityCrossfade = cityCrossfadeSmoother.getNextValue();
	waterGain = waterGainSmoother.getNextValue();
	waterCrossfade = waterCrossfadeSmoother.getNextValue();



	randomThunderGain = randomThunderGainSmoother.getNextValue();
	randomEarthquakeGain = randomEarthquakeGainSmoother.getNextValue();
	randomBeastGain = randomBeastGainSmoother.getNextValue();

}
