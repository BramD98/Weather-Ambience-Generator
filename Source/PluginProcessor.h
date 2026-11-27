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
#include "CrossfadeComponent.h"
#include "WeatherAudioMapper.h"



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
    Parameters params;
    WeatherManager* getWeatherManager() { return weatherManager.get(); }

    void updateWeatherParameters(const WeatherFetcher::WeatherData& data);

    // Loopers
    AudioSamplePlayer calmWindPlayer;
    AudioSamplePlayer midWindPlayer;
    AudioSamplePlayer strongWindPlayer;

    AudioSamplePlayer calmRainPlayer;
    AudioSamplePlayer midRainPlayer;
    AudioSamplePlayer heavyRainPlayer;

    AudioSamplePlayer calmFirePlayer;
    AudioSamplePlayer midFirePlayer;
    AudioSamplePlayer heavyFirePlayer;

    AudioSamplePlayer calmCityPlayer;
    AudioSamplePlayer midCityPlayer;
    AudioSamplePlayer heavyCityPlayer;

    AudioSamplePlayer calmWaterPlayer;
    AudioSamplePlayer midWaterPlayer;
    AudioSamplePlayer heavyWaterPlayer;

    //One Shots
    AudioSamplePlayer thunderPlayer;
    AudioSamplePlayer earthquakePlayer;
    AudioSamplePlayer beastPlayer;

    CrossfadeComponent windCrossfade;
    CrossfadeComponent rainCrossfade;
    CrossfadeComponent fireCrossfade;
    CrossfadeComponent cityCrossfade;
    CrossfadeComponent waterCrossfade;
    
    FileLoader fileLoader;
    juce::dsp::StateVariableTPTFilter<float> lowPassFilter;
    juce::dsp::Limiter<float> limiter;
    


private:
    //==============================================================================
    std::unique_ptr <WeatherManager> weatherManager;
    WeatherAudioMapper weatherMapper;
    void FileLoaderRegisterPlayers();
    void PrepareSamplePlayers(double sampleRate, int samplesPerBlock);
    void ProcessSamplePlayers(juce::AudioBuffer<float>& buffer);
    std::atomic<bool> weatherControlEnabled{ false };
    WeatherAudioMapper::AudioParameters weatherParams;
    juce::SpinLock weatherParamsLock;
    float lowpassCutoffFreq = 1750.0f;
    float calculateSmartGainCompensation();
  
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WeatherAmbienceGeneratorAudioProcessor)
};
