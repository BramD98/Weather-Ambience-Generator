/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WeatherAmbienceGeneratorAudioProcessorEditor::WeatherAmbienceGeneratorAudioProcessorEditor(WeatherAmbienceGeneratorAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p), windLoopComponent("Wind"), rainLoopComponent("Rain"), thunderComponent("Thunder")
{
	rotaryRingImg = juce::ImageFileFormat::loadFrom(BinaryData::RotaryRing_png, BinaryData::RotaryRing_pngSize);
	rotaryKnobImg = juce::ImageFileFormat::loadFrom(BinaryData::RotaryKnob_png, BinaryData::RotaryKnob_pngSize);
	textBoxImg = juce::ImageFileFormat::loadFrom(BinaryData::textBoxImg_png, BinaryData::textBoxImg_pngSize);
	mainLNF.setRotaryKnobImages(rotaryRingImg, rotaryKnobImg, textBoxImg);
	setLookAndFeel(&mainLNF);
	setSize(1200, 800);

	// Title setup
	titleLabel.setText("Weather Ambience Generator", juce::dontSendNotification);
	titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
	titleLabel.setJustificationType(juce::Justification::centred);
	titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
	addAndMakeVisible(titleLabel);
	
	//Weather Fetcher
	addAndMakeVisible(weatherFetcherComponent);
	weatherFetcherComponent.setWeatherManager(audioProcessor.getWeatherManager());

	addAndMakeVisible(windLoopComponent);
	addAndMakeVisible(rainLoopComponent);

	// Set attachments - what are your parameter IDs for wind intensity and gain?
	windLoopComponent.setIntensityAttachment(audioProcessor.apvts, windCrossfadeParamID.getParamID());
	windLoopComponent.setGainAttachment(audioProcessor.apvts, windGainParamID.getParamID());
	windLoopComponent.setPlayers(&audioProcessor.calmWindPlayer, &audioProcessor.midWindPlayer,
		&audioProcessor.strongWindPlayer);

	rainLoopComponent.setIntensityAttachment(audioProcessor.apvts, rainCrossfadeParamID.getParamID());
	rainLoopComponent.setGainAttachment(audioProcessor.apvts, rainGainParamID.getParamID());
	rainLoopComponent.setPlayers(&audioProcessor.calmRainPlayer, &audioProcessor.midRainPlayer, &audioProcessor.heavyRainPlayer);

	/*fileLoader.registerPlayer("Calm Wind", &audioProcessor.calmWindPlayer);
	fileLoader.registerPlayer("Mid Wind", &audioProcessor.midWindPlayer);
	fileLoader.registerPlayer("Strong Wind", &audioProcessor.strongWindPlayer);
	fileLoader.registerPlayer("Calm Fire", &audioProcessor.calmFirePlayer);
	fileLoader.registerPlayer("Strong Fire", &audioProcessor.strongFirePlayer);*/

	// Register thunder player with file loader
	//audioProcessor.fileLoader.registerPlayer("Thunder", &audioProcessor.thunderPlayer);

	addAndMakeVisible(audioProcessor.fileLoader);


	addAndMakeVisible(thunderComponent);
	thunderComponent.setTriggerCallback([this]() { triggerThunder(); });
	thunderComponent.setRandomToggleAttachment(audioProcessor.apvts, randomThunderToggleParamID.getParamID());
	thunderComponent.setMinIntervalAttachment(audioProcessor.apvts, minIntervalThunderParamID.getParamID());
	thunderComponent.setMaxIntervalAttachment(audioProcessor.apvts, maxIntervalThunderParamID.getParamID());
	thunderComponent.setGainAttachment(audioProcessor.apvts, randomThunderGainParamID.getParamID());
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
	int totalHeight = area.getHeight();
	int totalWidth = area.getWidth();

	// Reserve top 10% for title space
	int titleHeight = totalHeight * 0.10f;
	auto titleArea = area.removeFromTop(titleHeight);

	// Title label in the top section
	titleLabel.setBounds(titleArea);

	// Wind loop component at TOP LEFT
	// Takes 1/5 of width (20%) and 40% of height (from 10% to 50% of screen)
	int windWidth = totalWidth * 0.20f;
	int windHeight = totalHeight * 0.40f;
	auto windArea = area.removeFromLeft(windWidth).removeFromTop(windHeight);
	windLoopComponent.setBounds(windArea);

	// Rain loop component NEXT TO wind component
	// Takes 1/5 of width (20%) and same height as wind (40%)
	int rainWidth = totalWidth * 0.20f;
	int rainHeight = totalHeight * 0.40f;
	auto rainArea = area.removeFromLeft(rainWidth).removeFromTop(rainHeight);
	rainLoopComponent.setBounds(rainArea);

	// Thunder section BELOW wind slider on the left
	// Takes 1/3 of width and centered between wind slider and file loader
	int thunderWidth = totalWidth * 0.33f;
	int thunderHeight = 180; // Fixed height with padding for future buttons

	// Calculate the available space between wind slider and file loader
	int fileLoaderHeight = totalHeight * 0.20f;
	int spaceAboveFileLoader = totalHeight - windHeight - fileLoaderHeight - titleHeight;
	int thunderTopPadding = (spaceAboveFileLoader - thunderHeight) / 2;

	auto thunderArea = getLocalBounds().reduced(20);
	thunderArea.removeFromTop(titleHeight + windHeight + thunderTopPadding); // Skip title, wind, and center vertically
	thunderArea = thunderArea.removeFromLeft(thunderWidth).removeFromTop(thunderHeight);

	// Manual trigger button
	thunderComponent.setBounds(thunderArea);

	// Remaining space for future buttons (thunderArea has remaining height)

	// File loader at BOTTOM LEFT
	// Takes 40% of width and 20% of height, positioned at bottom left (mirroring weather API)
	int fileLoaderWidth = totalWidth * 0.40f;

	auto fileLoaderArea = getLocalBounds().reduced(20);
	fileLoaderArea.removeFromTop(totalHeight - fileLoaderHeight); // Position at bottom
	fileLoaderArea = fileLoaderArea.removeFromLeft(fileLoaderWidth);
	audioProcessor.fileLoader.setBounds(fileLoaderArea);

	// Weather API section at BOTTOM RIGHT
	// Takes 40% of width and 20% of height, positioned at bottom right
	int weatherWidth = totalWidth * 0.40f;
	int weatherHeight = totalHeight * 0.20f;

	auto weatherArea = getLocalBounds().reduced(20);
	weatherArea.removeFromTop(totalHeight - weatherHeight); // Position at bottom
	weatherArea.removeFromLeft(totalWidth - weatherWidth); // Position at right




	weatherFetcherComponent.setBounds(weatherArea);
}

void WeatherAmbienceGeneratorAudioProcessorEditor::triggerThunder()
{
	audioProcessor.thunderPlayer.trigger();
}



