//
//  main.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <SDL.h>
#include "Nes.hpp"
#include "NESEmu/Cartridge/Mapper0.hpp"
#include "NESEmu/Cartridge/Mapper1.hpp"
#include "NESEmu/Cartridge/Mapper3.hpp"
#include "NESEmu/Controller/Standard.hpp"


struct GraphicHardware {
    
    GraphicHardware(SDL_Event &event) : _event(event) {
        //SDL_Init(SDL_INIT_VIDEO);
        SDL_InitSubSystem(SDL_INIT_VIDEO);
        std::cout << SDL_GetError() << "\n";
        
        _window = SDL_CreateWindow("NESEmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 480, /*SDL_WINDOW_FULLSCREEN | */SDL_WINDOW_OPENGL);
        std::cout << SDL_GetError() << "\n";
        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
        std::cout << SDL_GetError() << "\n";
        _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
        std::cout << SDL_GetError() << "\n";
        
        _palette = (uint32_t *) malloc(sizeof(uint32_t) * 64);
        _pixels = (uint32_t *) malloc(sizeof(uint32_t) * 256 * 240);
        
        // Read palette
        std::ifstream ifs("../UnitTestFiles/ntscpalette.pal", std::ios::binary);
        
        // Check that file exists
        assert(ifs.good());
        
        for (int i = 0; i < 64; ++i) {
            uint8_t rgb[3];
            ifs.read(reinterpret_cast<char *>(&rgb), 3);
            
            _palette[i] = (rgb[0] << 16) | (rgb[1] << 8) | rgb[2];
        }
        
        _fpsLastTime = SDL_GetTicks();
        _fpsFrames = 0;
    }
    
    ~GraphicHardware() {
        free(_pixels);
        free(_palette);
        
        SDL_DestroyTexture(_texture);
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        
        //SDL_Quit();
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }
    
    /*uint32_t getColorFromIndex(uint8_t color, bool r, bool g, bool b) {
        return 0;
    }*/
    
    void plotPixel(unsigned int x, unsigned int y, uint8_t color, bool emphasizeRed, bool emphasizeGreen, bool emphasizeBlue) {
        assert(x < 256);
        assert(y < 240);
        assert(color < 64);
        
        uint32_t paletteColor = _palette[color];
        
        //paletteColor = (((paletteColor & 0xFF0000) << emphasizeRed) & 0xFF0000) | (((paletteColor & 0xFF00) << emphasizeGreen) & 0xFF00) | (((paletteColor & 0xFF) << emphasizeBlue) & 0xFF);//TODO: voir apres
        
        _pixels[(y * 256) + x] = paletteColor;
    }
    
    void notifyVBlankStarted() {
        int texturePitch;
        void *texturePixels;
        
        // Update texture
        SDL_LockTexture(_texture, NULL, &texturePixels, &texturePitch);
        memcpy(texturePixels, _pixels, sizeof(uint32_t) * 256 * 240);
        SDL_UnlockTexture(_texture);
        
        // Update renderer
        SDL_RenderClear(_renderer);
        SDL_RenderCopy(_renderer, _texture, NULL, NULL);
        SDL_RenderPresent(_renderer);
        SDL_PollEvent(&_event);
        
        // Update fps counter
        ++_fpsFrames;
        if (_fpsLastTime < (SDL_GetTicks() - 1000.0)) {
            _fpsLastTime = SDL_GetTicks();
            _fpsCurrent = _fpsFrames;
            _fpsFrames = 0;
            
            std::cout << "FPS = " << _fpsCurrent << "\n";
        }
    }
    
private:
    SDL_Event &_event;
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    
    uint32_t *_palette;
    uint32_t *_pixels;
    
    uint32_t _fpsLastTime;
    uint32_t _fpsCurrent;
    uint32_t _fpsFrames;
};

struct ControllerHardware {
    
    void update() {
        _keyStates = SDL_GetKeyboardState(NULL);
    }
    
    bool getButtonA() const {
        return (_keyStates[SDL_SCANCODE_C] != 0);
    }
    
    bool getButtonB() const {
        return (_keyStates[SDL_SCANCODE_V] != 0);
    }
    
    bool getButtonSelect() const {
        return (_keyStates[SDL_SCANCODE_SPACE] != 0);
    }
    
    bool getButtonStart() const {
        return (_keyStates[SDL_SCANCODE_RETURN] != 0);
    }
    
    bool getDirectionalUp() const {
        return (_keyStates[SDL_SCANCODE_UP] != 0);
    }
    
    bool getDirectionalDown() const {
        return (_keyStates[SDL_SCANCODE_DOWN] != 0);
    }
    
    bool getDirectionalLeft() const {
        return (_keyStates[SDL_SCANCODE_LEFT] != 0);
    }
    
    bool getDirectionalRight() const {
        return (_keyStates[SDL_SCANCODE_RIGHT] != 0);
    }
    
private:
    uint8_t const *_keyStates;
};


int main(int argc, const char * argv[]) {
    // Init SDL
    SDL_Init(SDL_INIT_EVENTS);
    
    SDL_Event event;
    
    GraphicHardware graphicHardware(event);
    ControllerHardware controllerHardware;
    auto controller = std::make_unique<NESEmu::Controller::Standard<ControllerHardware>>(controllerHardware);
    
    // Open ROM
    //std::ifstream ifs("../UnitTestFiles/SMB.nes", std::ios::binary);  // 32kb de prg-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/DK.nes", std::ios::binary);  // 16kb de prg-rom, horizontal mirroring

    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/nestest.nes", std::ios::binary);  // 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/branch_timing_tests/1.Branch_Basics.nes", std::ios::binary);  // 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/instr_misc/instr_misc.nes", std::ios::binary);  // 32kb de prg-rom, horizontal mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/oam_read/oam_read.nes", std::ios::binary);  // 32kb de prg-rom, vertical mirroring
    std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_read_buffer/test_ppu_read_buffer.nes", std::ios::binary);  // Mapper3, 16kb de prg-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_sprite_hit/ppu_sprite_hit.nes", std::ios::binary);  // Mapper1, 256kb de prg-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_sprite_hit/rom_singles/04-flip.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    
    // Check that file exists
    assert(ifs.good());
    
    // Mapper for SMB
    NESEmu::Cartridge::Mapper3<16, 0, NESEmu::Cartridge::MirroringType::Vertical> mapper0(ifs);
    
    // Create NES with Mapper
    NESEmu::Nes<NESEmu::Model::Ntsc, decltype(mapper0), GraphicHardware> nes(mapper0, graphicHardware);
    
    // Connect controller
    nes.connectController(0, std::move(controller));
    
    // Power up NES
    nes.powerUp();
    
    // Release reset to start NES
    nes.reset(true);
    
    // Clock loop
    for (;;) {
        if (event.type == SDL_QUIT) {
            break;
        }
        
        nes.clock();
    }
    
    // Clean up SDL
    SDL_Quit();
    
    return 0;
}

