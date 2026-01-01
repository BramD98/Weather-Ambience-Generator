
#include "CrossfadeComponent.h"


CrossfadeComponent::CrossfadeComponent(const juce::String& label1,
    const juce::String& label2,
    const juce::String& label3)
    : nameA(label1), nameB(label2), nameC(label3)
{
    // Setup crossfade slider - now 0.0 to 2.0 for three positions
    crossfadeSlider.setRange(0.0, 2.0, 0.01);
    crossfadeSlider.setValue(1.0);  // Start in the middle (player B)
    crossfadeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    crossfadeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    crossfadeSlider.addListener(this);
    addAndMakeVisible(crossfadeSlider);

    // Setup label A (left side)
    labelA.setText(nameA, juce::dontSendNotification);
    labelA.setJustificationType(juce::Justification::centredLeft);
    labelA.setFont(juce::Font(12.0f));
    addAndMakeVisible(labelA);

    // Setup label B (center)
    labelB.setText(nameB, juce::dontSendNotification);
    labelB.setJustificationType(juce::Justification::centred);
    labelB.setFont(juce::Font(12.0f));
    addAndMakeVisible(labelB);

    // Setup label C (right side)
    labelC.setText(nameC, juce::dontSendNotification);
    labelC.setJustificationType(juce::Justification::centredRight);
    labelC.setFont(juce::Font(12.0f));
    addAndMakeVisible(labelC);

    // Setup center label
    centerLabel.setText("Intensity", juce::dontSendNotification);
    centerLabel.setJustificationType(juce::Justification::centred);
    centerLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    addAndMakeVisible(centerLabel);
}

CrossfadeComponent::~CrossfadeComponent()
{
}

void CrossfadeComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.darker());

    g.setColour(juce::Colours::white);
    g.drawRect(getLocalBounds(), 1);

    // Draw visual crossfade indicator
    auto sliderArea = crossfadeSlider.getBounds();
    auto indicatorArea = sliderArea.reduced(0, -15);

    float crossfadeValue = static_cast<float>(crossfadeSlider.getValue());

    // Draw gradient showing mix (blue -> green -> red)
    juce::ColourGradient gradient(
        juce::Colours::blue, static_cast<float>(indicatorArea.getX()), 0.0f,
        juce::Colours::green, static_cast<float>(indicatorArea.getCentreX()), 0.0f,
        false
    );
    gradient.addColour(1.0, juce::Colours::red);

    g.setGradientFill(gradient);
    g.fillRect(indicatorArea);

    // Draw current position marker
    int markerX = static_cast<int>(indicatorArea.getX() + (crossfadeValue / 2.0f) * indicatorArea.getWidth());
    g.setColour(juce::Colours::white);
    g.drawLine(static_cast<float>(markerX), static_cast<float>(indicatorArea.getY()),
        static_cast<float>(markerX), static_cast<float>(indicatorArea.getBottom()),
        2.0f);
}

void CrossfadeComponent::resized()
{
    auto area = getLocalBounds().reduced(10);

    centerLabel.setBounds(area.removeFromTop(25));
    area.removeFromTop(5);

    auto labelArea = area.removeFromTop(20);
    int width = getWidth();
    labelA.setBounds(labelArea.removeFromLeft(width / 3 - 10));
    labelB.setBounds(labelArea.removeFromLeft(width / 3));
    labelC.setBounds(labelArea.removeFromRight(width / 3 - 10));

    area.removeFromTop(20);  // Space for gradient indicator
    crossfadeSlider.setBounds(area.removeFromTop(30));
}

void CrossfadeComponent::setPlayers(AudioSamplePlayer* playerA_ptr,
    AudioSamplePlayer* playerB_ptr,
    AudioSamplePlayer* playerC_ptr)
{
    playerA = playerA_ptr;
    playerB = playerB_ptr;
    playerC = playerC_ptr;

    // Set initial gains based on current slider position
    updatePlayerGains();
}

float CrossfadeComponent::getCrossfadeValue() const
{
    return static_cast<float>(crossfadeSlider.getValue());
}

void CrossfadeComponent::setCrossfadeValue(float value)
{
    crossfadeSlider.setValue(juce::jlimit(0.0f, 2.0f, value), juce::sendNotificationSync);
}

void CrossfadeComponent::setMasterGain(float gain)
{
    masterGain = gain;
}

void CrossfadeComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &crossfadeSlider)
    {
        updatePlayerGains();
        repaint();  // Redraw the visual indicator
    }
}

void CrossfadeComponent::updatePlayerGains()
{
    float crossfadeValue = static_cast<float>(crossfadeSlider.getValue());

    if (playerA != nullptr && playerB != nullptr && playerC != nullptr)
    {
        float gainA = 0.0f;
        float gainB = 0.0f;
        float gainC = 0.0f;

        // Three-way crossfade logic:
        // 0.0 - 1.0: Fade from A (full) to B (full), C is silent
        // 1.0 - 2.0: Fade from B (full) to C (full), A is silent

        if (crossfadeValue <= 1.0f)
        {
            // First half: crossfade between A and B
            gainA = 1.0f - crossfadeValue;  // 1.0 -> 0.0
            gainB = crossfadeValue;          // 0.0 -> 1.0
            gainC = 0.0f;
        }
        else
        {
            // Second half: crossfade between B and C
            float secondHalf = crossfadeValue - 1.0f;  // 0.0 -> 1.0
            gainA = 0.0f;
            gainB = 1.0f - secondHalf;  // 1.0 -> 0.0
            gainC = secondHalf;          // 0.0 -> 1.0
        }

        playerA->setGain(gainA * masterGain);
        playerB->setGain(gainB * masterGain);
        playerC->setGain(gainC * masterGain);

        DBG("Crossfade: " + juce::String(crossfadeValue, 2) +
            " | Gain A: " + juce::String(gainA, 2) +
            " | Gain B: " + juce::String(gainB, 2) +
            " | Gain C: " + juce::String(gainC, 2));
    }
}