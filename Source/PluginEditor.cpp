/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EasyProjectTimerAudioProcessorEditor::EasyProjectTimerAudioProcessorEditor (EasyProjectTimerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (300, 150);
        
    // Setup time display label
    timeLabel.setFont (juce::Font (24.0f, juce::Font::bold));
    timeLabel.setJustificationType (juce::Justification::centred);
    timeLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    timeLabel.setText ("00:00:00", juce::dontSendNotification);
    addAndMakeVisible (timeLabel);
    
    // Setup status label
    statusLabel.setFont (juce::Font (14.0f));
    statusLabel.setJustificationType (juce::Justification::centred);
    statusLabel.setColour (juce::Label::textColourId, juce::Colours::lightgrey);
    statusLabel.setText ("Stopped", juce::dontSendNotification);
    addAndMakeVisible (statusLabel);
    
    // Setup reset button
    resetButton.setButtonText ("Reset Timer");
    resetButton.onClick = [this]()
    {
        audioProcessor.resetTimer();
    };
    addAndMakeVisible (resetButton);
    
    // Start timer to update display
    startTimerHz (10); // Update 10 times per second
}

EasyProjectTimerAudioProcessorEditor::~EasyProjectTimerAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void EasyProjectTimerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill background
    g.fillAll (juce::Colour (0xff2d2d2d));
    
    // Draw border
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);
    
    // Draw title
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    g.drawFittedText ("Easy Project Timer", 0, 10, getWidth(), 20,
                      juce::Justification::centred, 1);
}

void EasyProjectTimerAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce (10, 10);
    
    // Title space
    bounds.removeFromTop (30);
    
    // Time display
    timeLabel.setBounds (bounds.removeFromTop (40));
    
    // Status
    statusLabel.setBounds (bounds.removeFromTop (25));
    
    bounds.removeFromTop (10); // Spacing
    
    // Reset button
    resetButton.setBounds (bounds.removeFromTop (30).reduced (60, 0));
}

void EasyProjectTimerAudioProcessorEditor::timerCallback()
{
    // Update time display
    double currentTotal = audioProcessor.getTotalTime();
    
    // If currently playing, add the time since playback started
    if (audioProcessor.isCurrentlyPlaying())
    {
        // We need to estimate current session time since we can't access
        // the private lastPlaybackStart from here
        // The processor handles the actual timing, this is just for display
        statusLabel.setText ("Playing", juce::dontSendNotification);
        statusLabel.setColour (juce::Label::textColourId, juce::Colours::lightgreen);
    }
    else
    {
        statusLabel.setText ("Stopped", juce::dontSendNotification);
        statusLabel.setColour (juce::Label::textColourId, juce::Colours::lightgrey);
    }
    
    timeLabel.setText (formatTime (currentTotal), juce::dontSendNotification);
}

juce::String EasyProjectTimerAudioProcessorEditor::formatTime(double seconds)
{
    int totalSeconds = static_cast<int>(seconds);
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int secs = totalSeconds % 60;
    
    return juce::String::formatted ("%02d:%02d:%02d", hours, minutes, secs);
}
