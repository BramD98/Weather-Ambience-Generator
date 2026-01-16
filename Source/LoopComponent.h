/*
  ==============================================================================
    LoopComponent.h
    Created: [Date]
    Author:  Bram
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "VerticalSliderALNF.h"
#include "CrossfadeComponent.h"

class LoopComponent : public juce::Component
{
public:
    LoopComponent(const juce::String& title);
    ~LoopComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Set parameter attachments
    void setIntensityAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID);
    void setGainAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID);

private:
    juce::String componentTitle;
    juce::Label titleLabel;
    juce::Label calmLabel;
    juce::Label mediumLabel;
    juce::Label strongLabel;

    VerticalSliderWithLabel intensityControl;
    juce::Slider gainSlider;
    juce::Label gainLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intensityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoopComponent)
};