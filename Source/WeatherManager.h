/*
  ==============================================================================

    WeatherManager.h
    Created: 1 Nov 2025 6:57:43pm
    Author:  bramd

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "WeatherFetcher.h"

class WeatherManager {

public:

    using WeatherData = WeatherFetcher::WeatherData;
    using WeatherUpdateCallback = std::function<void(const WeatherData&)>;

    WeatherManager(const juce::String& apiKey)
    {
        weatherFetcher = std::make_unique<WeatherFetcher>(apiKey);
    }

    void fetchWeather(const juce::String& city, WeatherUpdateCallback callback)
    {
        currentCity = city;
        weatherFetcher->fetchWeather(city, [this, callback](const WeatherData& data)
            {
                currentWeather = data;
                if (callback)
                {
                    callback(data);
                }
            });
    }

    const WeatherData& getCurrentWeather() const
    {
        return currentWeather;
    }
    const juce::String& getCurrentCity() const
    {
        return currentCity;
    }
private:
    std::unique_ptr<WeatherFetcher> weatherFetcher;
    WeatherData currentWeather;
    juce::String currentCity;
};
