//
//  main.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>//TODO: pour le filtre audio
#include <cmath>//TODO: pour le filtre audio
#include <SDL.h>
#include "NESEmu/Cartridge/Loader/INes.hpp"
#include "NESEmu/Cartridge/Factory.hpp"
#include "NESEmu/Nes.hpp"
#include "NESEmu/Controller/Standard.hpp"


template <class TSync>
struct GraphicHardware {
    
    GraphicHardware(SDL_Event &event, TSync &sync) : _event(event), _sync(sync) {
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
        
        _lastTime = SDL_GetPerformanceCounter();
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
        
        // Check to wait for locked fps
        for (;;) {
            float elapsedMs = (SDL_GetPerformanceCounter() - _lockedFps) / static_cast<float>(SDL_GetPerformanceFrequency()) * 1000.0f;
            if (elapsedMs >= 16.667f) {
                break;
            }
        }
        _lockedFps = SDL_GetPerformanceCounter();
        
        // TODO: ce n'est nécessaire que pour synchroniser +- le son quand le frame rate est different de 60fps
        //TODO: surement mettre ca et le frame lock dans une autre classe qui gere le timing (TimingHardware ?)
        Uint64 currentTime = SDL_GetPerformanceCounter();
        //29780.5 = cycles CPU par frame
        _sync.setSamplerFrequency(29780.5f / ((currentTime - _lastTime) / static_cast<float>(SDL_GetPerformanceFrequency())));
        _lastTime = currentTime;
        
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
    
    Uint64 _lockedFps;
    TSync &_sync;
    Uint64 _lastTime;
};

struct SoundHardware {
    
    SoundHardware(unsigned int frequency, std::size_t bufferSize) : _counter(0), _currentBufferWriteIndex(0), _currentBufferReadIndex(0) {
        // TODO: bufferSize doit etre une puissance de 2
        
        // Set buffer size as double of audio hardware buffer size
        _buffer.resize(bufferSize * 2);
        
        // Initialize audio
        SDL_InitSubSystem(SDL_INIT_AUDIO);
        std::cout << SDL_GetError() << "\n";
        
        // Set audio spec
        SDL_memset(&_audioSpec, 0, sizeof(_audioSpec));
        
        _audioSpec.freq = frequency;
        _audioSpec.format = /*AUDIO_U8;*/AUDIO_F32SYS;
        _audioSpec.channels = 1;
        _audioSpec.samples = bufferSize;
        _audioSpec.callback = [] (void *param, Uint8 *stream, int len) {
            static_cast<SoundHardware *>(param)->fillBuffer(stream, len);
        };
        _audioSpec.userdata = this;
        
        // Open audio device
        _audioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &_audioSpec, &_audioSpec, /*SDL_AUDIO_ALLOW_FREQUENCY_CHANGE*/0);
        
        // Unpause audio device
        SDL_PauseAudioDevice(_audioDeviceID, SDL_FALSE);
    }
    
    ~SoundHardware() {
        // Close audio device
        SDL_CloseAudioDevice(_audioDeviceID);
        
        // Uninitialize audio
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
    
    void setSamplerFrequency(unsigned int frequency) {
        // Calculate frequency ratio
        _frequencyRatio = static_cast<float>(frequency) / _audioSpec.freq;
    }
    
    bool askForAddingSample() {
        // Increment counter
        ++_counter;
        
        // Check if counter reached frequency ratio
        if (_counter < _frequencyRatio) {
            return false;
        }
        
        // Update counter
        _counter -= _frequencyRatio;
        
        return true;
    }
    
    void addSample(float value) {
        // If write too fast for audio hardware reading, wait before adding more sample
        if (_currentBufferWriteIndex == _currentBufferReadIndex) {
            return;
        }
        
        // Lock audio device
        SDL_LockAudioDevice(_audioDeviceID);
        
        // Avoir la possibilité de filtrer la value ? (style de shader audio) :
        /*std::size_t previousBufferIndex = ((_currentBufferIndex > 0) ? _currentBufferIndex : _buffer.size()) - 1;
        float absDiff = std::fabs(value - (_buffer[previousBufferIndex] + 0.5f));
        if (absDiff > (_buffer[previousBufferIndex] * 2)) {
            value /= 2.0f;
        }
        */
        /*
        std::size_t previousBufferIndex = ((_currentBufferIndex > 0) ? _currentBufferIndex : _buffer.size()) - 1;
        float absDiff = std::fabs(value - (_buffer[previousBufferIndex] + 0.5f));
        
        if (absDiff > 0.75f) {
            value /= 4.0f;
        } else if (absDiff > 0.66f) {
            value /= 3.0f;
        } else if (absDiff > 0.5f) {
            value /= 2.0f;
        }
        //_buffer[previousBufferIndex] = value - 0.5f;
        */
        
        // Add sample
        _buffer[_currentBufferWriteIndex] = value - 0.5f;// TODO: aussi essayer avec uint8_t !!!
        
        // Update write index
        ++_currentBufferWriteIndex;
        
        if (_currentBufferWriteIndex >= _buffer.size()) {
            _currentBufferWriteIndex = 0;
        }
        
        // Unlock audio device
        SDL_UnlockAudioDevice(_audioDeviceID);
    }
    
private:
    
    void fillBuffer(Uint8 *stream, int len) {
        // Get size
        std::size_t size = len;
        
        // Get sample size
        std::size_t sampleSize = len / sizeof(float);
        
        // Save read index
        std::size_t readIndex = _currentBufferReadIndex;
        
        // Update read index
        _currentBufferReadIndex += sampleSize;
        
        // Check if copy will cross boundary of buffer
        if (_currentBufferReadIndex > _buffer.size()) {
            // Copy until boundary
            size = (_buffer.size() - readIndex) * sizeof(float);
            memcpy(&stream[size], reinterpret_cast<Uint8 *>(_buffer.data()), len - size);
            
            // Correct read index
            _currentBufferReadIndex -= _buffer.size();
        }
        
        // Copy buffer
        memcpy(stream, reinterpret_cast<Uint8 *>(&(_buffer.data()[readIndex])), size);
    }
    
    
    SDL_AudioSpec _audioSpec;
    SDL_AudioDeviceID _audioDeviceID;
    std::vector<float> _buffer;
    std::size_t _currentBufferWriteIndex;
    std::size_t _currentBufferReadIndex;
    float _counter;
    float _frequencyRatio;
};

struct ControllerHardware {
    
    void update() {
        _keyStates = SDL_GetKeyboardState(NULL);
    }
    
    bool getButtonA() const {
        return (_keyStates[SDL_SCANCODE_V] != 0);
    }
    
    bool getButtonB() const {
        return (_keyStates[SDL_SCANCODE_C] != 0);
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


struct LoopManager {
    
    LoopManager(SDL_Event &event) : _event(event), _resetState(false) {
    }
    
    template <class TNes>
    bool operator()(TNes &nes) {
        return needToStop(nes);
    }
    
    template <class TNes>
    bool needToStop(TNes &nes) {
        --_x;
        if ((_event.type == SDL_QUIT) || (_x == 0)) {
            return true;
        }
        
        if ((_event.type == SDL_KEYDOWN) && (_event.key.keysym.scancode == SDL_SCANCODE_R) && (_resetState == false)) {
            nes.reset(false);
            _resetState = true;
            std::cout << "Reset start\n";
        }
        
        if ((_event.type == SDL_KEYUP) && (_event.key.keysym.scancode == SDL_SCANCODE_R) && (_resetState == true)) {
            nes.reset(true);
            _resetState = false;
            std::cout << "Reset end\n";
        }
        
        return false;
    }
    
private:
    unsigned int _x = 35795460;
    SDL_Event &_event;
    bool _resetState;
};

template <class TNes>
bool nesLoopNeedToStop(TNes &nes) {
    static unsigned int x = 35795460;
    
    --x;
    if (x == 0) {
        return true;
    }
    
    return false;
}

template <class TNes>
void runNes(TNes &nes, SDL_Event &event) {
    unsigned int _x = 35795460;
    bool resetState = false;
    
    for (;;) {
        nes.clockFull();
        //nes.clock();
        
        --_x;
        if ((event.type == SDL_QUIT)/* || (_x == 0)*/) {
            return;
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
    }
}

int main(int argc, const char * argv[]) {
    // Init SDL
    SDL_Init(SDL_INIT_EVENTS);
    
    SDL_Event event;
    
    SoundHardware soundHardware(44100, 2048);
    GraphicHardware<SoundHardware> graphicHardware(event, soundHardware);
    ControllerHardware controllerHardware;
    auto controller = std::make_unique<NESEmu::Controller::Standard<ControllerHardware>>(controllerHardware);
    
    // Open ROM
    //std::ifstream ifs("../UnitTestFiles/SMB.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/Spelunker.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/Ms. Pac-Man (Tengen).nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/DK.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/Castlevania.nes", std::ios::binary);  // Mapper2, 128kb de prg-rom, vertical mirroring chr-ram
    //std::ifstream ifs("../UnitTestFiles/Duck Tales.nes", std::ios::binary);  // Mapper2, 128kb de prg-rom, vertical mirroring chr-ram
    //std::ifstream ifs("../UnitTestFiles/Battletoads.nes", std::ios::binary);  // Mapper7, 256kb de prg-rom, single screen mirroring chr-ram
    //std::ifstream ifs("../UnitTestFiles/Paperboy.nes", std::ios::binary);  // Mapper3, 32kb de prg-rom, 32kb de chr-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/Huge Insect.nes", std::ios::binary);  // Mapper3, 32kb de prg-rom, 32kb de chr-rom, vertical mirroring
    std::ifstream ifs("../UnitTestFiles/SMB3.nes", std::ios::binary);  // Mapper4, 256kb de prg-rom, 128kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/SMB2.nes", std::ios::binary);  // Mapper4, 128kb de prg-rom, 128kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/Young Indiana Jones Chronicles.nes", std::ios::binary);  // Mapper4, 128kb de prg-rom, 128kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/Adventures of Lolo 2.nes", std::ios::binary);  // Mapper4, 32kb de prg-rom, 32kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/Crystalis.nes", std::ios::binary);  // Mapper4, 256kb de prg-rom, 128kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/Silver Surfer.nes", std::ios::binary);  // Mapper4, 128kb de prg-rom, 256kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/Metroid.nes", std::ios::binary);  // Mapper1, 128kb de prg-rom
    //std::ifstream ifs("../UnitTestFiles/Final Fantasy.nes", std::ios::binary);  // Mapper1, 256kb de prg-rom
    //std::ifstream ifs("../UnitTestFiles/Zelda.nes", std::ios::binary);  // Mapper1, 128kb de prg-rom
    //std::ifstream ifs("../UnitTestFiles/Zelda 2.nes", std::ios::binary);  // Mapper1, 128kb de prg-rom, 128 de chr-rom // TODO: bug mais peut etre parce que le mapper1 n'est pas complet (selon les versions du mapper !) deja c du chr-ram et ici c du chr-rom : oui en plus il y a 8ko dans le mapper1 et ici 128 !!! : OK
    //std::ifstream ifs("../UnitTestFiles/Simpsons - Bart Vs the Space Mutants.nes", std::ios::binary);  // Mapper1, 128kb de prg-rom, 128 de chr-rom, IL Y A LE BUG DU SCOREBAR QUI SHAKE UN PEU, a voir (timing ppu ?)
    //std::ifstream ifs("../UnitTestFiles/Bill & Ted's Excellent Video Game Adventure.nes", std::ios::binary);  // Mapper1, 128kb de prg-rom, 128 de chr-rom // TODO : ne fonctionne pas car on doit gerer la double ecriture dans le MMC1 : ok
    
    //std::ifstream ifs("../UnitTestFiles/1942.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Batman - The Video Game.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Dragon Quest.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Duck Hunt.nes", std::ios::binary);//TODO: regarder pour le zapper controller
    //std::ifstream ifs("../UnitTestFiles/Ghosts'n Goblins.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Kid Icarus.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Mega Man.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/R.C. Pro-Am.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Teenage Mutant Ninja Turtles.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Wild Gunman.nes", std::ios::binary);

    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/nestest.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/branch_timing_tests/1.Branch_Basics.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/branch_timing_tests/2.Backward_Branch.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/branch_timing_tests/3.Forward_Branch.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/instr_misc/instr_misc.nes", std::ios::binary);  // Mapper1, 64kb de prg-rom, horizontal mirroring TODO: ne passe pas le 4eme car besoin de l'APU : ne va tjs pas pour le 4eme avec le frame counter implementé !!! -> peut etre parce qu'il teste des unofficial opcode que je n'ai pas encore implementé ?
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cputime.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, Horizontal mirroring // ?
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/instr_timing/rom_singles/1-instr_timing.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // TODO: a voir une fois le APU length counter terminé : ok sauf pour les unofficial opcodes non implementé
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_exec_space/test_cpu_exec_space_apu.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring TODO: ok meme sans l'APU ?!
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_interrupts_v2/cpu_interrupts.nes", std::ios::binary);  // Mapper1, 80kb de prg-rom // TODO: ne passe pas normal, il faut implementer l'APU : ok mais voir car debut de l'apu frame counter
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_dummy_writes/cpu_dummy_writes_ppumem.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_reset/registers.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/instr_test-v5/official_only.nes", std::ios::binary);  // Mapper1, 256kb de prg-rom, vertical mirroring // TODO: ne foire que sur les instructions pas encore supportées
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/oam_read/oam_read.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_read_buffer/test_ppu_read_buffer.nes", std::ios::binary);  // Mapper3, 16kb de prg-rom, 32 de chr-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_sprite_hit/ppu_sprite_hit.nes", std::ios::binary);  // Mapper1, 256kb de prg-rom, vertical mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_vbl_nmi/ppu_vbl_nmi.nes", std::ios::binary);  // Mapper1, 256kb de prg-rom, vertical mirroring OK !!!
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_sprite_overflow/ppu_sprite_overflow.nes", std::ios::binary);// Mapper1, 128kb de prg-rom, vertical mirroring OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_open_bus/ppu_open_bus.nes", std::ios::binary);// Mapper0, 32kb de prg-rom, vertical mirroring TODO: il faut implementer le decay de l'open bus pour que ca fonctionne
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/nmi_sync/demo_ntsc.nes", std::ios::binary);// Mapper0, 32kb de prg-rom, horizontal mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/sprdma_and_dmc_dma/sprdma_and_dmc_dma.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/oam_stress/oam_stress.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // Ok, juste long a executer, attendre bien la fin
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/oamtest3/oam3.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, vertical mirroring chr-ram // ???
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/blargg_ppu_tests_2005.09.15b/vbl_clear_time.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring chr-ram  // OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/scanline/scanline.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, vertical mirroring // TODO: pour qu'il fonctionne bien comme openEmu (juste 2 pixels de stars et pas 3), il faut faire les ppu io avant le process line seulement pour les io write, voir si c'est tjs bon par rapport aux autres tests roms
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/palette.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, horizontal mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/full_palette/full_palette.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, horizontal mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/sprite_overflow_tests/5.Emulator.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring // OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/Mapper/mmc3_test_2/rom_singles/4-scanline_timing.nes", std::ios::binary);  // Mapper4, 32kb de prg-rom, 8kb de chr-rom  // TODO: foire sur 4 et 6 (je peux faire passer 6 en changeant la facon de lancer l'irq mais ca fait foirer 5, c'est surement parce que 6 alt teste le mapper3 alternatif : oui)
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/Controller/allpads.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, 8kb de chr-ram, Horizontal mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/apu_test/apu_test.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/blargg_apu_2005.07.30/10.len_halt_timing.nes", std::ios::binary);//TODO: foire sur 10 et 11
    
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/test_apu_2/test_10.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/test_tri_lin_ctr/lin_ctr.nes", std::ios::binary);
    
    // Check that file exists
    assert(ifs.good());
    
    // Loop manager
    LoopManager loopManager(event);
    
    // Cartridge loader
    //NESEmu::Cartridge::Loader::INes inesLoader;//TODO: a la place d'avoir plusieurs loaders qui vont créer la cartridge (ines, ines2, tnes, ...), avoir un seul CartridgeFactory qui crée les cartridges (ne les crée meme pas, on fait cartridgeFactory.insertCartridgeIntoNes(Nes, std::istream) et les loaders vont seulement retourner une information sur la rom (num de mapper, nombre de banque, la mémoire des banques, ...) donc le cartridgeFactory ne devra pas avoir de methodes virtuelles et donc pas de soucis d'avoir la methode template et donc plus de redondance de template parameter comme cette solution que j'ai pour l'instant
    // TODO: et pour que le factory choisisse correctement le loader, il y a des priorités entre loader + une methode qui dit s'il supporte la rom ou non (bool isSupported(std::istream) const et int getPriority() const)
    // TODO: on peut combiner les 2 solutions (ce que j'ai mis en comm au dessus avec la solution deja faite pour le coté d'inserer des cartridge), on aurait juste plus besoin de specifier les template parameter dans le loader car ce serait une methode template (mais a voir car comment faire si Cartridge n'a plus les template parameters ?)
    
    // TODO: comme les mappers sont resolus au compile-time, a chaque mapper ajouté dans le code, il faut l'instantier (dans Factory) et donc il va avoir une duplication de NesImplementation pour chaque mapper, ca va augmenter la taille du code a fond et peut etre foutre la merde dans l'instruction cache ? si ca tombe la version avec virtual dispatch (runtime) sera au final plus rapide car moins de code meme si les appels de methodes sont indirects !!! A TESTER
    
    using Nes = NESEmu::Nes<NESEmu::Model::Ntsc, GraphicHardware<SoundHardware>, SoundHardware>;
    
    NESEmu::Cartridge::Factory<Nes::CpuHardwareInterface, Nes::PpuHardwareInterface> cartridgeFactory;
    cartridgeFactory.registerLoader(std::shared_ptr<NESEmu::Cartridge::Loader::Interface>(new NESEmu::Cartridge::Loader::INes()));
    
    // Create cartridge
    auto cartridge = cartridgeFactory.createCartridgeFromStream(ifs);
    
    // Read persistent memory if necessary
    if (cartridge->hasPersistentMemory()) {
        std::ifstream ifs("../persistentMemory.dat", std::ios::binary);
        
        if (ifs.good()) {
            // Get persitent memory
            auto &persistentMemory = cartridge->getPersistentMemory();
            
            // Read from file
            ifs.read(reinterpret_cast<char *>(persistentMemory.data()), persistentMemory.size());
        }
    }
    
    // Create NES
    Nes nes(graphicHardware, soundHardware);
    
    // Insert cartridge
    nes.insertCartridge(std::move(cartridge));
    
    // Connect controller
    nes.connectController(0, std::move(controller));
    
    // Power up NES
    nes.powerUp();
    
    // Release reset to start NES
    //nes.reset(true);
    
    // Run NES
    //nes.run([&loopManager](Nes &nes){ return loopManager.needToStop(nes); });              // With lambda
    //nes.run(nesLoopNeedToStop<Nes>);                                                       // With function
    //nes.run(loopManager);                                                                  // With functor
    //nes.run(std::bind(&LoopManager::needToStop<Nes>, loopManager, std::placeholders::_1)); // With method (via bind)
    runNes(nes, event);                                                                    // Custom loop
    
    // Remove cartridge
    cartridge = nes.removeCartridge();
    
    // Save persistent memory if necessary
    if (cartridge->hasPersistentMemory()) {
        std::ofstream ofs("../persistentMemory.dat", std::ios::binary);
        
        if (ofs.good()) {
            // Get persistent memory
            auto const &persistentMemory = cartridge->getPersistentMemory();
            
            // Write to file
            ofs.write(reinterpret_cast<char const *>(persistentMemory.data()), persistentMemory.size());
        }
    }
    
    // Clean up SDL
    SDL_Quit();
    
    return 0;
}
