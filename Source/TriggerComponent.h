/*
  ==============================================================================

    TriggerComponent.h
    Created: 11 Jan 2026 6:29:57pm
    Author:  bramd

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AudioSamplePlayer.h"


class TriggerComponent : public juce::Component
{
public:
    TriggerComponent(const juce::String& title);
    ~TriggerComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    
    void setTriggerCallback(std::function<void()> callback);

   
    void setRandomToggleAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID);
    void setMinIntervalAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID);
    void setMaxIntervalAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID);
    void setGainAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID);
    void setPlayer(AudioSamplePlayer* player);
    
    void setButtonImage(const juce::Image& image);

   

private:
    juce::Label componentTitle;
    AudioSamplePlayer* playerPtr = nullptr;

    
    juce::TextButton triggerButton;
    juce::ToggleButton randomToggle;
    juce::Slider minIntervalSlider;
    juce::Slider maxIntervalSlider;
    juce::Label minIntervalLabel;
    juce::Label maxIntervalLabel;
    juce::Slider gainSlider;
    juce::Label gainLabel;

    
    juce::Image buttonBackgroundImage;

    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> randomToggleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> minIntervalAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> maxIntervalAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriggerComponent)
};