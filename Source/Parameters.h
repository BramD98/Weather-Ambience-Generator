/*
  ==============================================================================

    Parameters.h
    Created: 1 Nov 2025 11:45:02am
    Author:  bramd

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once
const juce::ParameterID outputGainParamID{ "outputGain", 1 };

class Parameters {
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;
    void update()noexcept;

    float outputGain = 0.0f;


private:
    juce::AudioParameterFloat* outputGainParam;
    juce::LinearSmoothedValue<float> outputGainSmoother;
};
