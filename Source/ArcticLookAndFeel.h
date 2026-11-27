/*
  ==============================================================================

    ArcticLookAndFeel.h
    Created: 4 Jan 2026 7:47:02pm
    Author:  bramd

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class ArcticLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ArcticLookAndFeel();

    juce::Font getLabelFont(juce::Label&) override;
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;
    juce::Font getPopupMenuFont() override;

    void drawButtonBackground(juce::Graphics& g,
        juce::Button& button,
        const juce::Colour& backgroundColour,
        bool isMouseOverButton,
        bool isButtonDown) override;

    void drawComboBox(juce::Graphics& g, int width, int height,
        bool isButtonDown, int buttonX, int buttonY,
        int buttonW, int buttonH, juce::ComboBox& box) override;

    void drawLinearSlider(juce::Graphics& g,
        int x, int y,
        int width, int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const juce::Slider::SliderStyle style,
        juce::Slider& slider) override;
    void setRotaryKnobImages(juce::Image ringImageToUse,
        juce::Image knobImageToUse,
        juce::Image textboxImageToUse);
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override;

    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    juce::Font getSliderPopupFont(juce::Slider&) override;
   
private:
    juce::Image buttonBackgroundImg;
    juce::Image ringImage;
    juce::Image knobImage;
    juce::Image textboxImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ArcticLookAndFeel)
};