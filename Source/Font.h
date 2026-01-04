/*
  ==============================================================================

    Font.h
    Created: 4 Jan 2026 7:41:45pm
    Author:  bramd

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Fonts
{
public:
    Fonts() = delete;
    static juce::Font getFont(float height = 16.0f);

private:
    static const juce::Typeface::Ptr typeface;
};