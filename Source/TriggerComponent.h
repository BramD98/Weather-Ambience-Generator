/*
  ==============================================================================

    TriggerComponent.h
    Created: 11 Jan 2026 6:29:57pm
    Author:  bramd

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class TriggerComponent : public juce::Component
{
public:
    TriggerComponent(const juce::String& title);
    ~TriggerComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Set the trigger callback
    void setTriggerCallback(std::function<void()> callback);

    // Set parameter attachments
    void setRandomToggleAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID);
    void setMinIntervalAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID);
    void setMaxIntervalAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID);
    void setGainAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID);
    // Set button background image
    void setButtonImage(const juce::Image& image);

   

private:
    juce::String componentTitle;

    // UI Components
    juce::TextButton triggerButton;
    juce::ToggleButton randomToggle;
    juce::Slider minIntervalSlider;
    juce::Slider maxIntervalSlider;
    juce::Label minIntervalLabel;
    juce::Label maxIntervalLabel;
    juce::Slider gainSlider;
    juce::Label gainLabel;

    // Button background image
    juce::Image buttonBackgroundImage;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> randomToggleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> minIntervalAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> maxIntervalAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriggerComponent)
};