/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/


#pragma once

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FileLoader.h"
#include "CrossfadeComponent.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

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

    juce::Slider windGainSlider;
    juce::Label windGainLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> windGainAttachment;
    //FileLoader fileLoader;
    CrossfadeComponent windCrossfade;
    CrossfadeComponent fireCrossfade;
    juce::TextButton thunderTriggerButton;
    juce::ToggleButton randomThunderToggle;
    juce::Label minIntervalLabel, maxIntervalLabel;
    juce::TextEditor minIntervalInput, maxIntervalInput;

    // Thunder methods
    void triggerThunder();
    void onRandomToggleChanged();
    void onIntervalChanged();

    void onFetchButtonClicked();
    void onWeatherUpdate(const WeatherManager::WeatherData& data);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WeatherAmbienceGeneratorAudioProcessorEditor)
};
