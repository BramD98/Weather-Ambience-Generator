/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WeatherAmbienceGeneratorAudioProcessorEditor::WeatherAmbienceGeneratorAudioProcessorEditor(WeatherAmbienceGeneratorAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p), windCrossfade("Calm Wind", "Mid Wind", "Strong Wind"), fireCrossfade("Calm Fire", "Mid Fire", "Strong Fire")
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.

	setSize(1200, 900);
	// Setup city input
	cityInput.setText("London");
	cityInput.setFont(juce::Font(16.0f));
	addAndMakeVisible(cityInput);

	// Setup fetch button
	fetchButton.setButtonText("Fetch Weather");
	fetchButton.onClick = [this]() { onFetchButtonClicked(); };
	addAndMakeVisible(fetchButton);

	// Setup status label
   // statusLabel.setText("Enter a city and click Fetch Weather", juce::dontSendNotification);
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
		audioProcessor.apvts, windGainParamID.getParamID(), windGainSlider);


	/*fileLoader.registerPlayer("Calm Wind", &audioProcessor.calmWindPlayer);
	fileLoader.registerPlayer("Mid Wind", &audioProcessor.midWindPlayer);
	fileLoader.registerPlayer("Strong Wind", &audioProcessor.strongWindPlayer);
	fileLoader.registerPlayer("Calm Fire", &audioProcessor.calmFirePlayer);
	fileLoader.registerPlayer("Strong Fire", &audioProcessor.strongFirePlayer);*/

	// Register thunder player with file loader
	//audioProcessor.fileLoader.registerPlayer("Thunder", &audioProcessor.thunderPlayer);

	addAndMakeVisible(audioProcessor.fileLoader);

	// Setup Thunder trigger button
	thunderTriggerButton.setButtonText("⚡ THUNDER ⚡");
	thunderTriggerButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
	thunderTriggerButton.setColour(juce::TextButton::textColourOffId, juce::Colours::yellow);
	thunderTriggerButton.onClick = [this]() { triggerThunder(); };
	addAndMakeVisible(thunderTriggerButton);

	// Setup random trigger toggle
	randomThunderToggle.setButtonText("Random Thunder");
	randomThunderToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
		audioProcessor.apvts,
		randomThunderToggleParamID.getParamID(),  
		randomThunderToggle);
	addAndMakeVisible(randomThunderToggle);

	


	//Crossfaders
	windCrossfade.setPlayers(&audioProcessor.calmWindPlayer, &audioProcessor.midWindPlayer,
		&audioProcessor.strongWindPlayer);
	windCrossfade.attachMasterGain(audioProcessor.apvts, windGainParamID);
	addAndMakeVisible(windCrossfade);

	//Interval SLiders one shots

	minIntervalSlider.setSliderStyle(juce::Slider::LinearHorizontal);
	minIntervalSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
	addAndMakeVisible(minIntervalSlider);

	minIntervalAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, minIntervalThunderParamID.getParamID(), minIntervalSlider);

	maxIntervalSlider.setSliderStyle(juce::Slider::LinearHorizontal);
	maxIntervalSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
	addAndMakeVisible(maxIntervalSlider);

	maxIntervalAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.apvts, maxIntervalThunderParamID.getParamID(), maxIntervalSlider);


	//fireCrossfade.setPlayers(&audioProcessor.calmFirePlayer, &audioProcessor.strongFirePlayer);
	//addAndMakeVisible(fireCrossfade);
}

WeatherAmbienceGeneratorAudioProcessorEditor::~WeatherAmbienceGeneratorAudioProcessorEditor()
{
}

//==============================================================================
void WeatherAmbienceGeneratorAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	g.setColour(juce::Colours::white);
	g.setFont(juce::FontOptions(15.0f));
	//g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void WeatherAmbienceGeneratorAudioProcessorEditor::resized()
{
	auto area = getLocalBounds().reduced(20);

	// Top section - wind gain slider and label on the left
	auto topSection = area.removeFromTop(250);
	auto sliderSection = topSection.removeFromLeft(100);

	windGainLabel.setBounds(sliderSection.removeFromTop(20));
	sliderSection.removeFromTop(5);
	windGainSlider.setBounds(sliderSection.removeFromTop(200));

	// Right side of top section - file loader
	topSection.removeFromLeft(10);  // spacing
	audioProcessor.fileLoader.setBounds(topSection.removeFromTop(120));

	area.removeFromTop(10);

	// Wind crossfade
	windCrossfade.setBounds(area.removeFromTop(100));
	area.removeFromTop(10);

	// Fire crossfade
	fireCrossfade.setBounds(area.removeFromTop(100));
	area.removeFromTop(20);

	// Thunder section
	auto thunderSection = area.removeFromTop(120);
	thunderSection.removeFromLeft(20);
	thunderSection.removeFromRight(20);

	// Manual trigger button
	thunderTriggerButton.setBounds(thunderSection.removeFromTop(50));
	thunderSection.removeFromTop(10);

	// Random thunder controls
	auto randomSection = thunderSection.removeFromTop(60);
	randomThunderToggle.setBounds(randomSection.removeFromTop(25));
	randomSection.removeFromTop(5);
	//Interval sliders
	auto intervalRow = randomSection.removeFromTop(25);
	minIntervalSlider.setBounds(intervalRow.removeFromLeft(intervalRow.getWidth() / 2).reduced(5));
	maxIntervalSlider.setBounds(intervalRow.reduced(5));
	

	// Status label
	statusLabel.setBounds(area.removeFromTop(100));
}

void WeatherAmbienceGeneratorAudioProcessorEditor::triggerThunder()
{
	audioProcessor.thunderPlayer.trigger();
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