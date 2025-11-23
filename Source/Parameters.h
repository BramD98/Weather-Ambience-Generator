/*
  ==============================================================================

    Parameters.h
    Created: 1 Nov 2025 11:45:02am
    Author:  bramd

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once
const juce::ParameterID windGainParamID{ "windGain", 1 };

class Parameters {
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;
    void update()noexcept;

    float windGain = 0.0f;


private:
    juce::AudioParameterFloat* windGainParam;
    juce::LinearSmoothedValue<float> windGainSmoother;
};
