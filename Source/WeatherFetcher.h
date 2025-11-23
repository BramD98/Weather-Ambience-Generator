/*
  ==============================================================================

	WeatherFetcher.h
	Created: 1 Nov 2025 4:52:30pm
	Author:  bramd

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

const juce::String m_apiKey = "184ed93dc4811020f74ce7193507e8fb";

class WeatherFetcher {
public:
	struct WeatherData {
		juce::String condition;
		double temperature;
		double windSpeed;
		int humidity;
		double precipitation;
		bool isValid{ false };
	};

	using WeatherCallback = std::function<void(const WeatherData&)>;

	WeatherFetcher(const juce::String& apiKey) : apiKey(apiKey)
	{

	}

	void fetchWeather(const juce::String& city, WeatherCallback callback)
	{
		juce::String url = "https://api.openweathermap.org/data/2.5/weather?q="
			+ juce::URL::addEscapeChars(city, true) // Makes city name URL Safe (no spaces in new york)
			+ "&appid=" + apiKey // Puts in the API key needed for the request
			+ "&units=metric"; //Asks for celsius instead of Fahrenheit

		std::thread([url, callback]()
		{
				WeatherData data;
			juce::URL weatherURL(url);

			std::unique_ptr<juce::InputStream> stream(weatherURL.createInputStream
			(juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress).withConnectionTimeoutMs(5000)));

			if (stream != nullptr)
			{
				juce::String response = stream->readEntireStreamAsString();

				data = parseWeatherData(response);
			}

			juce::MessageManager::callAsync([callback, data]()
				{
					callback(data);
				});

		}).detach();
	}
private:
	juce::String apiKey;
	static WeatherData parseWeatherData(const juce::String& jsonResponse)
	{
		WeatherData data;
		juce::var json = juce::JSON::parse(jsonResponse);

		if (json.isObject())
		{
			juce::var weather = json["weather"];
			if (weather.isArray() && weather.size() > 0)
			{
				data.condition = weather[0]["main"].toString();
			}

			juce::var main = json["main"];
			if (main.isObject())
			{
				data.temperature = main["temp"];
				data.humidity = main["humidity"];
			}

			juce::var wind = json["wind"];
			if (wind.isObject())
			{
				data.windSpeed = wind["speed"];
			}

			juce::var rain = json["rain"];
			if (rain.isObject())
			{
				data.precipitation = rain.getProperty("1h", 0.0);
			}

			data.isValid = true;
		}

		return data;
	}
};