/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/


#pragma once

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FileLoader.h"
#include "CrossfadeComponent.h"
#include "ArcticLookAndFeel.h"
#include "LoopComponent.h"
#include "TriggerComponent.h"
#include "WeatherAPIComponent.h"


#ifdef _MSC_VER
#pragma warning(pop)
#endif

//==============================================================================
/**
*/
class WeatherAmbienceGeneratorAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	WeatherAmbienceGeneratorAudioProcessorEditor(WeatherAmbienceGeneratorAudioProcessor&);
	~WeatherAmbienceGeneratorAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	WeatherAmbienceGeneratorAudioProcessor& audioProcessor;


	juce::Label titleLabel;

	
	ArcticLookAndFeel mainLNF;
	juce::Image rotaryKnobImg;
	juce::Image rotaryRingImg;
	juce::Image textBoxImg;
	juce::Image backgroundImg;

	LoopComponent windLoopComponent;
	LoopComponent rainLoopComponent;
	LoopComponent fireLoopComponent;
	LoopComponent cityLoopComponent;
	LoopComponent waterLoopComponent;
	TriggerComponent thunderComponent;
	TriggerComponent earthquakeComponent;
	TriggerComponent beastComponent;
	WeatherAPIComponent weatherFetcherComponent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WeatherAmbienceGeneratorAudioProcessorEditor)
};
