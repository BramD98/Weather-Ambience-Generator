/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include <memory>
#include "Parameters.h"
#include "WeatherFetcher.h"
#include "WeatherManager.h"
#include "AudioSamplePlayer.h"
#include "FileLoader.h"



//==============================================================================
/**
*/
class WeatherAmbienceGeneratorAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    WeatherAmbienceGeneratorAudioProcessor();
    ~WeatherAmbienceGeneratorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    //juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    Parameters params;
    WeatherManager* getWeatherManager() { return weatherManager.get(); }

    // Loopers
    AudioSamplePlayer calmWindPlayer;
    AudioSamplePlayer midWindPlayer;
    AudioSamplePlayer strongWindPlayer;

    AudioSamplePlayer calmRainPlayer;
    AudioSamplePlayer midRainPlayer;
    AudioSamplePlayer heavyRainPlayer;

    AudioSamplePlayer calmFirePlayer;
    AudioSamplePlayer strongFirePlayer;

    //One Shots
    AudioSamplePlayer thunderPlayer;
    
    FileLoader fileLoader;

private:
    //==============================================================================
    std::unique_ptr <WeatherManager> weatherManager;
    void FileLoaderRegisterPlayers();
    void PrepareSamplePlayers(double sampleRate, int samplesPerBlock);
    void ProcessSamplePlayers(juce::AudioBuffer<float>& buffer);
    
  
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WeatherAmbienceGeneratorAudioProcessor)
};
