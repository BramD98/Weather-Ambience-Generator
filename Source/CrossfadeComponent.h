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

  
    void setPlayers(    AudioSamplePlayer* playerA_ptr,
                        AudioSamplePlayer* playerB_ptr,
                        AudioSamplePlayer* playerC_ptr);

    
    void setCrossfadeValue(float value);

    
    float getCrossfadeValue() const;

    
    void setMasterGain(float gain);

   
    float getMasterGain() const;

    void applyGains();

private:
    AudioSamplePlayer* playerA = nullptr;
    AudioSamplePlayer* playerB = nullptr;
    AudioSamplePlayer* playerC = nullptr;

    std::atomic<float> crossfadeValue{ 0.0f };
    std::atomic<float> masterGain{ 1.0f };

    bool areStopped = false;
    static constexpr float silenceThresholdDb = -79.0f;
};