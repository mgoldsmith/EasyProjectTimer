/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class EasyProjectTimerAudioProcessorEditor  : public juce::AudioProcessorEditor,
private juce::Timer
{
public:
    EasyProjectTimerAudioProcessorEditor (EasyProjectTimerAudioProcessor&);
    ~EasyProjectTimerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EasyProjectTimerAudioProcessor& audioProcessor;
    
    void timerCallback() override;
    juce::String formatTime(double seconds);
    
    juce::Label timeLabel;
    juce::Label statusLabel;
    juce::TextButton resetButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyProjectTimerAudioProcessorEditor)
};
