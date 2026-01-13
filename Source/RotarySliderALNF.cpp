/*
  ==============================================================================

    RotarySliderALNF.cpp
    Created: 4 Jan 2026 7:54:03pm
    Author:  bramd

  ==============================================================================
*/

#include "RotarySliderALNF.h"
#include "Font.h"

RotarySliderArcticLookAndFeel::RotarySliderArcticLookAndFeel(juce::Image ringImageToUse,
    juce::Image knobImageToUse,
    juce::Image textboxImageToUse)
    : ringImage(ringImageToUse),
    knobImage(knobImageToUse),
    textboxImage(textboxImageToUse)
{
}

juce::Font RotarySliderArcticLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont();
}

juce::Font RotarySliderArcticLookAndFeel::getSliderPopupFont(juce::Slider&)
{
    return juce::Font(14.0f, juce::Font::bold);
}

void RotarySliderArcticLookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x, int y,
    int width, int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    float alpha = slider.isEnabled() ? 1.0f : 0.3f;

    // ============================================
    // ACCOUNT FOR TEXT BOX BELOW
    // ============================================
    const int textBoxHeight = slider.getTextBoxHeight();
    const int knobAreaHeight = height - textBoxHeight;

    const float centerX = x + width * 0.5f;
    const float centerY = y + knobAreaHeight * 0.5f;

    const float knobSize = juce::jmin(width, knobAreaHeight) * 0.85f;
    const float ringSize = juce::jmin(width, knobAreaHeight);

    const float angle =
        rotaryStartAngle +
        sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    const float meterThickness = 3.0f;
    const float radius = ringSize * 0.51f - meterThickness;

    // Empty meter
    juce::Path emptyMeterPath;
    emptyMeterPath.addCentredArc(
        centerX, centerY,
        radius, radius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true
    );

    g.setColour(juce::Colour(0x334FCFFF).withMultipliedAlpha(alpha));
    g.strokePath(emptyMeterPath,
        juce::PathStrokeType(meterThickness,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded));

    // Active meter
    juce::Path meterPath;
    meterPath.addCentredArc(
        centerX, centerY,
        radius, radius,
        0.0f,
        rotaryStartAngle,
        angle,
        true
    );

    g.setColour(juce::Colour(0XFFA0FFFC).withMultipliedAlpha(alpha));
    g.strokePath(meterPath,
        juce::PathStrokeType(meterThickness,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded));

    // Knob image
    if (knobImage.isValid())
    {
        const float knobImageSize = (float)knobImage.getWidth();
        const float knobScale = knobSize / knobImageSize;

        juce::AffineTransform knobTransform =
            juce::AffineTransform::identity
            .translated(-knobImageSize * 0.5f, -knobImageSize * 0.5f)
            .scaled(knobScale)
            .rotated(angle)
            .translated(centerX, centerY);

        g.setOpacity(alpha);
        g.drawImageTransformed(knobImage, knobTransform, false);
        g.setOpacity(1.0f);
    }
}

void RotarySliderArcticLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    // Only apply style if this label is used as a slider textbox
    if (auto* slider = dynamic_cast<juce::Slider*>(label.getParentComponent()))
    {
        float alpha = slider->isEnabled() ? 1.0f : 0.3f;
        auto bounds = label.getLocalBounds().toFloat();

        // Draw image background
        if (textboxImage.isValid())
        {
            g.setOpacity(alpha);
            g.drawImageWithin(textboxImage,
                bounds.getX(), bounds.getY() + 1.0f,
                bounds.getWidth(), bounds.getHeight(),
                juce::RectanglePlacement::stretchToFit);
            g.setOpacity(1.0f);
        }
        else
        {
            // Fallback frosted dark background
            g.setColour(juce::Colour(0xFF1E1E2E).withMultipliedAlpha(alpha));
            g.fillRoundedRectangle(bounds, 8.0f);

            // Icy blue border
            g.setColour(juce::Colour(0xFF6FCFFF).withMultipliedAlpha(alpha));
            g.drawRoundedRectangle(bounds, 8.0f, 2.0f);
        }

        // Draw text on top
        g.setColour(juce::Colour(0XFFA0FFFC).withMultipliedAlpha(alpha));
        g.drawFittedText(label.getText(),
            bounds.toNearestInt(),
            juce::Justification::centred,
            1);

        return;
    }

    // Fallback: default drawing for other labels
    LookAndFeel_V4::drawLabel(g, label);
}