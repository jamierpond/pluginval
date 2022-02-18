/*
  ==============================================================================

    CMultipleInstancesTest.cpp
    Created: 17 Sep 2021 1:42:33pm
    Author:  jamie

  ==============================================================================
*/

//#include "JuceHeader.h"
//
//#include "../PluginTests.h"
//#include "../TestUtilities.h"
//
//#include "SubBlockProcessingTest.h"
//
//#include <future>
//
//class InstanceTesterThread : public juce::Threadl
//{
//public:
//    InstanceTesterThread(
//        PluginTests& _ut,
//        AudioPluginInstance& _instance,
//        int indexToSet)
//        :
//        ut(_ut),
//        instance(_instance),
//        index(indexToSet),
//        juce::Thread(String("InstanceTesterThread-") + String(index))
//    {
//        // setPriority(9);
//    }
//
//    void run() override
//    {
//        ut.logMessage(String("Stated running thread") + String(index));
//
//        // TODO, IMPLEMENT LOAD EDITOR.
//
//        subBlockProcessingTest(instance, ut, isVariableBufferTest, false, true, 6);
//    }
//
//
//
//private:
//    int index;
//
//    PluginTests& ut;
//    juce::AudioPluginInstance& instance;
//    bool isVariableBufferTest  = false;
//
//};
////
////
////for (int i = 0; i < numThreads; i++)
////{
////    ut.logMessage("doing thread index" + String(i));
////    auto processThread = std::async(std::launch::async,
////        [&]
////        {
////            while (shouldProcess)
////            {
////                subBlockProcessingTest(instance, ut, false, false);
////                threadStartedEvents[i].signal();
////            }
////        });
////    threadStartedEvents[i].wait();
////}
////// Wait for threads to finish!
////ut.logMessage("Waiting for threads to finish...");
////Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 10000);
////ut.logMessage("Threads finished processing!");
//
////==============================================================================
//struct CMultipleInstancesTest : public PluginTest
//{
//    CMultipleInstancesTest()
//        : PluginTest("MultipleInstancesAutomation", 3, { Requirements::Thread::messageThread, Requirements::GUI::requiresGUI })
//    {
//
//    }
//
//    void runTest(PluginTests& ut, juce::AudioPluginInstance& instance) override
//    {
//        const bool isPluginInstrument = instance.getPluginDescription().isInstrument;
//
//        const std::vector<double>& sampleRates = ut.getOptions().sampleRates;
//        const std::vector<int>& blockSizes = ut.getOptions().blockSizes;
//
//        for (int i = 0; i < numThreads; i++)
//        {
//            instances[i] = ut.testOpenPlugin(instance.getPluginDescription());
//            threads[i] = std::make_unique<InstanceTesterThread>(ut, std::move(*instances[i]), i);
//            threads[i]->startThread(9);
//        }
//
//        for (auto& th : threads)
//        {
//            th->waitForThreadToExit(30000);
//        }
//    }
//
//    const int numThreads = 3;
//    std::vector<std::unique_ptr<InstanceTesterThread>> threads{numThreads};
//
//    std::vector<std::unique_ptr<AudioPluginInstance>> instances{numThreads};
//};
//
//// This static instance is required to add the test to pluginval.
//static CMultipleInstancesTest multipleInstanceAutomationTest;

//
//
//
//
//
////==============================================================================
//struct CMultipleInstancesTest : public PluginTest
//{
//    CMultipleInstancesTest()
//        : PluginTest("MultipleInstancesAutomation", 3, { Requirements::Thread::messageThread, Requirements::GUI::requiresGUI })
//    {
//
//    }
//
//    void runTest(PluginTests& rUnitTest, juce::AudioPluginInstance& instance) override
//    {
//        const bool bSubnormalsAreErrors = rUnitTest.getOptions().strictnessLevel > 5;
//        const bool bIsPluginInstrument = instance.getPluginDescription().isInstrument;
//
//        const std::vector<double>& pSampleRates = rUnitTest.getOptions().sampleRates;
//        const std::vector<int>& pBlockSizes = rUnitTest.getOptions().blockSizes;
//        jassert(pSampleRates.size() > 0 && pBlockSizes.size() > 0);
//
//        auto RandomNumberGenerator = rUnitTest.getRandom();
//
//        const int iNumInstancesToTest = 5; // This has to be a specific integer type to prevent casting further down the line.
//        auto PluginDescription = instance.getPluginDescription();
//        std::vector<std::unique_ptr<AudioPluginInstance>> pPluginInstances{ iNumInstancesToTest };
//        std::vector<std::unique_ptr<ScopedEditorShower>> pEditorShowers{ iNumInstancesToTest };
//        for (unsigned int i = 0; i < iNumInstancesToTest; i++)
//        {
//            pPluginInstances[i] = rUnitTest.testOpenPlugin(PluginDescription); // Creates an plugin instance.
//            pPluginInstances[i]->prepareToPlay(44100, 512);              // Initialize instance.
//            pEditorShowers[i] = std::make_unique<ScopedEditorShower>(*pPluginInstances[i]); // Opens instance's editor window.
//
//            // Position the windows offset, so you can see all the different plugins when they test (to make sure they all load!).
//            int iWindowPosition = i * 50;
//            pEditorShowers[i]->editor->setTopLeftPosition(iWindowPosition, iWindowPosition);
//        }
//
//        for (auto dSampleRate : pSampleRates)
//        {
//            for (auto iBlockSize : pBlockSizes)
//            {
//                const int iSubBlockSize = 32;
//                rUnitTest.logMessage(String("Testing with sample rate [SR] and block size [BS] and sub-block size [SB]")
//                    .replace("SR", String(dSampleRate, 0), false)
//                    .replace("BS", String(iBlockSize), false)
//                    .replace("SB", String(iSubBlockSize), false));
//
//                for (int i = 0; i < iNumInstancesToTest; i++)
//                    pPluginInstances[i]->prepareToPlay(dSampleRate, iBlockSize);
//
//                int iNumSamplesDone = 0;
//                const int iNumChannelsRequired = jmax(pPluginInstances[0]->getTotalNumInputChannels(), pPluginInstances[0]->getTotalNumOutputChannels());
//                AudioBuffer<float> audioBuffer(iNumChannelsRequired, iBlockSize);
//                MidiBuffer MidiBuffer;
//
//                // Add a random note on if the plugin is a synth
//                const int iNoteChannel = RandomNumberGenerator.nextInt({ 1, 17 });
//                const int iNoteNumber = RandomNumberGenerator.nextInt(128);
//                if (bIsPluginInstrument)
//                    addNoteOn(MidiBuffer, iNoteChannel, iNoteNumber, jmin(10, iSubBlockSize));
//
//
//                int numBlocksToDo = 10000;
//                for (int blockIndex = 0; blockIndex < numBlocksToDo; blockIndex++)
//                {
//                    for (;;)
//                    {
//                        // Create a sub-buffer and process
//                        const int iNumSamplesThisTime = jmin(iSubBlockSize, iBlockSize - iNumSamplesDone);
//
//                        // Trigger a note off in the last sub block
//                        if (bIsPluginInstrument && (iBlockSize - iNumSamplesDone) <= iSubBlockSize)
//                            addNoteOff(MidiBuffer, iNoteChannel, iNoteNumber, jmin(10, iSubBlockSize));
//
//                        AudioBuffer<float> SubBuffer(audioBuffer.getArrayOfWritePointers(),
//                                                        audioBuffer.getNumChannels(),
//                                                        iNumSamplesDone,
//                                                        iNumSamplesThisTime);
//
//                        for (int i = 0; i < iNumInstancesToTest; i++)
//                        {
//                            auto pParameters = getNonBypassAutomatableParameters(*pPluginInstances[i]);
//
//                            for (int j = 0; j < jmin(20, pParameters.size()); ++j)
//                            {
//                                const int iParamIndex = RandomNumberGenerator.nextInt(pParameters.size());
//                                pParameters[iParamIndex]->setValue(RandomNumberGenerator.nextFloat());
//                            }
//
//                            // TODO RANDOMLY HIDE AND SHOW THE EDITORS.
//
//                            fillNoise(SubBuffer);
//                            pPluginInstances[i]->processBlock(SubBuffer, MidiBuffer);
//
//                            rUnitTest.expectEquals(countNaNs(SubBuffer), 0, "NaNs found in buffer");
//                            rUnitTest.expectEquals(countInfs(SubBuffer), 0, "Infs found in buffer");
//                            rUnitTest.expectEquals(countSubnormals(SubBuffer), 0, "Subnormals found in buffer");
//                        }
//
//                        iNumSamplesDone += iNumSamplesThisTime;
//
//                        MidiBuffer.clear();
//
//                        if (iNumSamplesDone >= iBlockSize)
//                            break;
//                    }
//                }
//
//                rUnitTest.expectEquals(countNaNs(audioBuffer), 0, "NaNs found in buffer");
//                rUnitTest.expectEquals(countInfs(audioBuffer), 0, "Infs found in buffer");
//
//                const int iNumSubnormals = countSubnormals(audioBuffer);
//
//                if (bSubnormalsAreErrors)
//                    rUnitTest.expectEquals(countInfs(audioBuffer), 0, "Submnormals found in buffer");
//                else if (iNumSubnormals > 0)
//                    rUnitTest.logMessage("!!! WARNGING: " + String(countSubnormals(audioBuffer)) + " submnormals found in buffer");
//            }
//        }
//    }
//};
//
//// This static instance is required to add the test to pluginval.
//static CMultipleInstancesTest multipleInstanceAutomationTest;
//
//
//
//
//
////==============================================================================
//struct SingleInstanceAutomation : public PluginTest
//{
//    SingleInstanceAutomation()
//        : PluginTest("Automation", 3)
//    {
//    }
//
//    void runTest(PluginTests& ut, AudioPluginInstance& instance) override
//    {
//        const bool subnormalsAreErrors = ut.getOptions().strictnessLevel > 5;
//        const bool isPluginInstrument = instance.getPluginDescription().isInstrument;
//
//        const std::vector<double>& sampleRates = ut.getOptions().sampleRates;
//        const std::vector<int>& blockSizes = ut.getOptions().blockSizes;
//
//        const int iNumInstancesToTest = 5;
//        auto PluginDescription = instance.getPluginDescription();
//        std::vector<std::unique_ptr<AudioPluginInstance>> pluginInstances{ iNumInstancesToTest };
//        std::vector<std::unique_ptr<ScopedEditorShower>> editorShowers{ iNumInstancesToTest };
//        for (unsigned int i = 0; i < iNumInstancesToTest; i++)
//        {
//            pluginInstances[i] = std::make_unique<AudioPluginInstance>(ut.testOpenPlugin(PluginDescription); // Creates an plugin instance.
//            pluginInstances[i]->prepareToPlay(44100, 512);              // Initialize instance.
//            editorShowers[i] = std::make_unique<ScopedEditorShower>(*pluginInstances[i]); // Opens instance's editor window.
//
//            // Position the windows offset, so you can see all the different plugins when they test (to make sure they all load!).
//            int iWindowPosition = i * 50;
//            editorShowers[i]->editor->setTopLeftPosition(iWindowPosition, iWindowPosition);
//        }
//
//        jassert(sampleRates.size() > 0 && blockSizes.size() > 0);
//        instance.prepareToPlay(sampleRates[0], blockSizes[0]);
//
//        auto r = ut.getRandom();
//
//        for (auto sr : sampleRates)
//        {
//            for (auto bs : blockSizes)
//            {
//                const int subBlockSize = 32;
//                ut.logMessage(String("Testing with sample rate [SR] and block size [BS] and sub-block size [SB]")
//                    .replace("SR", String(sr, 0), false)
//                    .replace("BS", String(bs), false)
//                    .replace("SB", String(subBlockSize), false));
//
//                instance.releaseResources();
//                instance.prepareToPlay(sr, bs);
//
//                int numSamplesDone = 0;
//                const int numChannelsRequired = jmax(instance.getTotalNumInputChannels(), instance.getTotalNumOutputChannels());
//                AudioBuffer<float> ab(numChannelsRequired, bs);
//                MidiBuffer mb;
//
//                // Add a random note on if the plugin is a synth
//                const int noteChannel = r.nextInt({ 1, 17 });
//                const int noteNumber = r.nextInt(128);
//
//                if (isPluginInstrument)
//                    addNoteOn(mb, noteChannel, noteNumber, jmin(10, subBlockSize));
//
//                for (;;)
//                {
//                    // Set random parameter values
//                    {
//                        auto parameters = getNonBypassAutomatableParameters(instance);
//
//                        for (int i = 0; i < jmin(10, parameters.size()); ++i)
//                        {
//                            const int paramIndex = r.nextInt(parameters.size());
//                            parameters[paramIndex]->setValue(r.nextFloat());
//                        }
//                    }
//
//                    // Create a sub-buffer and process
//                    const int numSamplesThisTime = jmin(subBlockSize, bs - numSamplesDone);
//
//                    // Trigger a note off in the last sub block
//                    if (isPluginInstrument && (bs - numSamplesDone) <= subBlockSize)
//                        addNoteOff(mb, noteChannel, noteNumber, jmin(10, subBlockSize));
//
//                    AudioBuffer<float> subBuffer(ab.getArrayOfWritePointers(),
//                        ab.getNumChannels(),
//                        numSamplesDone,
//                        numSamplesThisTime);
//                    fillNoise(subBuffer);
//                    instance.processBlock(subBuffer, mb);
//                    numSamplesDone += numSamplesThisTime;
//
//                    mb.clear();
//
//                    if (numSamplesDone >= bs)
//                        break;
//                }
//
//                ut.expectEquals(countNaNs(ab), 0, "NaNs found in buffer");
//                ut.expectEquals(countInfs(ab), 0, "Infs found in buffer");
//
//                const int subnormals = countSubnormals(ab);
//
//                if (subnormalsAreErrors)
//                    ut.expectEquals(countInfs(ab), 0, "Submnormals found in buffer");
//                else if (subnormals > 0)
//                    ut.logMessage("!!! WARNGING: " + String(countSubnormals(ab)) + " submnormals found in buffer");
//            }
//        }
//    }
//};
//
//static SingleInstanceAutomation automationTest;
