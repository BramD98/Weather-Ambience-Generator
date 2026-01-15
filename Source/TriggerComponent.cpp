/*
  ==============================================================================

    TriggerComponent.cpp
    Created: 11 Jan 2026 6:29:57pm
    Author:  bramd

  ==============================================================================
*/

#include "TriggerComponent.h"

TriggerComponent::TriggerComponent(const juce::String& title)
    : componentTitle(title)
{
    //Gain Setup
    // === GAIN SLIDER ===
    gainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
    gainSlider.setTextValueSuffix(" dB");
    gainSlider.setRotaryParameters(
        juce::MathConstants<float>::pi * 1.25f,
        juce::MathConstants<float>::pi * 2.75f,
        true
    );
    addAndMakeVisible(gainSlider);

    // Gain label
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setFont(juce::Font(12.0f));
    addAndMakeVisible(gainLabel);


    /// Setup trigger button - NO TEXT, just emoji
    triggerButton.setButtonText("Manual Trigger");
    triggerButton.setClickingTogglesState(false);
    triggerButton.onClick = [this]() {
        if (playerPtr != nullptr) {
            playerPtr->trigger();
        }
        };
    addAndMakeVisible(triggerButton);

    // Setup random toggle
    randomToggle.setButtonText("Random " + componentTitle);
    addAndMakeVisible(randomToggle);

    // Setup min interval slider - TEXT BOX ONLY
    minIntervalSlider.setSliderStyle(juce::Slider::IncDecButtons);
    minIntervalSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 60, 20);
    minIntervalSlider.setIncDecButtonsMode(juce::Slider::incDecButtonsDraggable_Vertical);
    addAndMakeVisible(minIntervalSlider);

    // Setup min interval label
    minIntervalLabel.setText("Min", juce::dontSendNotification);
    minIntervalLabel.setJustificationType(juce::Justification::centredRight);
    minIntervalLabel.setFont(juce::Font(12.0f));
    addAndMakeVisible(minIntervalLabel);

    // Setup max interval slider - TEXT BOX ONLY
    maxIntervalSlider.setSliderStyle(juce::Slider::IncDecButtons);
    maxIntervalSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 60, 20);
    maxIntervalSlider.setIncDecButtonsMode(juce::Slider::incDecButtonsDraggable_Vertical);
    addAndMakeVisible(maxIntervalSlider);

    // Setup max interval label
    maxIntervalLabel.setText("Max", juce::dontSendNotification);
    maxIntervalLabel.setJustificationType(juce::Justification::centredRight);
    maxIntervalLabel.setFont(juce::Font(12.0f));
    addAndMakeVisible(maxIntervalLabel);
}

TriggerComponent::~TriggerComponent()
{
}

void TriggerComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // === OUTER GLOW / SHADOW ===
    juce::Path outerPath;
    outerPath.addRoundedRectangle(bounds, 8.0f);

    juce::DropShadow shadow(juce::Colour(0xFF000000).withAlpha(0.6f), 8, juce::Point<int>(0, 2));
    shadow.drawForPath(g, outerPath);

    // === MAIN BACKGROUND - Frosted dark gradient ===
    juce::ColourGradient gradient(
        juce::Colour(0xFF1A1A2E), bounds.getX(), bounds.getY(),
        juce::Colour(0xFF16213E), bounds.getX(), bounds.getBottom(),
        false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 8.0f);

    // === INNER GLOW - Subtle cyan glow from top ===
    juce::ColourGradient innerGlow(
        juce::Colour(0xFF4FCFFF).withAlpha(0.15f), bounds.getCentreX(), bounds.getY(),
        juce::Colours::transparentBlack, bounds.getCentreX(), bounds.getY() + 60.0f,
        false);
    g.setGradientFill(innerGlow);
    g.fillRoundedRectangle(bounds.reduced(1.0f), 7.0f);

    // === GLASS REFLECTION - Top highlight ===
    auto reflectionArea = bounds.removeFromTop(bounds.getHeight() * 0.3f).reduced(8.0f, 4.0f);
    juce::ColourGradient reflection(
        juce::Colours::white.withAlpha(0.1f), reflectionArea.getCentreX(), reflectionArea.getY(),
        juce::Colours::transparentWhite, reflectionArea.getCentreX(), reflectionArea.getBottom(),
        false);
    g.setGradientFill(reflection);
    g.fillRoundedRectangle(reflectionArea, 4.0f);

    // NO OUTER BORDER - removed the full rectangle border

    // Draw title manually with forced bold font
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    auto titleBounds = getLocalBounds().removeFromTop(35).reduced(5);
    g.drawText(componentTitle, titleBounds, juce::Justification::centred);

    // === BLUE LINE UNDER TITLE ONLY ===
    g.setColour(juce::Colour(0xFF6FCFFF).withAlpha(0.6f));
    float lineY = titleBounds.getBottom() + 5.0f;
    g.drawLine(titleBounds.getX() + 10.0f, lineY,
        titleBounds.getRight() - 10.0f, lineY,
        2.0f);
}

void TriggerComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Skip title area
    bounds.removeFromTop(35);
    bounds.removeFromTop(10);

    // === VERTICAL SPLIT ===
    auto leftArea = bounds.removeFromLeft(bounds.getWidth() / 2).reduced(6);
    auto rightArea = bounds.reduced(6);

    // ======================================================
    // LEFT SIDE — RANDOM + INTERVALS
    // ======================================================

    const int rowHeight = 24;
    const int rowSpacing = 8;

    // Random toggle (centered horizontally)
    auto randomSlot = leftArea.removeFromTop(rowHeight);
    auto randomBounds = randomSlot.withSizeKeepingCentre(150, rowHeight);
    randomToggle.setBounds(randomBounds);

    leftArea.removeFromTop(rowSpacing );

    // Min interval
    auto minArea = leftArea.removeFromTop(rowHeight);
    minIntervalLabel.setBounds(minArea.removeFromLeft(32));
    minIntervalSlider.setBounds(minArea);

    leftArea.removeFromTop(rowSpacing);

    // Max interval
    auto maxArea = leftArea.removeFromTop(rowHeight);
    maxIntervalLabel.setBounds(maxArea.removeFromLeft(32));
    maxIntervalSlider.setBounds(maxArea);

    // ======================================================
    // RIGHT SIDE — TRIGGER + GAIN (WITH GAP)
    // ======================================================

    // Trigger button (smaller, centered)
    auto triggerSlot = rightArea.removeFromTop(rightArea.getHeight() * 0.35f);
    auto triggerBounds = triggerSlot.withSizeKeepingCentre(
        juce::jmin(triggerSlot.getWidth(), 120),
        32
    );
    triggerButton.setBounds(triggerBounds);

    // Explicit gap between trigger and gain
    rightArea.removeFromTop(10);

    // Gain knob (slightly smaller)
    auto gainArea = rightArea;

    int knobSize = juce::jmin(
        juce::jmax(gainArea.getWidth(), gainArea.getHeight()),
        80   // reduced from 90
    );

    auto knobBounds = gainArea.withSizeKeepingCentre(knobSize, knobSize);
    gainSlider.setBounds(knobBounds);

    // Label under knob
    gainLabel.setBounds(
        knobBounds.getX(),
        knobBounds.getBottom() + 6,
        knobBounds.getWidth(),
        16
    );
}

void TriggerComponent::setTriggerCallback(std::function<void()> callback)
{
    triggerButton.onClick = callback;
}

void TriggerComponent::setRandomToggleAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID)
{
    randomToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, parameterID, randomToggle);
}

void TriggerComponent::setMinIntervalAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID)
{
    minIntervalAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, parameterID, minIntervalSlider);
}

void TriggerComponent::setMaxIntervalAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID)
{
    maxIntervalAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, parameterID, maxIntervalSlider);
}

void TriggerComponent::setButtonImage(const juce::Image& image)
{
    buttonBackgroundImage = image;
    // You can apply this image to the button background in a custom LookAndFeel if needed
}

void TriggerComponent::setGainAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID)
{
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, parameterID, gainSlider);

}

void TriggerComponent::setPlayer(AudioSamplePlayer* player)
{
    playerPtr = player;
}
