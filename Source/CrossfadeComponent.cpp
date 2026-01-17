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

    //updatePlayerGains();
}

void CrossfadeComponent::setCrossfadeValue(float value)
{
    crossfadeValue.store(juce::jlimit(0.0f, 2.0f, value));
}

float CrossfadeComponent::getCrossfadeValue() const
{
    return crossfadeValue.load();
}

void CrossfadeComponent::setMasterGain(float gain)
{
    masterGain.store(gain);
}

float CrossfadeComponent::getMasterGain() const
{
    return masterGain.load();
}

void CrossfadeComponent::applyGains()
{
    // Read atomic values once at the start of this buffer
    float currentCrossfade = crossfadeValue.load();
    float currentMasterGain = masterGain.load();

    // Safety check
    if (playerA == nullptr || playerB == nullptr || playerC == nullptr)
        return;

    // ADDED: Convert master gain to dB to check if we're effectively silent
    float masterGainDb = juce::Decibels::gainToDecibels(currentMasterGain, -80.0f);
    bool shouldBeSilent = (masterGainDb <= silenceThresholdDb);

    // ADDED: Start/Stop logic to save CPU
    if (shouldBeSilent && !areStopped)
    {
        DBG(">>> STOPPING players - Master gain: " << masterGainDb << " dB");
        // Stop all players when effectively silent
        playerA->stop();
        playerB->stop();
        playerC->stop();
        areStopped = true;
        return; // No need to calculate gains if stopped
    }
    else if (!shouldBeSilent && areStopped)
    {
        DBG(">>> STARTING players - Master gain: " << masterGainDb << " dB");
        // Restart players when volume comes back up
        playerA->start();
        playerB->start();
        playerC->start();
        areStopped = false;
    }

    float gainA = 0.0f;
    float gainB = 0.0f;
    float gainC = 0.0f;

    // Three-way crossfade logic:
    // 0.0 - 1.0: Fade from A (full) to B (full), C is silent
    // 1.0 - 2.0: Fade from B (full) to C (full), A is silent

    if (currentCrossfade <= 1.0f)
    {
        // First half: crossfade between A and B
        gainA = 1.0f - currentCrossfade;  // 1.0 -> 0.0
        gainB = currentCrossfade;          // 0.0 -> 1.0
        gainC = 0.0f;
    }
    else
    {
        // Second half: crossfade between B and C
        float secondHalf = currentCrossfade - 1.0f;  // 0.0 -> 1.0
        gainA = 0.0f;
        gainB = 1.0f - secondHalf;  // 1.0 -> 0.0
        gainC = secondHalf;          // 0.0 -> 1.0
    }

    // Apply final gains (crossfade * master) to all players
    // This happens on the audio thread, so it's safe
    playerA->setGain(gainA * currentMasterGain);
    playerB->setGain(gainB * currentMasterGain);
    playerC->setGain(gainC * currentMasterGain);
}


