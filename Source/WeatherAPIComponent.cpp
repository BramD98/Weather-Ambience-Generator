/*
  ==============================================================================

    WeatherAPIComponent.cpp
    Created: 11 Jan 2026 7:51:00pm
    Author:  bramd

  ==============================================================================
*/

#include "WeatherAPIComponent.h"
WeatherAPIComponent::WeatherAPIComponent()
{
    // Setup city input
    cityInput.setText("London");
    cityInput.setFont(juce::Font(16.0f));
    cityInput.setJustification(juce::Justification::centred);
    addAndMakeVisible(cityInput);

    // Setup fetch button
    fetchButton.setButtonText("Fetch Weather");
    fetchButton.onClick = [this]() { onFetchButtonClicked(); };
    addAndMakeVisible(fetchButton);

    // Setup status label
    statusLabel.setMultiLine(true);
    statusLabel.setReadOnly(true);
    statusLabel.setScrollbarsShown(true);
    statusLabel.setCaretVisible(false);
    statusLabel.setPopupMenuEnabled(false);
    statusLabel.setJustification(juce::Justification::topLeft);
    statusLabel.setFont(juce::Font(14.0f));
    statusLabel.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    statusLabel.setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    statusLabel.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(statusLabel);
}

WeatherAPIComponent::~WeatherAPIComponent()
{
}

void WeatherAPIComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // === OUTER GLOW / SHADOW ===
    juce::Path outerPath;
    outerPath.addRoundedRectangle(bounds, 8.0f);

    juce::DropShadow shadow(juce::Colour(0xFF000000).withAlpha(0.6f), 8, juce::Point<int>(0, 2));
    shadow.drawForPath(g, outerPath);

    // === MAIN BACKGROUND - Frosted dark gradient ===
    juce::ColourGradient gradient(
        juce::Colour(0xFF1A1A2E), bounds.getX(), bounds.getY(),
        juce::Colour(0xFF16213E), bounds.getX(), bounds.getBottom(),
        false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 8.0f);

    // === INNER GLOW - Subtle cyan glow from top ===
    juce::ColourGradient innerGlow(
        juce::Colour(0xFF4FCFFF).withAlpha(0.15f), bounds.getCentreX(), bounds.getY(),
        juce::Colours::transparentBlack, bounds.getCentreX(), bounds.getY() + 60.0f,
        false);
    g.setGradientFill(innerGlow);
    g.fillRoundedRectangle(bounds.reduced(1.0f), 7.0f);

    // === GLASS REFLECTION - Top highlight ===
    auto reflectionArea = bounds.removeFromTop(bounds.getHeight() * 0.3f).reduced(8.0f, 4.0f);
    juce::ColourGradient reflection(
        juce::Colours::white.withAlpha(0.1f), reflectionArea.getCentreX(), reflectionArea.getY(),
        juce::Colours::transparentWhite, reflectionArea.getCentreX(), reflectionArea.getBottom(),
        false);
    g.setGradientFill(reflection);
    g.fillRoundedRectangle(reflectionArea, 4.0f);

    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    auto titleBounds = getLocalBounds().removeFromTop(35).reduced(5);
    g.drawText("Weather API", titleBounds, juce::Justification::centred);

    // === BLUE LINE UNDER TITLE ===
    g.setColour(juce::Colour(0xFF6FCFFF).withAlpha(0.6f));
    float lineY = titleBounds.getBottom() + 5.0f;
    g.drawLine(titleBounds.getX() + 10.0f, lineY,
        titleBounds.getRight() - 10.0f, lineY,
        2.0f);
}

void WeatherAPIComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Title spacing
    const int titleHeight = 35;
    bounds.removeFromTop(titleHeight + 6);

    // Split layout: left = controls, right = results
    const int controlWidth = juce::jlimit(140, 180, bounds.getWidth() / 3);
    auto controlArea = bounds.removeFromLeft(controlWidth);
    auto resultsArea = bounds;

    // =====================================
    // LEFT COLUMN: City input + Fetch button evenly spaced
    // =====================================
    const int inputHeight = 30;
    const int buttonHeight = 28;

    const int totalControlHeight = inputHeight + buttonHeight;
    const int numGaps = 3; // top, middle, bottom
    const int gap = (controlArea.getHeight() - totalControlHeight) / numGaps;

    int y = controlArea.getY() + gap;

    // Textbox
    cityInput.setBounds(
        controlArea.getX() + 4,
        y,
        controlArea.getWidth() - 8,
        inputHeight
    );

    // Middle gap
    y += inputHeight + gap;

    // Fetch button
    fetchButton.setBounds(
        controlArea.getX() + 4,
        y,
        controlArea.getWidth() - 8,
        buttonHeight
    );

    // =====================================
    // RIGHT COLUMN: Weather results centered
    // =====================================
    statusLabel.setBounds(resultsArea.reduced(6));

    // Center text horizontally
    statusLabel.setJustification(juce::Justification::centred);
}

void WeatherAPIComponent::setWeatherManager(WeatherManager* manager)
{
    weatherManager = manager;
}

void WeatherAPIComponent::onFetchButtonClicked()
{
    if (weatherManager == nullptr)
        return;

    juce::String city = cityInput.getText();

    if (city.isEmpty())
    {
        statusLabel.setText("Please enter a city name!", juce::dontSendNotification);
        return;
    }

    statusLabel.setText("Fetching weather for " + city + "...", juce::dontSendNotification);

    weatherManager->fetchWeather(city, [this](const WeatherManager::WeatherData& data)
        {
            onWeatherUpdate(data);
        });
}

void WeatherAPIComponent::onWeatherUpdate(const WeatherManager::WeatherData& data)
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