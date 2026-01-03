
#include "JuceHeader.h"
#include "AudioSamplePlayer.h"

class CrossfadeComponent : public juce::Component, private juce::Slider::Listener
{
public:
    CrossfadeComponent(const juce::String& label1, const juce::String& label2, const juce::String& label3);
    ~CrossfadeComponent();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setPlayers(AudioSamplePlayer* playerA_ptr,
        AudioSamplePlayer* playerB_ptr,
        AudioSamplePlayer* playerC_ptr);
    void attachMasterGain(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID);
    void attachCrossfadeParam(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID);
    float getCrossfadeValue() const;
    void setCrossfadeValue(float value);
    void setMasterGain(float gain);
private:
    void sliderValueChanged(juce::Slider* slider) override;
    void updatePlayerGains();

    juce::Slider crossfadeSlider;
    juce::Slider masterGainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crossfadeAttachment;
    juce::Label labelA, labelB, labelC;
    juce::Label centerLabel;
    juce::String nameA, nameB, nameC;
    std::atomic<float>masterGain{ 1.0f };

    AudioSamplePlayer* playerA = nullptr;
    AudioSamplePlayer* playerB = nullptr;
    AudioSamplePlayer* playerC = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CrossfadeComponent)
};