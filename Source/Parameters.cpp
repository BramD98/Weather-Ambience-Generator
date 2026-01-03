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
	castParameter(apvts, windGainParamID, windGainParam);
	castParameter(apvts, windCrossfadeParamID, windCrossfadeParam);
	castParameter(apvts, randomThunderToggleParamID, randomThunderToggleParam);
	castParameter(apvts, minIntervalThunderParamID, minIntervalThunderParam);
	castParameter(apvts, maxIntervalThunderParamID, maxIntervalThunderParam);

}
juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{

	juce::AudioProcessorValueTreeState::ParameterLayout layout;


	layout.add(std::make_unique<juce::AudioParameterFloat>(
		windGainParamID.getParamID(), "windGain",
		juce::NormalisableRange<float>{ -24.f, 24.f, 0.1f, 1.0f},
		0.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		windCrossfadeParamID.getParamID(), "windCrossfade",
		juce::NormalisableRange<float>{ 0.0f, 2.0f, 0.1f, 1.0f},
		0.0f));

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


	return layout;

}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
	double duration = 0.02;
	windGainSmoother.reset(sampleRate, duration);
	windCrossfadeSmoother.reset(sampleRate, duration);
	

}

void Parameters::reset() noexcept
{
	windGain = 0.0f;
	windGainSmoother.setCurrentAndTargetValue(
		juce::Decibels::decibelsToGain(windGainParam->get()));

	windCrossfade = 0.0f;
	windCrossfadeSmoother.setCurrentAndTargetValue(windCrossfadeParam->get());

	minIntThunder = 5.0f;
	
	maxIntThunder = 15.0f;
	
}
void Parameters::update() noexcept
{
	windGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(windGainParam->get()));
	windCrossfadeSmoother.setTargetValue(windCrossfadeParam->get());

	randomThunderToggle = randomThunderToggleParam->get();

	minIntThunder = minIntervalThunderParam->get();
	maxIntThunder = maxIntervalThunderParam->get();
	
}
void Parameters::smoothen() noexcept
{
	windGain = windGainSmoother.getNextValue();
	windCrossfade = windCrossfadeSmoother.getNextValue();

}
