/*
  ==============================================================================

    VerticalSliderALNF.cpp
    Created: 4 Jan 2026 8:24:35pm
    Author:  bramd

  ==============================================================================
*/

#include "VerticalSliderALNF.h"


VerticalSliderWithLabel::VerticalSliderWithLabel(const juce::String& labelText)
{
    // Setup vertical slider
        slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setRange(0.0, 100.0, 0.1);
    slider.setValue(50.0);
    addAndMakeVisible(slider);

    // Setup name label (e.g., "Intensity")
    nameLabel.setText(labelText, juce::dontSendNotification);
    nameLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(nameLabel);

    // Setup value label (shows the number) - BIGGER FONT
    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setColour(juce::Label::textColourId, juce::Colour(0XFFA0FFFC));
    valueLabel.setFont(juce::Font(18.0f, juce::Font::bold));  // Add this line!
    addAndMakeVisible(valueLabel);

    // Update value label when slider changes
    slider.onValueChange = [this]() { updateValueLabel(); };
    updateValueLabel();
}

VerticalSliderWithLabel::~VerticalSliderWithLabel()
{
}

void VerticalSliderWithLabel::paint(juce::Graphics& g)
{
}

void VerticalSliderWithLabel::resized()
{
    auto bounds = getLocalBounds();

    // Name label at TOP (if not empty)
    if (nameLabel.getText().isNotEmpty())
    {
        nameLabel.setBounds(bounds.removeFromTop(20));
        bounds.removeFromTop(3); // small gap
    }

    // Value display below name
    valueLabel.setBounds(bounds.removeFromTop(35));

    bounds.removeFromTop(5); // gap

    // Slider takes remaining space
    slider.setBounds(bounds.reduced(15, 5));
}

void VerticalSliderWithLabel::updateValueLabel()
{
    //valueLabel.setText(juce::String(slider.getValue(), 2), juce::dontSendNotification);
}
void VerticalSliderWithLabel::setAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID)
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, parameterID, slider);
}