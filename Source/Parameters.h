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
const juce::ParameterID windCrossfadeParamID{ "windCrossfade",1 };
const juce::ParameterID rainGainParamID{ "rainGain", 1 };
const juce::ParameterID rainCrossfadeParamID{ "rainCrossfade",1 };

const juce::ParameterID randomThunderToggleParamID{ "randomThunderToggle" , 1 };
const juce::ParameterID minIntervalThunderParamID{ "minIntervalThunder" , 1 };
const juce::ParameterID maxIntervalThunderParamID{ "maxIntervalThunder" , 1 };
const juce::ParameterID randomThunderGainParamID{ "randomThungerGain",1 };



class Parameters {
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;
    void update()noexcept;

    float windGain = 0.0f;
    float windCrossfade = 0.0f;
    float rainGain = 0.0f;
    float rainCrossfade = 0.0f;
    bool randomThunderToggle = false;
    float minIntThunder = 0.0f;
    float maxIntThunder = 0.0f;
    float randomThunderGain = 0.0f;

private:
    //Loops
        //Wind
    juce::AudioParameterFloat* windGainParam;
    juce::LinearSmoothedValue<float> windGainSmoother;
    juce::AudioParameterFloat* windCrossfadeParam;
    juce::LinearSmoothedValue<float> windCrossfadeSmoother;
        //Rain
    juce::AudioParameterFloat* rainGainParam;
    juce::LinearSmoothedValue<float> rainGainSmoother;
    juce::AudioParameterFloat* rainCrossfadeParam;
    juce::LinearSmoothedValue<float> rainCrossfadeSmoother;


    //Triggers
    juce::AudioParameterBool* randomThunderToggleParam;
    juce::AudioParameterFloat* randomThunderGainParam;
    juce::LinearSmoothedValue<float> randomThunderGainSmoother;
    juce::AudioParameterFloat* minIntervalThunderParam;
    juce::LinearSmoothedValue<float> minIntervalThunderSmoother;
    juce::AudioParameterFloat* maxIntervalThunderParam;
    juce::LinearSmoothedValue<float> maxIntervalThunderSmoother;

   
};
