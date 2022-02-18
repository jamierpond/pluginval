/*
  ==============================================================================

    ChangingSampleRateOnFlyTest.cpp
    Created: 27 Sep 2021 11:28:18am
    Author:  Jamie

  ==============================================================================
*/

#include "JuceHeader.h" 

#include "../PluginTests.h"
#include "../TestUtilities.h"


//==============================================================================
struct CChangingSampleRateOnFlyTest : public PluginTest
{
    CChangingSampleRateOnFlyTest()
        : PluginTest("Changing Sample Rate on the fly test", 3)
    {

    }

    static void runAudioProcessingTest(PluginTests& ut, AudioPluginInstance& instance,
        bool callReleaseResourcesBeforeSampleRateChange)
    {
        const bool isPluginInstrument = instance.getPluginDescription().isInstrument;

        const std::vector<double>& sampleRates = ut.getOptions().sampleRates;
        const std::vector<int>& blockSizes = ut.getOptions().blockSizes;

        jassert(sampleRates.size() > 0 && blockSizes.size() > 0);
        instance.prepareToPlay(sampleRates[0], blockSizes[0]);

        const int numBlocks = 300;
        auto r = ut.getRandom();

        const int numTests = 50;


        for (int i = 0; i < numTests; i++)
        {
            // Get random blocksize and samplerate. 
            double sr = static_cast<double>(r.nextInt({15000, 192000}));
            int bs = r.nextInt({32, 8192});

            ut.logMessage(String("Testing with sample rate [SR] and block size [BS]")
                .replace("SR", String(sr, 0), false)
                .replace("BS", String(bs), false));

            if (callReleaseResourcesBeforeSampleRateChange)
                instance.releaseResources();

            instance.prepareToPlay(sr, bs);

            const int numChannelsRequired = jmax(instance.getTotalNumInputChannels(), instance.getTotalNumOutputChannels());
            AudioBuffer<float> ab(numChannelsRequired, bs);
            MidiBuffer mb;

            // Add a random note on if the plugin is a synth
            const int noteChannel = r.nextInt({ 1, 17 });
            const int noteNumber = r.nextInt(128);

            if (isPluginInstrument)
                addNoteOn(mb, noteChannel, noteNumber, jmin(10, bs));

            for (int i = 0; i < numBlocks; ++i)
            {
                instance.prepareToPlay(sr, bs);

                // Add note off in last block if plugin is a synth
                if (isPluginInstrument && i == (numBlocks - 1))
                    addNoteOff(mb, noteChannel, noteNumber, 0);

                fillNoise(ab);
                instance.processBlock(ab, mb);
                mb.clear();

                ut.expectEquals(countNaNs(ab), 0, "NaNs found in buffer");
                ut.expectEquals(countInfs(ab), 0, "Infs found in buffer");
                ut.expectEquals(countSubnormals(ab), 0, "Subnormals found in buffer");
            }
        }
    }

    void runTest(PluginTests& ut, AudioPluginInstance& instance) override
    {
        runAudioProcessingTest(ut, instance, true);
    }
};

// This static instance is required to add the test to pluginval.
// static CChangingSampleRateOnFlyTest changeSampleRateOnFlyTest;
