/*
  ==============================================================================

    RotarySliderALNF.h
    Created: 4 Jan 2026 7:54:03pm
    Author:  bramd

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class RotarySliderArcticLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RotarySliderArcticLookAndFeel(juce::Image ringImageToUse,
        juce::Image knobImageToUse,
        juce::Image textboxImageToUse);

    juce::Font getLabelFont(juce::Label&) override;
    juce::Font getSliderPopupFont(juce::Slider&) override;

    void drawRotarySlider(juce::Graphics& g,
        int x, int y,
        int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider) override;

    void drawLabel(juce::Graphics& g, juce::Label& label) override;

private:
    juce::Image ringImage;
    juce::Image textboxImage;
    juce::Image knobImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotarySliderArcticLookAndFeel)
};