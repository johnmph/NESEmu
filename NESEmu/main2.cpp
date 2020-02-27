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
#include "NESEmu/Cartridge/Mapper2.hpp"
#include "NESEmu/Cartridge/Mapper3.hpp"
#include "NESEmu/Cartridge/Mapper7.hpp"
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
    //std::ifstream ifs("../UnitTestFiles/SMB.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/DK.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/Castlevania.nes", std::ios::binary);  // Mapper2, 128kb de prg-rom, vertical mirroring chr-ram
    //std::ifstream ifs("../UnitTestFiles/Duck Tales.nes", std::ios::binary);  // Mapper2, 128kb de prg-rom, vertical mirroring chr-ram
    //std::ifstream ifs("../UnitTestFiles/Battletoads.nes", std::ios::binary);  // Mapper7, 256kb de prg-rom, single screen mirroring chr-ram

    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/nestest.nes", std::ios::binary);  // 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/branch_timing_tests/1.Branch_Basics.nes", std::ios::binary);  // 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/instr_misc/instr_misc.nes", std::ios::binary);  // 32kb de prg-rom, horizontal mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/oam_read/oam_read.nes", std::ios::binary);  // 32kb de prg-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_read_buffer/test_ppu_read_buffer.nes", std::ios::binary);  // Mapper3, 16kb de prg-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_sprite_hit/ppu_sprite_hit.nes", std::ios::binary);  // Mapper1, 256kb de prg-rom, vertical mirroring
    
    std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_sprite_hit/rom_singles/09-timing.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    // TestROM/PPU/ppu_sprite_hit/rom_singles/ -> fail : 09 (timing) a cause que l'instruction qui lit le vblank flag (lda) est appelée juste avant que le ppu mette le flag a 1, ensuite la micro instruction ld0 se fait mais le bus a deja été lu juste avant
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_vbl_nmi/rom_singles/06-suppression.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    // TestROM/PPU/ppu_vbl_nmi/rom_singles/ -> fail : 05 (timing), 06 (suppression), 07 (nmi on timing), 08 (nmi off timing), 10 (even odd timing)
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/sprdma_and_dmc_dma/sprdma_and_dmc_dma.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/oam_stress/oam_stress.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // Ne va pas
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/oamtest3/oam3.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, vertical mirroring chr-ram // ???
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_sprite_overflow/rom_singles/03-timing.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // Seul 03-timing foire
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/blargg_ppu_tests_2005.09.15b/vram_access.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring chr-ram  // OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/scanline/scanline.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, vertical mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/instr_timing/rom_singles/1-instr_timing.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // TODO: a voir une fois le APU frame counter terminé
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_exec_space/test_cpu_exec_space_apu.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_interrupts_v2/rom_singles/2-nmi_and_brk.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // TODO: ne passe pas
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_dummy_writes/cpu_dummy_writes_ppumem.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_reset/registers.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/instr_misc/rom_singles/03-dummy_reads.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/instr_test-v5/rom_singles/16-special.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // TODO: ne foire que sur les instructions pas encore supportées
    
    
    // Check that file exists
    assert(ifs.good());
    
    // Mapper for SMB
    NESEmu::Cartridge::Mapper0<32, 0, NESEmu::Cartridge::MirroringType::Vertical> mapper0(ifs);
    
    // Create NES with Mapper
    NESEmu::Nes<NESEmu::Model::Ntsc, decltype(mapper0), GraphicHardware> nes(mapper0, graphicHardware);
    
    // Connect controller
    nes.connectController(0, std::move(controller));
    
    // Power up NES
    nes.powerUp();
    
    // Release reset to start NES
    nes.reset(true);
    
    // Clock loop
    bool resetState = false;
    
    for (;;) {
        if (event.type == SDL_QUIT) {
            break;
        }
        
        if ((event.type == SDL_KEYDOWN) && (event.key.keysym.scancode == SDL_SCANCODE_R) && (resetState == false)) {
            nes.reset(false);
            resetState = true;
            std::cout << "Reset start\n";
        }
        
        if ((event.type == SDL_KEYUP) && (event.key.keysym.scancode == SDL_SCANCODE_R) && (resetState == true)) {
            nes.reset(true);
            resetState = false;
            std::cout << "Reset end\n";
        }
        
        nes.clock();
    }
    
    // Clean up SDL
    SDL_Quit();
    
    return 0;
}

