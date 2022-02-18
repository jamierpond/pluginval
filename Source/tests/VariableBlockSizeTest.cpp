/*
  ==============================================================================

    CVariableBlockSizeTest.cpp
    Created: 9 Sep 2021 12:32:06pm
    Author:  Jamie

  ==============================================================================
*/

#include "../PluginTests.h"
#include "../TestUtilities.h"
#include "SubBlockProcessingTest.h"
#include <future>
#include <random>

bool gbWriteVariableBufferAudioToWavFile = false;



//==============================================================================

struct CVariableBlockSizeConsistencyTest : public PluginTest
{
    CVariableBlockSizeConsistencyTest()
        : PluginTest("Variable blocksize CONTINUITY/SOUND test", 3)
    {
        
    }

    void runTest(PluginTests& ut, AudioPluginInstance& instance) override
    {

        auto pd = instance.getPluginDescription();
        auto fixedInstanceA = ut.testOpenPlugin(pd);
        auto fixedInstanceB = ut.testOpenPlugin(pd);

        const bool subnormalsAreErrors = ut.getOptions().strictnessLevel > 5;
        const bool isPluginInstrument = instance.getPluginDescription().isInstrument;

        const std::vector<double>& sampleRates = ut.getOptions().sampleRates;
        const std::vector<int>& blockSizes = ut.getOptions().blockSizes;

        jassert(sampleRates.size() > 0 && blockSizes.size() > 0);
        instance.prepareToPlay(sampleRates[0], blockSizes[0]);
        fixedInstanceA->prepareToPlay(sampleRates[0], blockSizes[0]);
        fixedInstanceB->prepareToPlay(sampleRates[0], blockSizes[0]);

        auto r = ut.getRandom();

        int numChannels = jmax(instance.getTotalNumInputChannels(), instance.getTotalNumOutputChannels());

        size_t numBuffersToTest = size_t(sampleRates.size() * blockSizes.size());

        ArrayOfAudioBufferPointers variableBlockBuffers(numBuffersToTest);
        ArrayOfAudioBufferPointers fixedBlockBuffersA(numBuffersToTest);
        ArrayOfAudioBufferPointers fixedBlockBuffersB(numBuffersToTest);

       
        for (size_t i = 0; i < numBuffersToTest; i++)
        {
            int blockSizeToAssign = blockSizes[i % blockSizes.size()];
            variableBlockBuffers[i] = std::make_unique<juce::AudioBuffer<float>>(numChannels, blockSizeToAssign);
            fixedBlockBuffersA[i] = std::make_unique<juce::AudioBuffer<float>>(numChannels, blockSizeToAssign);
            fixedBlockBuffersB[i] = std::make_unique<juce::AudioBuffer<float>>(numChannels, blockSizeToAssign);
        }

        bool showEditor = true;
        subBlockProcessingTest(instance, ut, true, variableBlockBuffers, showEditor, false, 1);
        subBlockProcessingTest(*fixedInstanceA, ut,  false, fixedBlockBuffersA,showEditor, false, 1);
        subBlockProcessingTest(*fixedInstanceB, ut, false, fixedBlockBuffersB, showEditor, false, 1);

        for (size_t i = 0; i < numBuffersToTest; i++)
        {
            float fixedBufferDifference = GetFFTDifferenceMeasure(*fixedBlockBuffersA[i], *fixedBlockBuffersB[i], ut);
            
            float variableBufferDifference = GetFFTDifferenceMeasure(*variableBlockBuffers[i], *fixedBlockBuffersA[i], ut);
            
            if(variableBufferDifference > 2.0 * fixedBufferDifference)
                ut.logMessage(String("Warning! High difference in variable buffer size! Difference: ") + String(variableBufferDifference));
        }
    }


    static void WriteWavToFile(juce::File File, double dSampleRate, juce::AudioBuffer<float> Buffer)
    {
        WavAudioFormat Format;
        std::unique_ptr<AudioFormatWriter> Writer;
        Writer.reset(Format.createWriterFor(new FileOutputStream{ File },
                                            dSampleRate,
                                            Buffer.getNumChannels(),
                                            32,
                                            {},
                                            0));
        if (Writer != nullptr)
            Writer->writeFromAudioSampleBuffer(Buffer, 0, Buffer.getNumSamples());
    }

    static float GetBufferDifferenceMeasure(juce::AudioBuffer<float>* cBufferA, juce::AudioBuffer<float>* cBufferB)
    {
        float fTotalDifference = 0.0f;
        for (int iChannel = 0; iChannel < cBufferA->getNumChannels(); iChannel++)
        {
            auto* pBufferA = cBufferA->getReadPointer(iChannel);
            auto* pBufferB = cBufferB->getReadPointer(iChannel);

            for (int iBufferIndex = 0; iBufferIndex < cBufferA->getNumSamples(); iBufferIndex++)
            {
                fTotalDifference += std::abs(pBufferA[iBufferIndex] - pBufferB[iBufferIndex]);
            }
        }
        return fTotalDifference;
    }

    // TODO IMPLEMENT FROM THIS INSTEAD! // TEST
    static float GetFFTDifferenceMeasure(juce::AudioBuffer<float>& BufferToCopyA,
                                         juce::AudioBuffer<float>& BufferToCopyB,
                                         PluginTests& rUnitTest)
    {     
        //jassert(BufferToCopyA->getNumChannels() == BufferToCopyB->getNumChannels()); // Not a fail since this is testing the test...
        // jassert(BufferToCopyA->getNumSamples() == BufferToCopyB->getNumSamples());
        int iNumChannels = BufferToCopyA.getNumChannels();
        int iNumSamples = BufferToCopyA.getNumSamples();
        
        int iFftOrder = 0;
        while(std::powf(2, iFftOrder) < iNumSamples)
            iFftOrder++;
        int iFftSize = (int)std::powf(2, iFftOrder);
        
        // Create an FFT, that can accomidate the buffer.
        juce::dsp::FFT Fft(iFftOrder);

        // Copy buffers. 
        juce::AudioBuffer<float> AudioBufferA {iNumChannels, 2 * iFftSize};
        juce::AudioBuffer<float> AudioBufferB {iNumChannels, 2 * iFftSize};
        AudioBufferA.clear();
        AudioBufferB.clear();

        // PERFORM FFT AND TAKE DIFFERENCE. 
        for (int iChannel = 0; iChannel < iNumChannels; iChannel++)
        {
            AudioBufferA.copyFrom(iChannel, 0, BufferToCopyA.getReadPointer(iChannel), iNumSamples);
            AudioBufferB.copyFrom(iChannel, 0, BufferToCopyB.getReadPointer(iChannel), iNumSamples);
            
            Fft.performFrequencyOnlyForwardTransform(AudioBufferA.getWritePointer(iChannel));
            Fft.performFrequencyOnlyForwardTransform(AudioBufferB.getWritePointer(iChannel));
        }
        return GetBufferDifferenceMeasure(&AudioBufferA, &AudioBufferB);
    }
    
    int nextPowerOfTwo(int x)
    {
        return static_cast<int>(pow(2, ceil(log(x)/log(2))));
    }
};

// 
static CVariableBlockSizeConsistencyTest variableBlockSizeConsistencyTest;
