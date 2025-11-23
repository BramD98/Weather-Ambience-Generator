/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class WeatherAmbienceGeneratorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    WeatherAmbienceGeneratorAudioProcessorEditor (WeatherAmbienceGeneratorAudioProcessor&);
    ~WeatherAmbienceGeneratorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WeatherAmbienceGeneratorAudioProcessor& audioProcessor;

    juce::TextEditor cityInput;
    juce::TextButton fetchButton;
    juce::Label statusLabel;

    void onFetchButtonClicked();
    void onWeatherUpdate(const WeatherManager::WeatherData& data);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WeatherAmbienceGeneratorAudioProcessorEditor)
};
