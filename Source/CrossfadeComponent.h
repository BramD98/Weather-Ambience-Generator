/*
  ==============================================================================
    CrossfadeComponent.h - Logic only, no UI
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AudioSamplePlayer.h"

class CrossfadeComponent
{
public:
    CrossfadeComponent();
    ~CrossfadeComponent();

    // Set the three players
    void setPlayers(AudioSamplePlayer* playerA_ptr,
        AudioSamplePlayer* playerB_ptr,
        AudioSamplePlayer* playerC_ptr);

    // Set crossfade value (0.0 to 2.0)
    void setCrossfadeValue(float value);

    // Get current crossfade value
    float getCrossfadeValue() const;

    // Set master gain
    void setMasterGain(float gain);

    // Get master gain
    float getMasterGain() const;

private:
    AudioSamplePlayer* playerA = nullptr;
    AudioSamplePlayer* playerB = nullptr;
    AudioSamplePlayer* playerC = nullptr;

    float crossfadeValue = 0.0f;
    std::atomic<float> masterGain{ 1.0f };

    void updatePlayerGains();
};