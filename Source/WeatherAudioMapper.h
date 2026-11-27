/*
  ==============================================================================

    WeatherAudioMapper.h
    Created: 17 Jan 2026 11:01:45am
    Author:  bramd

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "WeatherFetcher.h"

class WeatherAudioMapper {
public:
    struct AudioParameters {
        float windIntensity = 0.0f;    
        float windGain = 0.0f;         
        float rainIntensity = 0.0f;    
        float rainGain = 0.0f;         
    };

    WeatherAudioMapper();
    ~WeatherAudioMapper();

    AudioParameters mapWeatherToAudio(const WeatherFetcher::WeatherData& weather);

private:
    float mapWindSpeedToIntensity(double windSpeed);
    float mapWindSpeedToGain(double windSpeed);
    float mapPrecipitationToIntensity(double precipitation);
    float mapPrecipitationToGain(double precipitation);
};