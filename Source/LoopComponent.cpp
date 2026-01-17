/*
  ==============================================================================
	LoopComponent.cpp
	Created: [Date]
	Author:  Bram
  ==============================================================================
*/

#include "LoopComponent.h"

LoopComponent::LoopComponent(const juce::String& title)
	: componentTitle(title),
	intensityControl("Intensity")
{
	componentTitle.setText(title, juce::dontSendNotification);
	componentTitle.setEditable(true, true, false);
	componentTitle.setJustificationType(juce::Justification::centred);
	componentTitle.setFont(juce::Font(19.0f, juce::Font::bold));
	componentTitle.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.9f));
	componentTitle.setColour(juce::Label::backgroundWhenEditingColourId, juce::Colour(0xFF2E2E45));
	componentTitle.setColour(juce::TextEditor::highlightColourId, juce::Colour(0xFF6FCFFF).withAlpha(0.3f));
	addAndMakeVisible(componentTitle);

	// Intensity slider (vertical) - range 0-2 for three-way crossfade
	intensityControl.getSlider().setRange(0.0, 2.0, 0.01);
	intensityControl.getSlider().setValue(1.0);
	addAndMakeVisible(intensityControl);

	// Gain slider (rotary)
	gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	gainSlider.setRange(-60.0, 0.0, 0.1);  // dB range
	gainSlider.setValue(-12.0);
	addAndMakeVisible(gainSlider);

	// Gain label
	gainLabel.setText("Gain", juce::dontSendNotification);
	gainLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(gainLabel);
}

LoopComponent::~LoopComponent()
{
}
void LoopComponent::paint(juce::Graphics& g)
{
	auto bounds = getLocalBounds().toFloat().reduced(1.0f);
	const float cornerRadius = 10.0f;

	// === COLOURS ===
	const juce::Colour panelTop = juce::Colour(0xFF2E2E45);
	const juce::Colour panelBottom = juce::Colour(0xFF1F1F32);
	const juce::Colour borderColour = juce::Colour(0xFF6FCFFF).withAlpha(0.35f);

	const juce::Colour separatorCore = juce::Colour(0xFF9FE7FF);
	const juce::Colour separatorGlow = juce::Colour(0xFF6FCFFF).withAlpha(0.18f);

	const float separatorMargin = 14.0f;
	const float separatorGlowThickness = 3.0f;
	const float separatorCoreThickness = 1.0f;

	// ============================================================
	// BACKGROUND
	// ============================================================

	juce::ColourGradient baseGradient(
		panelTop,
		bounds.getTopLeft(),
		panelBottom,
		bounds.getBottomLeft(),
		false
	);
	g.setGradientFill(baseGradient);
	g.fillRoundedRectangle(bounds, cornerRadius);

	juce::ColourGradient frostGlow(
		juce::Colour(0xFF6FCFFF).withAlpha(0.12f),
		bounds.getCentreX(), bounds.getY(),
		juce::Colours::transparentBlack,
		bounds.getCentreX(), bounds.getY() + bounds.getHeight() * 0.45f,
		false
	);
	g.setGradientFill(frostGlow);
	g.fillRoundedRectangle(bounds, cornerRadius);

	juce::ColourGradient vignette(
		juce::Colours::transparentBlack,
		bounds.getCentre(),
		juce::Colour(0xFF0D1020).withAlpha(0.35f),
		bounds.getTopLeft(),
		true
	);
	g.setGradientFill(vignette);
	g.fillRoundedRectangle(bounds, cornerRadius);

	// ============================================================
	// PANEL EDGE
	// ============================================================

	g.setColour(juce::Colours::white.withAlpha(0.04f));
	g.drawRoundedRectangle(bounds.reduced(1.0f), cornerRadius - 1.0f, 1.0f);

	g.setColour(borderColour);
	g.drawRoundedRectangle(bounds, cornerRadius, 1.5f);

	// ============================================================
	// TITLE - Now handled by componentTitle Label
	// (removed manual drawing)
	// ============================================================

	// ============================================================
	// TOP SEPARATOR (TITLE → INTENSITY)
	// ============================================================

	const float titleBottom = static_cast<float>(componentTitle.getBottom());
	const float intensityTop = static_cast<float>(intensityControl.getY());

	if (intensityTop > titleBottom)
	{
		const float topSeparatorY =
			titleBottom + (intensityTop - titleBottom) * 0.5f;

		juce::Line<float> topSeparator(
			separatorMargin,
			topSeparatorY,
			getWidth() - separatorMargin,
			topSeparatorY
		);

		g.setColour(separatorGlow);
		g.drawLine(topSeparator, separatorGlowThickness);

		g.setColour(separatorCore);
		g.drawLine(topSeparator, separatorCoreThickness);
	}

	// ============================================================
	// BOTTOM SEPARATOR (INTENSITY → GAIN)
	// ============================================================

	const float gainTop = static_cast<float>(gainSlider.getY());

	if (gainTop > 0.0f)
	{
		const float bottomSeparatorY = gainTop - 8.0f;

		juce::Line<float> bottomSeparator(
			separatorMargin,
			bottomSeparatorY,
			getWidth() - separatorMargin,
			bottomSeparatorY
		);

		g.setColour(separatorGlow);
		g.drawLine(bottomSeparator, separatorGlowThickness);

		g.setColour(separatorCore);
		g.drawLine(bottomSeparator, separatorCoreThickness);
	}
}





void LoopComponent::resized()
{
	auto bounds = getLocalBounds().reduced(10);

	// ================================
	// TITLE (now a Label component)
	// ================================
	const int titleHeight = 36;
	auto titleArea = bounds.removeFromTop(titleHeight);
	componentTitle.setBounds(titleArea.reduced(6, 0));  // ADD THIS LINE

	const int titleGap = 12;
	bounds.removeFromTop(titleGap);

	// ================================
	// GAIN SECTION DIMENSIONS
	// ================================
	const int gainLabelWidth = 44;
	const int gainKnobSize = 58;
	const int gainPadding = 8;

	const int gainSectionHeight =
		gainKnobSize + gainPadding * 2;

	// Gain area defines separator position
	auto gainArea = bounds.removeFromBottom(gainSectionHeight);

	// ================================
	// INTENSITY (ENDS AT SEPARATOR)
	// ================================
	intensityControl.setBounds(bounds);

	// ================================
	// AXIS ALIGNMENT
	// ================================
	const int sliderCentreX = intensityControl.getBounds().getCentreX();

	// ================================
	// GAIN (LABEL LEFT, KNOB UNDER SLIDER)
	// ================================
	gainArea = gainArea.reduced(10, gainPadding);

	// Knob — centered on intensity slider axis
	gainSlider.setBounds(
		sliderCentreX - gainKnobSize / 2,
		gainArea.getCentreY() - gainKnobSize / 2,
		gainKnobSize,
		gainKnobSize
	);

	// Label — to the left of knob, vertically centered
	const int labelHeight = 18;
	gainLabel.setBounds(
		sliderCentreX - gainKnobSize / 2 - gainLabelWidth - 6,
		gainArea.getCentreY() - labelHeight / 2,
		gainLabelWidth,
		labelHeight
	);
}




void LoopComponent::setIntensityAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID)
{
	intensityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		apvts, parameterID, intensityControl.getSlider());
}

void LoopComponent::setGainAttachment(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterID)
{
	gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		apvts, parameterID, gainSlider);
}
