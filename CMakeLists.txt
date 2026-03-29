cmake_minimum_required(VERSION 3.22)

project(MyPlugin VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#  MacMini
#  /Users/dovis/Documents/JUCE

#  Macbook Air
#  /Users/dovydas/CLionProjects/JUCE

add_subdirectory(
        /Users/dovis/Documents/JUCE
        ${CMAKE_BINARY_DIR}/JUCE
)

juce_add_plugin(MyPlugin
        COMPANY_NAME "YourName"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE Manu
        PLUGIN_CODE Plug
        FORMATS VST3 Standalone
        PRODUCT_NAME "MyPlugin"
)

target_sources(MyPlugin PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/ParameterBridge.cpp
        Source/dsp/Oscillator.cpp
        Source/modulation/NoiseLFO.cpp
        Source/modulation/TriangleLFO.cpp
        Source/modulation/ComplexLFO.cpp
        Source/lowpass/lowpass.cpp
        Source/bitcrusher/BitCrusher.cpp
        Source/chorus/chorus.cpp
        Source/pitch/PitchModulator.cpp
        Source/player/AudioFilePlayer.cpp
        Source/bitcrusher/BitCrusher.cpp
        Source/chorus/chorus.cpp
        Source/radio/NoiseGenerator.cpp
        Source/radio/RadioTuner.cpp
        Source/radio/MechanicalDrift.cpp
        Source/radio/SweepFilter.cpp
        Source/radio/StationBurstGenerator.cpp
        Source/radio/BandLimiter.cpp
)

# --- React UI BinaryData (built from ui/dist/) ---
# Run: cd ui && npm run build  (before building the C++ project in release)
file(GLOB_RECURSE UI_DIST_FILES "${CMAKE_SOURCE_DIR}/ui/dist/*")

if(UI_DIST_FILES)
    juce_add_binary_data(PluginUIData SOURCES ${UI_DIST_FILES})
    target_link_libraries(MyPlugin PRIVATE PluginUIData)
endif()

target_link_libraries(MyPlugin PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_devices
        juce::juce_dsp
        juce::juce_gui_basics
)
