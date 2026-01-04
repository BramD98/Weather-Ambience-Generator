/*
  ==============================================================================

    Font.cpp
    Created: 4 Jan 2026 7:41:45pm
    Author:  bramd

  ==============================================================================
*/

#include "Font.h"
const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(
    BinaryData::ABOFont_ttf, BinaryData::ABOFont_ttfSize);

juce::Font Fonts::getFont(float height)
{
    return juce::FontOptions(typeface)
        .withMetricsKind(juce::TypefaceMetricsKind::legacy)
        .withHeight(height);
}