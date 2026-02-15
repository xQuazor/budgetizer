cmake_minimum_required(VERSION 3.22)

project(MyPlugin VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(
        /Users/dovydas/CLionProjects/JUCE
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
        Source/dsp/Oscillator.cpp
)

target_link_libraries(MyPlugin PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_devices
)