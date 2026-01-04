/*
  ==============================================================================

    ArcticLookAndFeel.cpp
    Created: 4 Jan 2026 7:47:02pm
    Author:  bramd

  ==============================================================================
*/

#include "ArcticLookAndFeel.h"
#include "Font.h"

ArcticLookAndFeel::ArcticLookAndFeel()
{
    buttonBackgroundImg = juce::ImageFileFormat::loadFrom(
        BinaryData::button_bg_png, BinaryData::button_bg_pngSize);
}

juce::Font ArcticLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont();
}

juce::Font ArcticLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    return Fonts::getFont();
}

juce::Font ArcticLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return Fonts::getFont();
}

juce::Font ArcticLookAndFeel::getPopupMenuFont()
{
    return Fonts::getFont();
}

void ArcticLookAndFeel::drawButtonBackground(juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& backgroundColour,
    bool isMouseOverButton,
    bool isButtonDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    float baseAlpha = button.isEnabled() ? 1.0f : 0.3f;
    juce::Colour overlay = juce::Colours::transparentBlack;

    if (isButtonDown)
    {
        baseAlpha = 0.7f;
        overlay = juce::Colours::black.withAlpha(0.2f);
    }
    else if (isMouseOverButton)
    {
        baseAlpha = 1.0f;
        overlay = juce::Colours::white.withAlpha(0.15f);
    }

    if (buttonBackgroundImg.isValid())
    {
        g.setOpacity(baseAlpha);
        g.drawImageWithin(buttonBackgroundImg,
            bounds.getX(), bounds.getY(),
            bounds.getWidth(), bounds.getHeight(),
            juce::RectanglePlacement::stretchToFit);

        g.setOpacity(1.0f);
        g.setColour(overlay);
        g.fillRoundedRectangle(bounds, 6.0f);
    }
    else
    {
        g.setColour(juce::Colours::darkgrey.withMultipliedAlpha(baseAlpha));
        g.fillRoundedRectangle(bounds, 6.0f);
    }
}

void ArcticLookAndFeel::drawComboBox(juce::Graphics& g,
    int width, int height,
    bool isButtonDown,
    int buttonX, int buttonY,
    int buttonW, int buttonH,
    juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<float>(0, 0, (float)width, (float)height);
    float baseAlpha = box.isEnabled() ? 1.0f : 0.3f;

    if (buttonBackgroundImg.isValid())
    {
        g.setOpacity(baseAlpha);
        g.drawImageWithin(buttonBackgroundImg,
            0, 0, width, height,
            juce::RectanglePlacement::stretchToFit);
    }
    else
    {
        g.setColour(juce::Colours::darkgrey.withMultipliedAlpha(baseAlpha));
        g.fillRoundedRectangle(bounds, 6.0f);
    }
}
void ArcticLookAndFeel::drawLinearSlider(juce::Graphics& g,
    int x, int y,
    int width, int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const juce::Slider::SliderStyle style,
    juce::Slider& slider)
{
    // Only handle vertical sliders, let base class handle others
    if (style != juce::Slider::LinearVertical)
    {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos,
            minSliderPos, maxSliderPos, style, slider);
        return;
    }

    float alpha = slider.isEnabled() ? 1.0f : 0.3f;

    // Track dimensions
    const float trackWidth = 6.0f;
    const float trackX = x + (width - trackWidth) * 0.5f;
    const float trackY = y;
    const float trackHeight = height;

    // Draw background track (empty part)
    g.setColour(juce::Colour(0xFF1E1E2E).withMultipliedAlpha(alpha));
    g.fillRoundedRectangle(trackX, trackY, trackWidth, trackHeight, trackWidth * 0.5f);

    // Draw filled track (from bottom up to slider position)
    float fillHeight = trackY + trackHeight - sliderPos;
    g.setColour(juce::Colour(0XFFA0FFFC).withMultipliedAlpha(alpha));
    g.fillRoundedRectangle(trackX, sliderPos, trackWidth, fillHeight, trackWidth * 0.5f);

    // Draw thumb (circular handle)
    const float thumbSize = 16.0f;
    const float thumbX = x + (width - thumbSize) * 0.5f;
    const float thumbY = sliderPos - thumbSize * 0.5f;

    // Thumb shadow
    g.setColour(juce::Colours::black.withAlpha(0.3f * alpha));
    g.fillEllipse(thumbX + 1, thumbY + 1, thumbSize, thumbSize);

    // Thumb main color
    g.setColour(juce::Colour(0xFF6FCFFF).withMultipliedAlpha(alpha));
    g.fillEllipse(thumbX, thumbY, thumbSize, thumbSize);

    // Thumb highlight (small bright dot for 3D effect)
    g.setColour(juce::Colour(0XFFA0FFFC).withMultipliedAlpha(alpha * 0.6f));
    g.fillEllipse(thumbX + 3, thumbY + 3, thumbSize * 0.4f, thumbSize * 0.4f);
}
void ArcticLookAndFeel::setRotaryKnobImages(juce::Image ringImageToUse,
    juce::Image knobImageToUse,
    juce::Image textboxImageToUse)
{
    ringImage = ringImageToUse;
    knobImage = knobImageToUse;
    textboxImage = textboxImageToUse;
}

juce::Font ArcticLookAndFeel::getSliderPopupFont(juce::Slider&)
{
    return juce::Font(14.0f, juce::Font::bold);
}

void ArcticLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPosProportional, float rotaryStartAngle,
    float rotaryEndAngle, juce::Slider& slider)
{
    float alpha = slider.isEnabled() ? 1.0f : 0.3f;
    const float centerX = x + width * 0.5f;
    const float centerY = y + height * 0.5f;

    const float knobSize = juce::jmin(width, height) * 0.85f;
    const float ringSize = juce::jmin(width, height) * 1.0f;

    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    const float meterThickness = 3.0f;
    const float radius = ringSize * 0.51f - meterThickness;

    // Draw empty meter
    juce::Path emptyMeterPath;
    emptyMeterPath.addCentredArc(centerX, centerY, radius, radius, 0.0f,
        rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0x334FCFFF).withMultipliedAlpha(alpha));
    g.strokePath(emptyMeterPath, juce::PathStrokeType(meterThickness,
        juce::PathStrokeType::curved,
        juce::PathStrokeType::rounded));

    // Draw active meter
    juce::Path meterPath;
    meterPath.addCentredArc(centerX, centerY, radius, radius, 0.0f,
        rotaryStartAngle, angle, true);
    g.setColour(juce::Colour(0XFFA0FFFC).withMultipliedAlpha(alpha));
    g.strokePath(meterPath, juce::PathStrokeType(meterThickness,
        juce::PathStrokeType::curved,
        juce::PathStrokeType::rounded));

    // Draw knob image
    if (knobImage.isValid())
    {
        const float knobImageSize = static_cast<float>(knobImage.getWidth());
        const float knobScale = knobSize / knobImageSize;

        juce::AffineTransform knobTransform =
            juce::AffineTransform::identity
            .translated(-knobImageSize * 0.5f, -knobImageSize * 0.5f)
            .scaled(knobScale)
            .rotated(angle)
            .translated(centerX, centerY);

        g.setOpacity(slider.isEnabled() ? 1.0f : 0.3f);
        g.drawImageTransformed(knobImage, knobTransform, false);
        g.setOpacity(1.0f);
    }
    else if (!ringImage.isValid())
    {
        g.setColour(juce::Colours::grey);
        g.fillEllipse(x + 4, y + 4, width - 8, height - 8);
    }
}

void ArcticLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    // Only apply custom style if this label is used as a slider textbox
    if (auto* slider = dynamic_cast<juce::Slider*>(label.getParentComponent()))
    {
        // Only for rotary sliders
        if (slider->getSliderStyle() != juce::Slider::RotaryHorizontalVerticalDrag &&
            slider->getSliderStyle() != juce::Slider::RotaryVerticalDrag &&
            slider->getSliderStyle() != juce::Slider::RotaryHorizontalDrag)
        {
            LookAndFeel_V4::drawLabel(g, label);
            return;
        }

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