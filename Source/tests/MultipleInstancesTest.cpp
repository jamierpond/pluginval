/*
  ==============================================================================

    MultipleInstancesTest.cpp
    Created: 17 Sep 2021 1:42:33pm
    Author:  jamie

  ==============================================================================
*/

#include "JuceHeader.h" 

#include "../PluginTests.h"
#include "../TestUtilities.h"


//==============================================================================
struct CMultipleInstanceAutomationTest : public PluginTest
{
    CMultipleInstanceAutomationTest()
        : PluginTest("MultipleInstancesAutomation", 3)
    {
    }

    void runTest(PluginTests& ut, AudioPluginInstance& instance) override
    {
        const bool subnormalsAreErrors = ut.getOptions().strictnessLevel > 5;
        const bool isPluginInstrument = instance.getPluginDescription().isInstrument;

        const std::vector<double>& sampleRates = ut.getOptions().sampleRates;
        const std::vector<int>& blockSizes = ut.getOptions().blockSizes;

        jassert(sampleRates.size() > 0 && blockSizes.size() > 0);
        instance.prepareToPlay(sampleRates[0], blockSizes[0]);

        auto r = ut.getRandom();

        const unsigned __int64 numInstancesToTest = 10;

        auto pd = instance.getPluginDescription();

        std::vector<std::unique_ptr<AudioPluginInstance>> instances{ numInstancesToTest };
        std::vector<std::unique_ptr<ScopedEditorShower>> editorShowers{ numInstancesToTest };
        for (int i = 0; i < numInstancesToTest; i++)
        {
            instances[i] = ut.testOpenPlugin(pd);
            editorShowers[i] = std::make_unique<ScopedEditorShower>(*instances[i]);
        }
            


        for (int i = 0; i < numInstancesToTest; i++)
        {
            instances[i]->releaseResources();
            instances[i]->prepareToPlay(44100, 512);
            
        }

        for (auto sr : sampleRates)
        {
            for (auto bs : blockSizes)
            {
                const int subBlockSize = 32;
                ut.logMessage(String("Testing with sample rate [SR] and block size [BS] and sub-block size [SB]")
                    .replace("SR", String(sr, 0), false)
                    .replace("BS", String(bs), false)
                    .replace("SB", String(subBlockSize), false));

                for (int i = 0; i < numInstancesToTest; i++)
                {
                    instances[i]->releaseResources();
                    instances[i]->prepareToPlay(sr, bs);
                }
                

                int numSamplesDone = 0;
                const int numChannelsRequired = jmax(instances[0]->getTotalNumInputChannels(), instances[0]->getTotalNumOutputChannels());
                AudioBuffer<float> ab(numChannelsRequired, bs);
                MidiBuffer mb;

                // Add a random note on if the plugin is a synth
                const int noteChannel = r.nextInt({ 1, 17 });
                const int noteNumber = r.nextInt(128);

                if (isPluginInstrument)
                    addNoteOn(mb, noteChannel, noteNumber, jmin(10, subBlockSize));

                for (;;)
                {
                    for (int i = 0; i < numInstancesToTest; i++)
                    {
                        auto parameters = getNonBypassAutomatableParameters(*instances[i]);

                        for (int j = 0; j < jmin(10, parameters.size()); ++j)
                        {
                            const int paramIndex = r.nextInt(parameters.size());
                            parameters[paramIndex]->setValue(r.nextFloat());
                        }
                    }

                    // Create a sub-buffer and process
                    const int numSamplesThisTime = jmin(subBlockSize, bs - numSamplesDone);

                    // Trigger a note off in the last sub block
                    if (isPluginInstrument && (bs - numSamplesDone) <= subBlockSize)
                        addNoteOff(mb, noteChannel, noteNumber, jmin(10, subBlockSize));

                    AudioBuffer<float> subBuffer(ab.getArrayOfWritePointers(),
                        ab.getNumChannels(),
                        numSamplesDone,
                        numSamplesThisTime);

                    for (int i = 0; i < numInstancesToTest; i++)
                    {
                        fillNoise(subBuffer);
                        instances[i]->processBlock(subBuffer, mb);
                    }

                    numSamplesDone += numSamplesThisTime;

                    mb.clear();

                    if (numSamplesDone >= bs)
                        break;
                }

                ut.expectEquals(countNaNs(ab), 0, "NaNs found in buffer");
                ut.expectEquals(countInfs(ab), 0, "Infs found in buffer");

                const int subnormals = countSubnormals(ab);

                if (subnormalsAreErrors)
                    ut.expectEquals(countInfs(ab), 0, "Submnormals found in buffer");
                else if (subnormals > 0)
                    ut.logMessage("!!! WARNGING: " + String(countSubnormals(ab)) + " submnormals found in buffer");
            }
        }
    }
};

static CMultipleInstanceAutomationTest multipleInstanceAutomationTest;