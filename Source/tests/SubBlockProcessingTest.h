/*
  ==============================================================================

    SubBlockProcessingTest.h
    Created: 22 Oct 2021 12:42:52pm
    Author:  jamie

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

using ArrayOfAudioBufferPointers = std::vector<std::unique_ptr<juce::AudioBuffer<float>>>;

inline void subBlockProcessingTest(
    AudioPluginInstance& instance,
    PluginTests& ut,
    bool isVariableBufferTest,
    ArrayOfAudioBufferPointers& buffers,
    bool showEditor = false,
    bool suppressOutput = false,
    int numBlocksToDoOfEach = 1,
    int drawEditorOffsetValue = 0
    )
{

    ut.logMessage("Started SubBlockProcessingTest ===============================");

    const bool subnormalsAreErrors = ut.getOptions().strictnessLevel > 5;
    const bool isPluginInstrument = instance.getPluginDescription().isInstrument;

    const std::vector<double>& sampleRates = ut.getOptions().sampleRates;
    const std::vector<int>& blockSizes = ut.getOptions().blockSizes;

    auto r = ut.getRandom();

    jassert(sampleRates.size() > 0 && blockSizes.size() > 0);



    std::unique_ptr<ScopedEditorShower> shower;
    if (showEditor)
    {
        shower = std::make_unique<ScopedEditorShower>(instance);
        if (drawEditorOffsetValue != 0)
        {
            auto edtiorBounds = shower->editor->getBounds();
            shower->editor->setBounds(drawEditorOffsetValue,
                drawEditorOffsetValue,
                edtiorBounds.getWidth(),
                edtiorBounds.getHeight());
        }
    }

    ut.logMessage(String("Testing plugin with [TESTTYPE] size sub buffers.")
        .replace("[TESTTYPE]", (isVariableBufferTest) ? "VARIABLE" : "FIXED"));
    size_t bufferToProcessIntoIndex = 0;
    for (auto sr : sampleRates)
    {
        for (auto bs : blockSizes)
        {
            if (!suppressOutput)
            {
                ut.logMessage(String("Testing with sample rate [SR] and block size [BS]")
                    .replace("SR", String(sr, 0), false)
                    .replace("BS", String(bs), false));
            }


            instance.releaseResources();
            instance.prepareToPlay(sr, bs);

            const int numChannelsRequired = jmax(instance.getTotalNumInputChannels(), instance.getTotalNumOutputChannels());
            // AudioBuffer<float> ab(numChannelsRequired, bs);

            auto& ab = *buffers[bufferToProcessIntoIndex++];
            MidiBuffer mb;

            // Add a random note on if the plugin is a synth
            const int noteChannel = r.nextInt({ 1, 17 });
            const int noteNumber = r.nextInt(128);

            if (isPluginInstrument)
                addNoteOn(mb, noteChannel, noteNumber, jmin(10, r.nextInt({ 0, bs })));

            for (int run = 0; run < numBlocksToDoOfEach; run++)
            {
                int numSamplesDone = 0;

                ab.clear();
                for (int ch = 0; ch < numChannelsRequired; ch++)
                    ab.setSample(ch, 0, 1.0f);

                for (;;)
                {
                    // Set random parameter values
                    {
                        auto parameters = getNonBypassAutomatableParameters(instance);

                        // Do all the parameters...
                        for (int i = 0; i < parameters.size(); ++i)
                        {
                            const int paramIndex = r.nextInt(parameters.size());
                            parameters[paramIndex]->setValue(r.nextFloat());
                        }
                    }

                    int subBufferSize = (isVariableBufferTest) ? r.nextInt({ 0, bs }) : 32;

                    const int numSamplesThisTime = jmin(subBufferSize, bs - numSamplesDone);
                    DBG(numSamplesThisTime);
                    AudioBuffer<float> subBuffer(ab.getArrayOfWritePointers(),
                        ab.getNumChannels(),
                        numSamplesDone,
                        numSamplesThisTime);

                    fillNoise(subBuffer);

                    instance.processBlock(subBuffer, mb);

                    numSamplesDone += numSamplesThisTime;
                    mb.clear();
                    if (numSamplesDone >= bs)
                        break;
                }

                juce::String testDetails = suppressOutput ? String(" at Sample rate: " + String(sr) + String(", Blocksize: ") + String(bs)) : "";

                ut.expectEquals(countNaNs(ab), 0, "NaNs found in buffer" + testDetails);
                ut.expectEquals(countInfs(ab), 0, "Infs found in buffer" + testDetails);

                const int subnormals = countSubnormals(ab);

                if (subnormalsAreErrors)
                    ut.expectEquals(countInfs(ab), 0, "Submnormals found in buffer" + testDetails);
                else if (subnormals > 0)
                    ut.logMessage("!!! WARNGING: " + String(countSubnormals(ab)) + " submnormals found in buffer" + testDetails);
            }
        }
    }
}
