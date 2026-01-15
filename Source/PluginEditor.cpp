/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WeatherAmbienceGeneratorAudioProcessorEditor::WeatherAmbienceGeneratorAudioProcessorEditor(WeatherAmbienceGeneratorAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p), windLoopComponent("Wind"), rainLoopComponent("Rain"), fireLoopComponent("Fire"),cityLoopComponent("City"),waterLoopComponent("Water"), 
	thunderComponent("Thunder"),earthquakeComponent("Earthquake"), beastComponent("Beast Roar")
{
	rotaryRingImg = juce::ImageFileFormat::loadFrom(BinaryData::RotaryRing_png, BinaryData::RotaryRing_pngSize);
	rotaryKnobImg = juce::ImageFileFormat::loadFrom(BinaryData::RotaryKnob_png, BinaryData::RotaryKnob_pngSize);
	textBoxImg = juce::ImageFileFormat::loadFrom(BinaryData::textBoxImg_png, BinaryData::textBoxImg_pngSize);
	backgroundImg = juce::ImageFileFormat::loadFrom(BinaryData::Background_png, BinaryData::Background_pngSize);
	mainLNF.setRotaryKnobImages(rotaryRingImg, rotaryKnobImg, textBoxImg);
	setLookAndFeel(&mainLNF);
	setSize(1200, 800);

	//// Title setup
	//titleLabel.setText("Weather Ambience Generator", juce::dontSendNotification);
	//titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
	//titleLabel.setJustificationType(juce::Justification::centred);
	//titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
	//addAndMakeVisible(titleLabel);
	
	//Weather Fetcher
	addAndMakeVisible(weatherFetcherComponent);
	weatherFetcherComponent.setWeatherManager(audioProcessor.getWeatherManager());

	addAndMakeVisible(windLoopComponent);
	addAndMakeVisible(rainLoopComponent);
	addAndMakeVisible(fireLoopComponent);
	addAndMakeVisible(cityLoopComponent);
	addAndMakeVisible(waterLoopComponent);

	// Set attachments - what are your parameter IDs for wind intensity and gain?
	windLoopComponent.setIntensityAttachment(audioProcessor.apvts, windCrossfadeParamID.getParamID());
	windLoopComponent.setGainAttachment(audioProcessor.apvts, windGainParamID.getParamID());
	windLoopComponent.setPlayers(&audioProcessor.calmWindPlayer, &audioProcessor.midWindPlayer,
		&audioProcessor.strongWindPlayer);

	rainLoopComponent.setIntensityAttachment(audioProcessor.apvts, rainCrossfadeParamID.getParamID());
	rainLoopComponent.setGainAttachment(audioProcessor.apvts, rainGainParamID.getParamID());
	rainLoopComponent.setPlayers(&audioProcessor.calmRainPlayer, &audioProcessor.midRainPlayer, &audioProcessor.heavyRainPlayer);

	fireLoopComponent.setIntensityAttachment(audioProcessor.apvts, fireCrossfadeParamID.getParamID());
	fireLoopComponent.setGainAttachment(audioProcessor.apvts, fireGainParamID.getParamID());
	fireLoopComponent.setPlayers(&audioProcessor.calmFirePlayer, &audioProcessor.midFirePlayer, &audioProcessor.heavyFirePlayer);

	cityLoopComponent.setIntensityAttachment(audioProcessor.apvts, cityCrossfadeParamID.getParamID());
	cityLoopComponent.setGainAttachment(audioProcessor.apvts, cityGainParamID.getParamID());
	cityLoopComponent.setPlayers(&audioProcessor.calmCityPlayer, &audioProcessor.midCityPlayer, &audioProcessor.heavyCityPlayer);

	waterLoopComponent.setIntensityAttachment(audioProcessor.apvts, waterCrossfadeParamID.getParamID());
	waterLoopComponent.setGainAttachment(audioProcessor.apvts, waterGainParamID.getParamID());
	waterLoopComponent.setPlayers(&audioProcessor.calmWaterPlayer, &audioProcessor.midWaterPlayer, &audioProcessor.heavyWaterPlayer);

	

	addAndMakeVisible(audioProcessor.fileLoader);


	addAndMakeVisible(thunderComponent);
	thunderComponent.setPlayer(&audioProcessor.thunderPlayer);
	thunderComponent.setRandomToggleAttachment(audioProcessor.apvts, randomThunderToggleParamID.getParamID());
	thunderComponent.setMinIntervalAttachment(audioProcessor.apvts, minIntervalThunderParamID.getParamID());
	thunderComponent.setMaxIntervalAttachment(audioProcessor.apvts, maxIntervalThunderParamID.getParamID());
	thunderComponent.setGainAttachment(audioProcessor.apvts, randomThunderGainParamID.getParamID());

	addAndMakeVisible(earthquakeComponent);
	earthquakeComponent.setPlayer(&audioProcessor.earthquakePlayer);
	earthquakeComponent.setRandomToggleAttachment(audioProcessor.apvts, randomEarthquakeToggleParamID.getParamID());
	earthquakeComponent.setMinIntervalAttachment(audioProcessor.apvts, minIntervalEarthquakeParamID.getParamID());
	earthquakeComponent.setMaxIntervalAttachment(audioProcessor.apvts, maxIntervalEarthquakeParamID.getParamID());
	earthquakeComponent.setGainAttachment(audioProcessor.apvts, randomEarthquakeGainParamID.getParamID());

	addAndMakeVisible(beastComponent);
	beastComponent.setPlayer(&audioProcessor.beastPlayer);
	beastComponent.setRandomToggleAttachment(audioProcessor.apvts, randomBeastToggleParamID.getParamID());
	beastComponent.setMinIntervalAttachment(audioProcessor.apvts, minIntervalBeastParamID.getParamID());
	beastComponent.setMaxIntervalAttachment(audioProcessor.apvts, maxIntervalBeastParamID.getParamID());
	beastComponent.setGainAttachment(audioProcessor.apvts, randomBeastGainParamID.getParamID());
	
}

WeatherAmbienceGeneratorAudioProcessorEditor::~WeatherAmbienceGeneratorAudioProcessorEditor()
{
	setLookAndFeel(nullptr);
}

//==============================================================================
void WeatherAmbienceGeneratorAudioProcessorEditor::paint(juce::Graphics& g)
{
	// Draw background image stretched to fill entire component
	if (backgroundImg.isValid())
	{
		g.drawImage(backgroundImg, getLocalBounds().toFloat());
	}
	else
	{
		// Fallback if image doesn't load
		g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	}

	g.setColour(juce::Colours::white);
	g.setFont(juce::FontOptions(15.0f));
}

void WeatherAmbienceGeneratorAudioProcessorEditor::resized()
{
	auto area = getLocalBounds().reduced(20);
	const int totalHeight = area.getHeight();
	const int totalWidth = area.getWidth();

	// ================= TITLE =================
	const int titleHeight = totalHeight * 0.10f;
	auto titleArea = area.removeFromTop(titleHeight);
	titleLabel.setBounds(titleArea);

	// ================= TOP LOOP COMPONENTS =================
	const int loopHeight = totalHeight * 0.40f;
	const int numLoops = 5;
	const int padding = 10;

	auto topRow = area.removeFromTop(loopHeight);

	const int totalPadding = padding * (numLoops - 1);
	const int loopWidth = (topRow.getWidth() - totalPadding) / numLoops;

	// Wind
	windLoopComponent.setBounds(topRow.removeFromLeft(loopWidth));

	topRow.removeFromLeft(padding);
	rainLoopComponent.setBounds(topRow.removeFromLeft(loopWidth));

	topRow.removeFromLeft(padding);
	fireLoopComponent.setBounds(topRow.removeFromLeft(loopWidth));

	topRow.removeFromLeft(padding);
	cityLoopComponent.setBounds(topRow.removeFromLeft(loopWidth));

	topRow.removeFromLeft(padding);
	waterLoopComponent.setBounds(topRow.removeFromLeft(loopWidth));

	// ================= TRIGGER COMPONENTS (THUNDER, EARTHQUAKE & BEAST, MIDDLE) =================
	const int triggerWidth = totalWidth * 0.33f;
	const int triggerHeight = 180;
	const int triggerPadding = 15;  // Padding between components

	const int fileLoaderHeight = totalHeight * 0.20f;
	const int spaceBetween = area.getHeight() - fileLoaderHeight;
	const int triggerTopPadding = (spaceBetween - triggerHeight) / 2;

	auto triggerArea = area;
	triggerArea.removeFromTop(triggerTopPadding);

	// Thunder (left)
	thunderComponent.setBounds(
		triggerArea.removeFromLeft(triggerWidth).removeFromTop(triggerHeight)
	);

	// Padding between thunder and earthquake
	triggerArea.removeFromLeft(triggerPadding);

	// Earthquake (middle)
	earthquakeComponent.setBounds(
		triggerArea.removeFromLeft(triggerWidth).removeFromTop(triggerHeight)
	);

	// Padding between earthquake and beast
	triggerArea.removeFromLeft(triggerPadding);

	// Beast (right)
	beastComponent.setBounds(
		triggerArea.removeFromLeft(triggerWidth).removeFromTop(triggerHeight)
	);

	// ================= FILE LOADER (BOTTOM LEFT) =================
	auto fileLoaderArea = getLocalBounds().reduced(20);
	fileLoaderArea.removeFromTop(totalHeight - fileLoaderHeight);
	fileLoaderArea = fileLoaderArea.removeFromLeft(totalWidth * 0.40f);
	audioProcessor.fileLoader.setBounds(fileLoaderArea);

	// ================= WEATHER FETCHER (BOTTOM RIGHT) =================
	auto weatherArea = getLocalBounds().reduced(20);
	weatherArea.removeFromTop(totalHeight - fileLoaderHeight);
	weatherArea.removeFromLeft(totalWidth * 0.60f);
	weatherFetcherComponent.setBounds(weatherArea);
}




