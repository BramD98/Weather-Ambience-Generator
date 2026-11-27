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
    
    float currentCrossfade = crossfadeValue.load();
    float currentMasterGain = masterGain.load();

    
    if (playerA == nullptr || playerB == nullptr || playerC == nullptr)
        return;

    
    float masterGainDb = juce::Decibels::gainToDecibels(currentMasterGain, -80.0f);
    bool shouldBeSilent = (masterGainDb <= silenceThresholdDb);

    
    if (shouldBeSilent && !areStopped)
    {
        DBG(">>> STOPPING players - Master gain: " << masterGainDb << " dB");
        
        playerA->stop();
        playerB->stop();
        playerC->stop();
        areStopped = true;
        return; 
    }
    else if (!shouldBeSilent && areStopped)
    {
        DBG(">>> STARTING players - Master gain: " << masterGainDb << " dB");
        
        playerA->start();
        playerB->start();
        playerC->start();
        areStopped = false;
    }

    float gainA = 0.0f;
    float gainB = 0.0f;
    float gainC = 0.0f;

    

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

    
    playerA->setGain(gainA * currentMasterGain);
    playerB->setGain(gainB * currentMasterGain);
    playerC->setGain(gainC * currentMasterGain);
}


