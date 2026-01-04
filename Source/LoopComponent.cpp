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
    // Title label
    titleLabel.setText(componentTitle, juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    //// Intensity level labels
    //calmLabel.setText("Calm", juce::dontSendNotification);
    //calmLabel.setJustificationType(juce::Justification::centred);
    //calmLabel.setFont(juce::Font(10.0f));
    //addAndMakeVisible(calmLabel);

    //mediumLabel.setText("Medium", juce::dontSendNotification);
    //mediumLabel.setJustificationType(juce::Justification::centred);
    //mediumLabel.setFont(juce::Font(10.0f));
    //addAndMakeVisible(mediumLabel);

    //strongLabel.setText("Strong", juce::dontSendNotification);
    //strongLabel.setJustificationType(juce::Justification::centred);
    //strongLabel.setFont(juce::Font(10.0f));
    //addAndMakeVisible(strongLabel);

    // Intensity slider (vertical) - range 0-2 for three-way crossfade
    intensityControl.getSlider().setRange(0.0, 2.0, 0.01);
    intensityControl.getSlider().setValue(1.0);  // Start at medium
    intensityControl.getSlider().onValueChange = [this]() { onIntensityChanged(); };
    addAndMakeVisible(intensityControl);

    // Gain slider (rotary)
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    gainSlider.setRange(-60.0, 0.0, 0.1);  // dB range
    gainSlider.setValue(-12.0);
    gainSlider.onValueChange = [this]() { onGainChanged(); };
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
    // Background
    g.fillAll(juce::Colour(0xFF2A2A3E));

    // Border
    g.setColour(juce::Colour(0xFF6FCFFF).withAlpha(0.3f));
    g.drawRect(getLocalBounds(), 1);
}

void LoopComponent::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    
    // Title at top
    titleLabel.setBounds(bounds.removeFromTop(25));
    
    bounds.removeFromTop(5); // gap
    
    // Reserve space for gain section at bottom
    auto gainSection = bounds.removeFromBottom(120);
    bounds.removeFromBottom(10); // gap
    
    // ALL remaining space for intensity control
    intensityControl.setBounds(bounds);
    
    // Gain section at bottom
    auto gainKnobArea = gainSection.removeFromTop(100);
    gainSlider.setBounds(gainKnobArea.reduced(10));
    
    // Gain label at very bottom
    gainLabel.setBounds(gainSection);
}

void LoopComponent::setPlayers(AudioSamplePlayer* calmPlayer,
    AudioSamplePlayer* mediumPlayer,
    AudioSamplePlayer* strongPlayer)
{
    crossfadeLogic.setPlayers(calmPlayer, mediumPlayer, strongPlayer);
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

void LoopComponent::onIntensityChanged()
{
    float value = static_cast<float>(intensityControl.getSlider().getValue());
    crossfadeLogic.setCrossfadeValue(value);
}

void LoopComponent::onGainChanged()
{
    float gainDB = static_cast<float>(gainSlider.getValue());
    float linearGain = juce::Decibels::decibelsToGain(gainDB);
    crossfadeLogic.setMasterGain(linearGain);
}