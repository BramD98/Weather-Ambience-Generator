/*
  ==============================================================================

    VerticalSliderALNF.h
    Created: 4 Jan 2026 8:24:35pm
    Author:  bramd

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class VerticalSliderWithLabel : public juce::Component
{
public:
    VerticalSliderWithLabel(const juce::String& labelText);
    ~VerticalSliderWithLabel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    juce::Slider& getSlider() { return slider; }
    void setAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID);
    void setValueSuffix(const juce::String& suffix) { valueSuffix = suffix; }
    void setDisplayMultiplier(float multiplier) { displayMultiplier = multiplier; }
private:
    juce::Slider slider;
    juce::Label nameLabel;
    juce::Label valueLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    juce::String valueSuffix;
    float displayMultiplier = 1.0f;
    void updateValueLabel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VerticalSliderWithLabel)
};