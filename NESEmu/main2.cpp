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


struct GraphicHardware {
    
    GraphicHardware() {
        SDL_Init(SDL_INIT_VIDEO/* | SDL_INIT_GAMECONTROLLER*/);
        
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
        /*
        uint8_t NESPaletteDefaut[256][3] = {
            {127,127,127}, {0,0,255}, {0,0,191}, {71,43,191},
            {151,0,135}, {171,0,35}, {171,19,0}, {139,23,0},
            {83,48,0}, {0,120,0}, {0,107,0}, {0,91,0},
            {0,67,88}, {0,0,0}, {0,0,0}, {0,0,0},
            {191,191,191}, {0,120,248}, {0,88,248}, {107,71,255},
            {219,0,205}, {231,0,91}, {248,56,0}, {231,95,19},
            {175,127,0}, {0,184,0}, {0,171,0}, {0,171,71},
            {0,139,139}, {0,0,0}, {0,0,0}, {0,0,0},
            {248,248,248}, {63,191,255}, {107,136,255}, {152,120,248},
            {248,120,248}, {248,88,152}, {248,120,88}, {255,163,71},
            {248,184,0}, {184,248,24}, {91,219,87}, {88,248,152},
            {0,235,219}, {120,120,120}, {0,0,0}, {0,0,0},
            {255,255,255}, {167,231,255}, {184,184,248}, {216,184,248},
            {248,184,248}, {251,167,195}, {240,208,176}, {255,227,171},
            {251,219,123}, {216,248,120}, {184,248,184}, {184,248,216},
            {0,255,255}, {248,216,248}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
            {104,104,104}, {0,64,128}, {0,0,128}, {64,0,128},
            {128,0,128}, {128,0,64}, {128,0,0}, {85,0,0},
            {64,64,0}, {51,80,0}, {0,80,0}, {0,80,0},
            {0,64,64}, {0,0,0}, {0,0,0}, {0,0,0},
            {152,152,152}, {0,128,192}, {64,64,192}, {128,0,192},
            {192,0,192}, {192,0,128}, {192,32,32}, {192,64,0},
            {128,128,0}, {85,128,0}, {0,128,0}, {51,128,51},
            {0,128,128}, {0,0,0}, {0,0,0}, {0,0,0},
            {208,208,208}, {64,192,255}, {128,128,255}, {192,64,255},
            {255,0,255}, {255,64,192}, {255,80,80}, {255,128,64},
            {192,192,0}, {128,192,0}, {0,192,0}, {85,192,85},
            {0,192,192}, {51,51,51}, {0,0,0}, {0,0,0},
            {255,255,255}, {128,255,255}, {192,192,255}, {255,128,255},
            {255,64,255}, {255,128,255}, {255,128,128}, {255,192,128},
            {255,255,64}, {192,255,64}, {64,255,64}, {170,255,170},
            {64,255,255}, {153,153,153}, {0,0,0}, {0,0,0},
            {152,152,152}, {24,88,248}, {24,56,248}, {88,56,248},
            {248,24,248}, {248,24,88}, {248,24,0}, {216,24,0},
            {184,88,0}, {0,120,0}, {0,152,0}, {0,120,24},
            {0,120,184}, {0,0,0}, {24,24,24}, {24,24,24},
            {216,216,216}, {0,120,248}, {0,88,248}, {88,88,248},
            {248,56,248}, {248,56,88}, {248,56,0}, {248,88,0},
            {96,176,0}, {0,184,0}, {0,216,0}, {24,216,88},
            {0,184,248}, {24,24,24}, {24,24,24}, {24,24,24},
            {248,248,248}, {0,152,248}, {120,120,248}, {152,120,248},
            {248,120,248}, {248,88,152}, {248,120,88}, {248,184,0},
            {248,184,0}, {184,248,24}, {120,248,56}, {88,248,152},
            {56,216,248}, {120,120,120}, {24,24,24}, {24,24,24},
            {248,248,248}, {120,184,248}, {184,184,248}, {216,184,248},
            {248,184,248}, {248,152,184}, {248,184,120}, {248,184,56},
            {248,184,88}, {216,248,120}, {184,248,184}, {184,248,216},
            {152,216,248}, {248,216,248}, {24,24,24}, {0,0,0}};
        
        for (int i = 0; i < 64; ++i) {
            _palette[i] = (NESPaletteDefaut[i][0] << 16) | (NESPaletteDefaut[i][1] << 8) | NESPaletteDefaut[i][2];
        }*/
        
        _fpsLastTime = SDL_GetTicks();
        _fpsFrames = 0;
    }
    
    ~GraphicHardware() {
        free(_pixels);
        free(_palette);
        
        SDL_DestroyTexture(_texture);
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        
        SDL_Quit();
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
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    
    uint32_t *_palette;
    uint32_t *_pixels;
    
    uint32_t _fpsLastTime;
    uint32_t _fpsCurrent;
    uint32_t _fpsFrames;
};


int main(int argc, const char * argv[]) {
    GraphicHardware graphicHardware;
    
    // Open ROM
    std::ifstream ifs("../UnitTestFiles/SMB.nes", std::ios::binary);
    
    // Check that file exists
    assert(ifs.good());
    
    // Mapper for SMB
    NESEmu::Cartridge::Mapper0<32, 0, NESEmu::Cartridge::MirroringType::Vertical> mapper0(ifs);
    
    // Create NES with Mapper
    NESEmu::Nes<NESEmu::Model::Ntsc, NESEmu::Cartridge::Mapper0<32, 0, NESEmu::Cartridge::MirroringType::Vertical>, GraphicHardware> nes(mapper0, graphicHardware);
    
    // Power up NES
    nes.powerUp();
    
    // Release reset to start NES
    nes.reset(true);
    
    // Clock loop
    for (;;) {  // TODO: rajouter une condition pour quitter
        nes.clock();
    }
    
    return 0;
}

