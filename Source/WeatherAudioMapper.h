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
        float windIntensity = 0.0f;    // 0.0 to 2.0 (crossfade value)
        float windGain = 0.0f;         // 0.0 to 1.0 (master gain)
        float rainIntensity = 0.0f;    // 0.0 to 2.0 (crossfade value)
        float rainGain = 0.0f;         // 0.0 to 1.0 (master gain)
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