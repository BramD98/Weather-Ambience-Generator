// ============================================================================
// FileLoader.h
// ============================================================================
#pragma once

#include <JuceHeader.h>
#include "AudioSamplePlayer.h"
#include <map>
#include <memory>

class FileLoader : public juce::Component
{
public:
    FileLoader();
    ~FileLoader() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Register a sample player with a name
    void registerPlayer(const juce::String& name, AudioSamplePlayer* player);

    // Set the current sample rate for resampling
    void setSampleRate(double sampleRate) { currentSampleRate = sampleRate; }

    bool loadFileForPlayer(const juce::String& playerName, const juce::File& file);

private:
    juce::TextButton loadFilesButton;
  
    juce::TextEditor sampleList;
    std::map<juce::String, juce::String> loadedFiles;

    std::map<juce::String, AudioSamplePlayer*> samplePlayers;
    juce::AudioFormatManager formatManager;
    double currentSampleRate = 44100.0;

    void showSampleSelectionMenu();
    void loadFilesButtonClicked();
    void loadNextSample(const std::vector<juce::String>& playerNames,
        int currentIndex,
        std::shared_ptr<int> totalLoaded);

    // Core file loading with resampling
    bool loadAndPrepareFile(const juce::File& file,
        juce::AudioBuffer<float>& outBuffer,
        double& outSourceSampleRate);
   
    void updateSampleList();
    void showLoadFilesMenu();
    void openFileChooserForPlayer(const juce::String& playerName);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileLoader)
};