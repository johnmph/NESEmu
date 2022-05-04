cmake_minimum_required(VERSION 3.0.0)

add_library(NESEmuCore OBJECT "Apu/EnvelopeUnit.cpp" "Apu/LengthCounter.cpp" "Apu/NoiseChannel.cpp" "Apu/PulseChannel.cpp" "Apu/SweepUnit.cpp" "Apu/TriangleChannel.cpp" "Cartridge/Loader/ArchaicINes.cpp" "Cartridge/Loader/INes.cpp" "Cartridge/Loader/INes2.cpp" "Cartridge/Common.cpp" "Controller/Nothing.cpp" "Common.cpp")
