
cmake_minimum_required(VERSION 3.0.0)

find_package(SDL2 REQUIRED)

add_subdirectory(Cpu6502)
add_subdirectory(NESEmu)

add_executable(NESEmu "main.cpp")
target_include_directories(NESEmu PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(NESEmu PRIVATE Cpu6502 NESEmuCore ${SDL2_LIBRARIES})

target_compile_features(NESEmu PRIVATE cxx_std_14)
