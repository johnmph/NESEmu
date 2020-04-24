# NESEmu
Portable NES Emulator, cycle accurate for CPU, PPU and APU.

NESEmu is a NES emulator written in C++ with these goals in mind (and in order) :

1) Try to be the most accurate possible :

- It is based on the essential informations provided in http://nesdev.com wiki and forum.
- To cover most of the obscure details, tests was made on the excellent visual tools :

-- For the general 6502 CPU : http://visual6502.org/JSSim/index.html

-- For the specific 2A03 CPU / APU : http://www.qmtpro.com/~nes/chipimages/visual2a03/

-- For the 2C02 PPU : https://www.qmtpro.com/~nes/chipimages/visual2c02/
- It runs almost all the tests ROM provided by the community without having some hack to pass them

2) Try to be the most portable possible :

- The emulation is fully decoupled from interfacing with real hardware :

-- You provide a GraphicManager which will receive pixel coordinates and color to plot and a notification when VBlank starts to allow you to blit buffer

-- You provide a SoundManager which will receive sample value at time to fill your audio buffer

-- You provide some ControllerManager which will be polled when needed to receive your inputs

-- You provide either a lambda, a function, a method, a functor to run the NES or you can control yourself the run loop
- The code is written in standard C++14
- It doesn't depend on any library (except SDL2 but only for the main.cpp example, see below)
- The code is provided with a small main.cpp example which use SDL2 to interface with OS, it compile and run on OSX, Windows and Linux without any code modification

3) Try to be the most well designed possible :
- The code is written to be the most readable possible (explicit variables / functions / classes / members / methods name)
- The 6502 CPU code can be used for any other project, it is decoupled from 2A03 CPU
- The internals of the NES are coupled like the original NES hardware :

-- 2A03 is 6502 + DMA + APU, it communicate with the PPU / memory by the NES bus

-- 2C02 is decoupled and communicate with the CPU / memory by the NES bus

-- NES is 2A03 + 2C02 + cartridge interface + controller ports + communication bus

-- Mappers are decoupled from 2A03 / 2C02, they communicates only by NES bus, it means that there is no mapper code outside mapper (even for scanline detection)

3) Try to be with the less bug possible :

- It has unit test cases to cover the majority of the CPU, PPU and DMA code
- It uses the modern C++ tools to reduce errors like memory leak, double free or pointer dangling (uses smart pointer)
