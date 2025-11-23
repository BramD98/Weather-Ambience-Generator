/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WeatherAmbienceGeneratorAudioProcessorEditor::WeatherAmbienceGeneratorAudioProcessorEditor (WeatherAmbienceGeneratorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    // Setup city input
    cityInput.setText("London");
    cityInput.setFont(juce::Font(16.0f));
    addAndMakeVisible(cityInput);

    // Setup fetch button
    fetchButton.setButtonText("Fetch Weather");
    fetchButton.onClick = [this]() { onFetchButtonClicked(); };
    addAndMakeVisible(fetchButton);

    // Setup status label
    statusLabel.setText("Enter a city and click Fetch Weather", juce::dontSendNotification);
    statusLabel.setFont(juce::Font(14.0f));
    statusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(statusLabel);

    // Setup wind gain slider
    windGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    windGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(windGainSlider);

    // Setup label
    windGainLabel.setText("Wind", juce::dontSendNotification);
    windGainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(windGainLabel);

    // Attach slider to APVTS parameter
    windGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "windGain", windGainSlider);
}

WeatherAmbienceGeneratorAudioProcessorEditor::~WeatherAmbienceGeneratorAudioProcessorEditor()
{
}

//==============================================================================
void WeatherAmbienceGeneratorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void WeatherAmbienceGeneratorAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    cityInput.setBounds(area.removeFromTop(40));
    area.removeFromTop(10);

    fetchButton.setBounds(area.removeFromTop(40));
    area.removeFromTop(20);

    statusLabel.setBounds(area.removeFromTop(100));

    windGainLabel.setBounds(20, 20, 80, 20);
    windGainSlider.setBounds(20, 45, 80, 200);
}
void WeatherAmbienceGeneratorAudioProcessorEditor::onFetchButtonClicked()
{
    juce::String city = cityInput.getText();

    if (city.isEmpty())
    {
        statusLabel.setText("Please enter a city name!", juce::dontSendNotification);
        return;
    }

    statusLabel.setText("Fetching weather for " + city + "...", juce::dontSendNotification);

    // Get weather manager from processor and fetch
    auto* weatherManager = audioProcessor.getWeatherManager();

    weatherManager->fetchWeather(city, [this](const WeatherManager::WeatherData& data)
        {
            onWeatherUpdate(data);
        });
}

void WeatherAmbienceGeneratorAudioProcessorEditor::onWeatherUpdate(const WeatherManager::WeatherData& data)
{
    if (data.isValid)
    {
        juce::String weatherInfo = "Weather: " + data.condition + "\n";
        weatherInfo += "Temp: " + juce::String(data.temperature, 1) + "°C\n";
        weatherInfo += "Wind: " + juce::String(data.windSpeed, 1) + " m/s\n";
        weatherInfo += "Humidity: " + juce::String(data.humidity) + "%\n";
        weatherInfo += "Rain: " + juce::String(data.precipitation, 1) + " mm";

        statusLabel.setText(weatherInfo, juce::dontSendNotification);
    }
    else
    {
        statusLabel.setText("Failed to fetch weather!", juce::dontSendNotification);
    }
}