/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WeatherAmbienceGeneratorAudioProcessorEditor::WeatherAmbienceGeneratorAudioProcessorEditor(WeatherAmbienceGeneratorAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p), windLoopComponent("Wind")
{
	rotaryRingImg = juce::ImageFileFormat::loadFrom(BinaryData::RotaryRing_png, BinaryData::RotaryRing_pngSize);
	rotaryKnobImg = juce::ImageFileFormat::loadFrom(BinaryData::RotaryKnob_png, BinaryData::RotaryKnob_pngSize);
	textBoxImg = juce::ImageFileFormat::loadFrom(BinaryData::textBoxImg_png, BinaryData::textBoxImg_pngSize);
	mainLNF.setRotaryKnobImages(rotaryRingImg, rotaryKnobImg, textBoxImg);
	setLookAndFeel(&mainLNF);
	setSize(1200, 900);
	// Setup city input
	cityInput.setText("London");
	cityInput.setFont(juce::Font(16.0f));
	addAndMakeVisible(cityInput);

	// Setup fetch button5
	fetchButton.setButtonText("Fetch Weather");
	fetchButton.onClick = [this]() { onFetchButtonClicked(); };
	addAndMakeVisible(fetchButton);

	// Setup status label
   // statusLabel.setText("Enter a city and click Fetch Weather", juce::dontSendNotification);
	statusLabel.setFont(juce::Font(14.0f));
	statusLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(statusLabel);

	addAndMakeVisible(windLoopComponent);

	// Set attachments - what are your parameter IDs for wind intensity and gain?
	windLoopComponent.setIntensityAttachment(audioProcessor.apvts, windCrossfadeParamID.getParamID());
	windLoopComponent.setGainAttachment(audioProcessor.apvts, windGainParamID.getParamID());
	windLoopComponent.setPlayers(&audioProcessor.calmWindPlayer, &audioProcessor.midWindPlayer,
		&audioProcessor.strongWindPlayer);

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
	setLookAndFeel(nullptr);
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

	// Top section - file loader
	auto topSection = area.removeFromTop(120);
	audioProcessor.fileLoader.setBounds(topSection);

	area.removeFromTop(10);

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

	// Interval sliders
	auto intervalRow = randomSection.removeFromTop(25);
	minIntervalSlider.setBounds(intervalRow.removeFromLeft(intervalRow.getWidth() / 2).reduced(5));
	maxIntervalSlider.setBounds(intervalRow.reduced(5));

	area.removeFromTop(10);

	// Status label
	statusLabel.setBounds(area.removeFromTop(100));

	area.removeFromTop(10);

	// Wind loop component at the BOTTOM
	windLoopComponent.setBounds(area.removeFromLeft(150));
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