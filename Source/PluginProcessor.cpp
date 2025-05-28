/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EasyProjectTimerAudioProcessor::EasyProjectTimerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

EasyProjectTimerAudioProcessor::~EasyProjectTimerAudioProcessor()
{
}

//==============================================================================
const juce::String EasyProjectTimerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EasyProjectTimerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EasyProjectTimerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EasyProjectTimerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EasyProjectTimerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EasyProjectTimerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EasyProjectTimerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EasyProjectTimerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EasyProjectTimerAudioProcessor::getProgramName (int index)
{
    return {};
}

void EasyProjectTimerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EasyProjectTimerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void EasyProjectTimerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EasyProjectTimerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void EasyProjectTimerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Get playback state from host
    auto playHead = getPlayHead();
    bool currentlyPlaying = false;
    
    if (playHead != nullptr)
    {
        auto positionInfo = playHead->getPosition();
        if (positionInfo.hasValue())
        {
            currentlyPlaying = positionInfo->getIsPlaying();
        }
    }
    
    // Handle playback state changes
    if (currentlyPlaying && !isPlaying)
    {
        // Just started playing
        lastPlaybackStart = juce::Time::getCurrentTime();
        isPlaying = true;
    }
    else if (!currentlyPlaying && isPlaying)
    {
        // Just stopped playing - add the elapsed time
        auto elapsed = juce::Time::getCurrentTime() - lastPlaybackStart;
        totalPlaybackTime += elapsed.inSeconds();
        isPlaying = false;
    }
    
    // Pass audio through unchanged
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
}

//==============================================================================
bool EasyProjectTimerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EasyProjectTimerAudioProcessor::createEditor()
{
    return new EasyProjectTimerAudioProcessorEditor (*this);
}

//==============================================================================
void EasyProjectTimerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save the total time in the plugin state
    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("EasyProjectTimer"));
    xml->setAttribute ("totalTime", totalPlaybackTime);
    copyXmlToBinary (*xml, destData);
}

void EasyProjectTimerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore the total time from plugin state
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName ("EasyProjectTimer"))
        {
            totalPlaybackTime = xmlState->getDoubleAttribute ("totalTime", 0.0);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EasyProjectTimerAudioProcessor();
}
