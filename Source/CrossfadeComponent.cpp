/*
  ==============================================================================
    CrossfadeComponent.cpp - Logic only, no UI
  ==============================================================================
*/

#include "CrossfadeComponent.h"

CrossfadeComponent::CrossfadeComponent()
{
}

CrossfadeComponent::~CrossfadeComponent()
{
}

void CrossfadeComponent::setPlayers(AudioSamplePlayer* playerA_ptr,
    AudioSamplePlayer* playerB_ptr,
    AudioSamplePlayer* playerC_ptr)
{
    playerA = playerA_ptr;
    playerB = playerB_ptr;
    playerC = playerC_ptr;

    updatePlayerGains();
}

void CrossfadeComponent::setCrossfadeValue(float value)
{
    crossfadeValue = juce::jlimit(0.0f, 2.0f, value);
    updatePlayerGains();
}

float CrossfadeComponent::getCrossfadeValue() const
{
    return crossfadeValue;
}

void CrossfadeComponent::setMasterGain(float gain)
{
    masterGain.store(gain);
    updatePlayerGains();
}

float CrossfadeComponent::getMasterGain() const
{
    return masterGain.load();
}

void CrossfadeComponent::updatePlayerGains()
{
    float currentMasterGain = masterGain.load();

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

        playerA->setGain(gainA * currentMasterGain);
        playerB->setGain(gainB * currentMasterGain);
        playerC->setGain(gainC * currentMasterGain);
    }
}