#pragma once

#include <JuceHeader.h>

class ToggleButtonArcticLookAndFeel : public juce::ToggleButton
{
public:
    ToggleButtonArcticLookAndFeel();
    ~ToggleButtonArcticLookAndFeel() override;

    void setImages(const juce::Image& imageOff, const juce::Image& imageOn);

    void paintButton(juce::Graphics& g,
        bool isMouseOverButton,
        bool isButtonDown) override;

    void attachToParameter(juce::AudioProcessorValueTreeState& apvts,
        const juce::String& paramID);

protected:
    juce::Image offImage;
    juce::Image onImage;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleButtonArcticLookAndFeel)
};
