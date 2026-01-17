/*
  ==============================================================================

    WeatherAudioMapper.cpp
    Created: 17 Jan 2026 11:01:45am
    Author:  bramd

  ==============================================================================
*/

#include "WeatherAudioMapper.h"


WeatherAudioMapper::WeatherAudioMapper()
{
}

WeatherAudioMapper::~WeatherAudioMapper()
{
}

WeatherAudioMapper::AudioParameters WeatherAudioMapper::mapWeatherToAudio(const WeatherFetcher::WeatherData& weather)
{
    AudioParameters params;

    if (!weather.isValid)
        return params;

    // Map wind speed to parameters
    params.windIntensity = mapWindSpeedToIntensity(weather.windSpeed);
    params.windGain = mapWindSpeedToGain(weather.windSpeed);

    // Map precipitation to parameters
    params.rainIntensity = mapPrecipitationToIntensity(weather.precipitation);
    params.rainGain = mapPrecipitationToGain(weather.precipitation);

    return params;
}

float WeatherAudioMapper::mapWindSpeedToIntensity(double windSpeed)
{
    // Wind speed mapping (in m/s):
    // 0-3: calm -> 0.0-0.5 (mostly calm wind)
    // 3-8: moderate -> 0.5-1.0 (transitioning to mid wind)
    // 8-15: strong -> 1.0-1.5 (mid to strong wind)
    // 15+: very strong -> 1.5-2.0 (strong wind)

    if (windSpeed < 3.0)
        return juce::jmap(windSpeed, 0.0, 3.0, 0.0, 0.5);
    else if (windSpeed < 8.0)
        return juce::jmap(windSpeed, 3.0, 8.0, 0.5, 1.0);
    else if (windSpeed < 15.0)
        return juce::jmap(windSpeed, 8.0, 15.0, 1.0, 1.5);
    else
        return juce::jmap(juce::jmin(windSpeed, 30.0), 15.0, 30.0, 1.5, 2.0);
}

float WeatherAudioMapper::mapWindSpeedToGain(double windSpeed)
{
    // More aggressive gain curve
    // At 10 m/s (moderate wind), we want around 0 dB
    // At 20 m/s (strong wind), we want close to max

    float normalized = juce::jlimit(0.0f, 1.0f, static_cast<float>(windSpeed) / 15.0f);

    // Use linear instead of squared for louder results
    return normalized;
}

float WeatherAudioMapper::mapPrecipitationToIntensity(double precipitation)
{
    // Precipitation mapping (in mm/hour):
    // 0-0.5: drizzle -> 0.0-0.5
    // 0.5-2: light rain -> 0.5-1.0
    // 2-10: moderate rain -> 1.0-1.5
    // 10+: heavy rain -> 1.5-2.0

    if (precipitation < 0.5)
        return juce::jmap(precipitation, 0.0, 0.5, 0.0, 0.5);
    else if (precipitation < 2.0)
        return juce::jmap(precipitation, 0.5, 2.0, 0.5, 1.0);
    else if (precipitation < 10.0)
        return juce::jmap(precipitation, 2.0, 10.0, 1.0, 1.5);
    else
        return juce::jmap(juce::jmin(precipitation, 20.0), 10.0, 20.0, 1.5, 2.0);
}

float WeatherAudioMapper::mapPrecipitationToGain(double precipitation)
{
    // More aggressive - at 5mm/h rain should be quite audible
    float normalized = juce::jlimit(0.0f, 1.0f, static_cast<float>(precipitation) / 5.0f);

    return normalized;
}