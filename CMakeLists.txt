cmake_minimum_required(VERSION 3.22)
project(ArmNongmuangLA2A VERSION 1.0.0)
include(FetchContent)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.6 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)
juce_add_plugin(ArmNongmuangLA2A
  COMPANY_NAME "Arm Nongmuang"
  BUNDLE_ID "studio.mueperm.armnongmuang.la2a"
  PLUGIN_MANUFACTURER_CODE Anmg PLUGIN_CODE La2A
  FORMATS VST3 Standalone PRODUCT_NAME "Arm Nongmuang LA-2A"
  IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE IS_MIDI_EFFECT FALSE)
target_sources(ArmNongmuangLA2A PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_features(ArmNongmuangLA2A PRIVATE cxx_std_17)
target_compile_definitions(ArmNongmuangLA2A PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(ArmNongmuangLA2A PRIVATE juce::juce_audio_utils juce::juce_dsp
  PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags)
