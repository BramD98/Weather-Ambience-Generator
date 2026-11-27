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
const juce::ParameterID fireGainParamID{ "fireGain", 1 };
const juce::ParameterID fireCrossfadeParamID{ "fireCrossfade",1 };
const juce::ParameterID cityGainParamID{ "cityGain", 1 };
const juce::ParameterID cityCrossfadeParamID{ "cityCrossfade",1 };
const juce::ParameterID waterGainParamID{ "waterGain", 1 };
const juce::ParameterID waterCrossfadeParamID{ "waterCrossfade",1 };

const juce::ParameterID randomThunderToggleParamID{ "randomThunderToggle" , 1 };
const juce::ParameterID minIntervalThunderParamID{ "minIntervalThunder" , 1 };
const juce::ParameterID maxIntervalThunderParamID{ "maxIntervalThunder" , 1 };
const juce::ParameterID randomThunderGainParamID{ "randomThunderGain",1 };

const juce::ParameterID randomEarthquakeToggleParamID{ "randomEarthquakeToggle" , 1 };
const juce::ParameterID minIntervalEarthquakeParamID{ "minIntervalEarthquake" , 1 };
const juce::ParameterID maxIntervalEarthquakeParamID{ "maxIntervalEarthquake" , 1 };
const juce::ParameterID randomEarthquakeGainParamID{ "randomEarthquakeGain",1 };

const juce::ParameterID randomBeastToggleParamID{ "randomBeastToggle" , 1 };
const juce::ParameterID minIntervalBeastParamID{ "minIntervalBeast" , 1 };
const juce::ParameterID maxIntervalBeastParamID{ "maxIntervalBeast" , 1 };
const juce::ParameterID randomBeastGainParamID{ "randomBeastGain",1 };

const juce::ParameterID lowpassToggleParamID{ "lowpasToggle",1 };



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
    float fireGain = 0.0f;
    float fireCrossfade = 0.0f;
    float cityGain = 0.0f;
    float cityCrossfade = 0.0f;
    float waterGain = 0.0f;
    float waterCrossfade = 0.0f;

    bool randomThunderToggle = false;
    float minIntThunder = 0.0f;
    float maxIntThunder = 0.0f;
    float randomThunderGain = 0.0f;

    bool randomEarthquakeToggle = false;
    float minIntEarthquake = 0.0f;
    float maxIntEarthquake = 0.0f;
    float randomEarthquakeGain = 0.0f;

    bool randomBeastToggle = false;
    float minIntBeast = 0.0f;
    float maxIntBeast = 0.0f;
    float randomBeastGain = 0.0f;

    bool lowpassToggle = false;

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
        //Fire
    juce::AudioParameterFloat* fireGainParam;
    juce::LinearSmoothedValue<float> fireGainSmoother;
    juce::AudioParameterFloat* fireCrossfadeParam;
    juce::LinearSmoothedValue<float> fireCrossfadeSmoother;
        //City
    juce::AudioParameterFloat* cityGainParam;
    juce::LinearSmoothedValue<float> cityGainSmoother;
    juce::AudioParameterFloat* cityCrossfadeParam;
    juce::LinearSmoothedValue<float> cityCrossfadeSmoother;
        // Water
    juce::AudioParameterFloat* waterGainParam;
    juce::LinearSmoothedValue<float> waterGainSmoother;
    juce::AudioParameterFloat* waterCrossfadeParam;
    juce::LinearSmoothedValue<float> waterCrossfadeSmoother;



    //Triggers
    juce::AudioParameterBool* randomThunderToggleParam;
    juce::AudioParameterFloat* randomThunderGainParam;
    juce::LinearSmoothedValue<float> randomThunderGainSmoother;
    juce::AudioParameterFloat* minIntervalThunderParam;
    juce::LinearSmoothedValue<float> minIntervalThunderSmoother;
    juce::AudioParameterFloat* maxIntervalThunderParam;
    juce::LinearSmoothedValue<float> maxIntervalThunderSmoother;

    juce::AudioParameterBool* randomEarthquakeToggleParam;
    juce::AudioParameterFloat* randomEarthquakeGainParam;
    juce::LinearSmoothedValue<float> randomEarthquakeGainSmoother;
    juce::AudioParameterFloat* minIntervalEarthquakeParam;
    juce::LinearSmoothedValue<float> minIntervalEarthquakeSmoother;
    juce::AudioParameterFloat* maxIntervalEarthquakeParam;
    juce::LinearSmoothedValue<float> maxIntervalEarthquakeSmoother;

    juce::AudioParameterBool* randomBeastToggleParam;
    juce::AudioParameterFloat* randomBeastGainParam;
    juce::LinearSmoothedValue<float> randomBeastGainSmoother;
    juce::AudioParameterFloat* minIntervalBeastParam;
    juce::LinearSmoothedValue<float> minIntervalBeasteSmoother;
    juce::AudioParameterFloat* maxIntervalBeastParam;
    juce::LinearSmoothedValue<float> maxIntervalBeastSmoother;

    juce::AudioParameterBool* lowpassToggleParam;
};
