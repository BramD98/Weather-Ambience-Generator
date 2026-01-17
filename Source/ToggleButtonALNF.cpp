/*
  ==============================================================================

    ToggleButtonALNF.cpp
    Created: 17 Jan 2026 4:18:58pm
    Author:  bramd

  ==============================================================================
*/

#include "ToggleButtonALNF.h"


ToggleButtonArcticLookAndFeel::ToggleButtonArcticLookAndFeel()
{
   
}


ToggleButtonArcticLookAndFeel::~ToggleButtonArcticLookAndFeel() = default;

void ToggleButtonArcticLookAndFeel::setImages(const juce::Image& imageOff,
    const juce::Image& imageOn)
{
    offImage = imageOff;
    onImage = imageOn;
    repaint();
}

void ToggleButtonArcticLookAndFeel::paintButton(juce::Graphics& g,
    bool isMouseOverButton,
    bool isButtonDown)
{
    juce::Image img = getToggleState() ? onImage : offImage;

    if (!img.isValid())
    {
        g.fillAll(juce::Colours::darkgrey);
        return;
    }

    float opacity = isEnabled() ? 1.0f : 0.3f;

    if (isMouseOverButton)
        opacity *= 1.1f;

    // Clamp to valid range
    opacity = juce::jlimit(0.0f, 1.0f, opacity);

    g.setOpacity(opacity);

    auto transform = juce::RectanglePlacement(juce::RectanglePlacement::centred)
        .getTransformToFit(img.getBounds().toFloat(),
            getLocalBounds().toFloat());

    // Optional: tiny glow around button on hover
    if (isMouseOverButton)
    {
        transform.scaled(1.25f);
    }
    g.drawImageTransformed(img, transform);

    
}

void ToggleButtonArcticLookAndFeel::attachToParameter(juce::AudioProcessorValueTreeState& apvts,
    const juce::String& paramID)
{
    buttonAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, paramID, *this);
}
