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

    nameLabel.setText(labelText, juce::dontSendNotification);
    nameLabel.setJustificationType(juce::Justification::centred);
    nameLabel.setFont(juce::Font(12.0f)); // Add this line - smaller font
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
        nameLabel.setBounds(bounds.removeFromTop(18)); // Slightly smaller
        bounds.removeFromTop(2); // Very small gap between label and slider
    }

    // Slider takes ALL remaining space - goes all the way to top
    slider.setBounds(bounds.reduced(15, 3)); // Less vertical reduction at top
}

void VerticalSliderWithLabel::updateValueLabel()
{
    //valueLabel.setText(juce::String(slider.getValue(), 2), juce::dontSendNotification);
}
void VerticalSliderWithLabel::setAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID)
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, parameterID, slider);
}