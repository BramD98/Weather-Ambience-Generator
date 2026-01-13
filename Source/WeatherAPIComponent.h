/*
  ==============================================================================

    WeatherAPIComponent.h
    Created: 11 Jan 2026 7:51:00pm
    Author:  bramd

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WeatherManager.h"

class WeatherAPIComponent : public juce::Component
{
public:
    WeatherAPIComponent();
    ~WeatherAPIComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setWeatherManager(WeatherManager* manager);

private:
    WeatherManager* weatherManager = nullptr;

    juce::TextEditor cityInput;
    juce::TextButton fetchButton;
    juce::TextEditor statusLabel;

    void onFetchButtonClicked();
    void onWeatherUpdate(const WeatherManager::WeatherData& data);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WeatherAPIComponent)
};