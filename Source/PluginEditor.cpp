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
	openWindowImg = juce::ImageFileFormat::loadFrom(BinaryData::Window_Open_png, BinaryData::Window_Open_pngSize);
	closedWindowImg = juce::ImageFileFormat::loadFrom(BinaryData::Window_Closed_png, BinaryData::Window_Closed_pngSize);
	mainLNF.setRotaryKnobImages(rotaryRingImg, rotaryKnobImg, textBoxImg);
	insideToggle.setImages(openWindowImg, closedWindowImg);
	setLookAndFeel(&mainLNF);
	
	setSize(1200, 800);

	
	//Weather Fetcher
	weatherFetcherComponent.setLookAndFeel(&mainLNF);
	addAndMakeVisible(weatherFetcherComponent);
	weatherFetcherComponent.setWeatherManager(audioProcessor.getWeatherManager());
	weatherFetcherComponent.onWeatherFetched = [this](const WeatherManager::WeatherData& data)
		{
			audioProcessor.updateWeatherParameters(data);
		};

	addAndMakeVisible(windLoopComponent);
	addAndMakeVisible(rainLoopComponent);
	addAndMakeVisible(fireLoopComponent);
	addAndMakeVisible(cityLoopComponent);
	addAndMakeVisible(waterLoopComponent);

	
	windLoopComponent.setIntensityAttachment(audioProcessor.apvts, windCrossfadeParamID.getParamID());
	windLoopComponent.setGainAttachment(audioProcessor.apvts, windGainParamID.getParamID());
	

	rainLoopComponent.setIntensityAttachment(audioProcessor.apvts, rainCrossfadeParamID.getParamID());
	rainLoopComponent.setGainAttachment(audioProcessor.apvts, rainGainParamID.getParamID());
	

	fireLoopComponent.setIntensityAttachment(audioProcessor.apvts, fireCrossfadeParamID.getParamID());
	fireLoopComponent.setGainAttachment(audioProcessor.apvts, fireGainParamID.getParamID());
	

	cityLoopComponent.setIntensityAttachment(audioProcessor.apvts, cityCrossfadeParamID.getParamID());
	cityLoopComponent.setGainAttachment(audioProcessor.apvts, cityGainParamID.getParamID());
	

	waterLoopComponent.setIntensityAttachment(audioProcessor.apvts, waterCrossfadeParamID.getParamID());
	waterLoopComponent.setGainAttachment(audioProcessor.apvts, waterGainParamID.getParamID());
	

	
	audioProcessor.fileLoader.setLookAndFeel(&mainLNF);
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
	
	insideToggle.attachToParameter(audioProcessor.apvts, lowpassToggleParamID.getParamID());
	addAndMakeVisible(insideToggle);
	
}

WeatherAmbienceGeneratorAudioProcessorEditor::~WeatherAmbienceGeneratorAudioProcessorEditor()
{
	setLookAndFeel(nullptr);
	audioProcessor.fileLoader.setLookAndFeel(nullptr);
	weatherFetcherComponent.setLookAndFeel(nullptr);
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

	auto bottomArea = getLocalBounds().reduced(20);
	bottomArea.removeFromTop(totalHeight - fileLoaderHeight);

	// Padding between bottom components
	const int bottomPadding = 15;

	// Split into left / middle / right
	auto fileLoaderArea = bottomArea.removeFromLeft(totalWidth * 0.40f);
	bottomArea.removeFromLeft(bottomPadding);

	auto insideToggleArea = bottomArea.removeFromLeft(totalWidth * 0.20f);
	bottomArea.removeFromLeft(bottomPadding);

	auto weatherArea = bottomArea; // remaining space (right)

	// Apply padding inside each area
	audioProcessor.fileLoader.setBounds(fileLoaderArea.reduced(10));
	weatherFetcherComponent.setBounds(weatherArea.reduced(10));

	// Center the inside toggle inside its slot
	insideToggle.setBounds(
		insideToggleArea
		.withSizeKeepingCentre(
			insideToggleArea.getHeight(),   // square toggle
			insideToggleArea.getHeight()
		)
		.reduced(0)
	);
}




