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
	castParameter(apvts, outputGainParamID, outputGainParam);
}
juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{

	juce::AudioProcessorValueTreeState::ParameterLayout layout;


	layout.add(std::make_unique<juce::AudioParameterFloat>(
		outputGainParamID.getParamID(), "OutputGain",
		juce::NormalisableRange<float>{ -24.f, 24.f, 0.1f, 1.0f},
		0.0f));

	return layout;

}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
	double duration = 0.02;
	outputGainSmoother.reset(sampleRate, duration);
}

void Parameters::reset() noexcept
{
	outputGain = 0.0f;
	outputGainSmoother.setCurrentAndTargetValue(
		juce::Decibels::decibelsToGain(outputGainParam->get()));
}
void Parameters::update() noexcept
{
	outputGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(outputGainParam->get()));
}
void Parameters::smoothen() noexcept
{
	outputGain = outputGainSmoother.getNextValue();
}
