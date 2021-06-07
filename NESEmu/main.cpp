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
//#include <algorithm>//TODO: pour le filtre audio
//#include <cmath>//TODO: pour le filtre audio
#include <random>
#include <cassert>
#include <thread>
#include <SDL.h>
#include "NESEmu/Cartridge/Loader/INes.hpp"
#include "NESEmu/Cartridge/Factory.hpp"
#include "NESEmu/Nes.hpp"
#include "NESEmu/Controller/Standard.hpp"
#include "NESEmu/Controller/Zapper.hpp"

// TODO: voir pour l'optimisation, mesen tourne a 512 fps sur le MSI, NESEmu tourne a 205 (en x64) :(
// TODO: Sur le MSI, Nintendulator est a +- 240 fps avec le son desactivé mais un filtre ntsc je pense
template <class TFrameListener>
struct GraphicManager {
    
    GraphicManager(SDL_Event &event, TFrameListener &frameListener) : _event(event), _frameListener(frameListener), _generator(_randomDevice()), _distribution(0, (256 * 240) - 1) {
        // Initialize video
        SDL_InitSubSystem(SDL_INIT_VIDEO);
        std::cout << SDL_GetError() << "\n";
        
        // Create window
        _window = SDL_CreateWindow("NESEmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 480, /*SDL_WINDOW_FULLSCREEN | */SDL_WINDOW_OPENGL);
        std::cout << SDL_GetError() << "\n";
        
        // Create renderer
        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
        std::cout << SDL_GetError() << "\n";
        
        // Create texture
        _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
        std::cout << SDL_GetError() << "\n";
        
        // Allocate memory
        _palette = (uint32_t *) malloc(sizeof(uint32_t) * 64);
        _pixelsBuffers = (uint32_t *) malloc(sizeof(uint32_t) * 256 * 240 * /*2*/1);
        _pixelBufferIndex = 0;
        
        // TODO: par apres emuler aussi le signal NTSC : http://wiki.nesdev.com/w/index.php/NTSC_video
        // TODO: possible par shader ?
        
        // Copy default palette
        memcpy(_palette, _defaultPalette, sizeof(uint32_t) * 64);
        
        // Read palette file if exists
        std::ifstream ifs("../UnitTestFiles/ntscpalette.pal", std::ios::binary);
        
        if (ifs.good()) {
            for (int i = 0; i < 64; ++i) {
                uint8_t rgb[3];
                ifs.read(reinterpret_cast<char *>(&rgb), 3);
                
                _palette[i] = (rgb[0] << 16) | (rgb[1] << 8) | rgb[2];
            }
        }
    }
    
    ~GraphicManager() {
        // Possible wait for old thread
        /*if (_pixelCopyThread.joinable()) {
            _pixelCopyThread.join();
        }*/
        
        // Free allocated memory
        free(_pixelsBuffers);
        free(_palette);
        
        // Release texture
        SDL_DestroyTexture(_texture);
        
        // Release renderer
        SDL_DestroyRenderer(_renderer);
        
        // Release window
        SDL_DestroyWindow(_window);
        
        // Uninitialize video
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }
    
    uint32_t getPixelIndexInCurrentBuffer(unsigned int x, unsigned int y) const {
        return (256 * 240 * _pixelBufferIndex) + (y * 256) + x;
    }
    
    uint32_t getPixelAtScreenPosition(unsigned int x, unsigned int y) const {
        // Convert screen position to real position (window is 2x the real size)
        x >>= 1;
        y >>= 1;
        
        return _pixelsBuffers[getPixelIndexInCurrentBuffer(x, y)];
    }
    
    void plotPixel(unsigned int x, unsigned int y, uint8_t color, bool emphasizeRed, bool emphasizeGreen, bool emphasizeBlue) {
        assert(x < 256);
        assert(y < 240);
        assert(color < 64);
        
        // Get color on palette
        uint32_t paletteColor = _palette[color];
        
        //paletteColor = (((paletteColor & 0xFF0000) << emphasizeRed) & 0xFF0000) | (((paletteColor & 0xFF00) << emphasizeGreen) & 0xFF00) | (((paletteColor & 0xFF) << emphasizeBlue) & 0xFF);//TODO: voir apres http://wiki.nesdev.com/w/index.php/Colour-emphasis_games
        
        // Set pixel on video buffer
        _pixelsBuffers[getPixelIndexInCurrentBuffer(x, y)] = paletteColor;
        
        // TODO: pour passer telling lys et avoir les input qui se rafraichissent n'importe quand et pas tout le temps au debut du vblank
        // TODO: avec cette technique il update n'importe quand sauf en hblank et vblank
        // Exit if no event already polled this frame
        if (_pollEventPixelCounter == 0) {
            return;
        }
        
        // Decrement poll event pixel counter
        --_pollEventPixelCounter;
        
        // Poll event if counter reached 0
        if (_pollEventPixelCounter == 0) {
            // TODO: a voir j'update les event (et donc le clavier) une fois par frame :  voir si ok et voir si pas plutot le mettre dans TimeManager
            SDL_PollEvent(&_event);
        }
    }
    
    void notifyVBlankStarted() {
        // Possible wait for old thread
        /*if (_pixelCopyThread.joinable()) {
            _pixelCopyThread.join();
        }
        
        // Create pixel copy thread
        _pixelCopyThread = std::thread(&GraphicManager::renderPixelBuffer, this, _pixelBufferIndex);*/
        renderPixelBuffer(0);
        
        // Swap buffer index
        //_pixelBufferIndex = (_pixelBufferIndex + 1) & 0x1;
        
        // Calculate next time for polling event
        _pollEventPixelCounter = _distribution(_generator);
        
        // Notify that a frame has been generated
        _frameListener.notifyFrameGenerated();
    }
    
    void renderPixelBuffer(uint32_t pixelBufferIndex) {
        int texturePitch;
        void *texturePixels;
        
        // Update texture
        SDL_LockTexture(_texture, NULL, &texturePixels, &texturePitch);
        memcpy(texturePixels, &_pixelsBuffers[256 * 240 * pixelBufferIndex], sizeof(uint32_t) * 256 * 240);
        SDL_UnlockTexture(_texture);
        
        // Update renderer
        SDL_RenderClear(_renderer);
        SDL_RenderCopy(_renderer, _texture, NULL, NULL);
        SDL_RenderPresent(_renderer);
        
        // Reset pixels memory to simulate CRT monitors (needed by zapper)
        // Simulate the electron gun drawing pixel by pixel, without this, the pixels stays lighted between frames)
        memset(&_pixelsBuffers[256 * 240 * pixelBufferIndex], 0, sizeof(uint32_t) * 256 * 240);
    }
    
private:
    
    static constexpr uint32_t _defaultPalette[] = {
        0x525252, 0x011A51, 0x0F0F65, 0x230663, 0x36034B, 0x400426, 0x3F0904, 0x321300,
        0x1F2000, 0x0B2A00, 0x002F00, 0x002E0A, 0x00262D, 0x000000, 0x000000, 0x000000,
        0xA0A0A0, 0x1E4A9D, 0x3837BC, 0x5828B8, 0x752194, 0x84235C, 0x822E24, 0x6F3F00,
        0x515200, 0x316300, 0x1A6B05, 0x0E692E, 0x105C68, 0x000000, 0x000000, 0x000000,
        0xFEFFFF, 0x699EFC, 0x8987FF, 0xAE76FF, 0xCE6DF1, 0xE070B2, 0xDE7C70, 0xC8913E,
        0xA6A725, 0x81BA28, 0x63C446, 0x54C17D, 0x56B3C0, 0x3C3C3C, 0x000000, 0x000000,
        0xFEFFFF, 0xBED6FD, 0xCCCCFF, 0xDDC4FF, 0xEAC0F9, 0xF2C1DF, 0xF1C7C2, 0xE8D0AA,
        0xD9DA9D, 0xC9E29E, 0xBCE6AE, 0xB4E5C7, 0xB5DFE4, 0xA9A9A9, 0x000000, 0x000000
    };
    
    SDL_Event &_event;
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    
    uint32_t *_palette;
    uint32_t *_pixelsBuffers;
    uint32_t _pixelBufferIndex;
    
    std::thread _pixelCopyThread;
    
    TFrameListener &_frameListener;
    
    std::random_device _randomDevice;
    std::mt19937 _generator;
    std::uniform_int_distribution<> _distribution;
    uint32_t _pollEventPixelCounter;
};

// Needed by C++14 because it is constexpr and we take defaultPalette by pointer so it is not evaluated
// It is not needed in C++17 because it is implicitly inlined
// See https://stackoverflow.com/questions/8452952/c-linker-error-with-class-static-constexpr
template <class TFrameListener>
constexpr uint32_t GraphicManager<TFrameListener>::_defaultPalette[];

struct SoundManager {
    /*
     Rajouter des methodes pour choisir le volume des canaux dans le soundmanager ou bien carrement laisser le soundmanager faire le mixing en ayant la possibilité d’utiliser la méthode de mixing de l’apu (qui deviendra une static method)
     */
    
    SoundManager(unsigned int frequency, std::size_t bufferSize) : _counter(0), _currentBufferWriteIndex(0), _currentBufferReadIndex(0) {
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
            static_cast<SoundManager *>(param)->fillBuffer(stream, len);
        };
        _audioSpec.userdata = this;
        
        // Open audio device
        _audioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &_audioSpec, &_audioSpec, /*SDL_AUDIO_ALLOW_FREQUENCY_CHANGE*/0);
        std::cout << SDL_GetError() << "\n";
        
        // Unpause audio device
        SDL_PauseAudioDevice(_audioDeviceID, SDL_FALSE);
    }
    
    ~SoundManager() {
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

struct KeyboardStandardControllerManager {
    
    void update() {
        _keyStates = SDL_GetKeyboardState(NULL);
    }
    
    bool getButtonA() const {
        return _keyStates[SDL_SCANCODE_V] != 0;
    }
    
    bool getButtonB() const {
        return _keyStates[SDL_SCANCODE_C] != 0;
    }
    
    bool getButtonSelect() const {
        return _keyStates[SDL_SCANCODE_SPACE] != 0;
    }
    
    bool getButtonStart() const {
        return _keyStates[SDL_SCANCODE_RETURN] != 0;
    }
    
    bool getDirectionalUp() const {
        return _keyStates[SDL_SCANCODE_UP] != 0;
    }
    
    bool getDirectionalDown() const {
        return _keyStates[SDL_SCANCODE_DOWN] != 0;
    }
    
    bool getDirectionalLeft() const {
        return _keyStates[SDL_SCANCODE_LEFT] != 0;
    }
    
    bool getDirectionalRight() const {
        return _keyStates[SDL_SCANCODE_RIGHT] != 0;
    }
    
private:
    uint8_t const *_keyStates;
};

struct JoystickStandardControllerManager {
    
    JoystickStandardControllerManager() : _joystick(nullptr) {
        // Initialize joystick
        SDL_Init(SDL_INIT_JOYSTICK);
        
        // Open joystick
        if (SDL_NumJoysticks() > 0) {
            _joystick = SDL_JoystickOpen(0);
        }
    }
    
    ~JoystickStandardControllerManager() {
        // Close joystick
        if (_joystick != nullptr) {
            SDL_JoystickClose(_joystick);
        }
        
        // Uninitialize joystick
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    }
    
    void update() {
    }
    
    bool getButtonA() const {
        return SDL_JoystickGetButton(_joystick, 0) != 0;    // Button 0 = cross
    }
    
    bool getButtonB() const {
        return SDL_JoystickGetButton(_joystick, 2) != 0;    // Button 2 = Square
    }
    
    bool getButtonSelect() const {
        return SDL_JoystickGetButton(_joystick, 3) != 0;    // Button 3 = Triangle
    }
    
    bool getButtonStart() const {
        return SDL_JoystickGetButton(_joystick, 6) != 0;    // Button 6 = Option
    }
    
    bool getDirectionalUp() const {
        return SDL_JoystickGetButton(_joystick, 11) != 0;   // Button 11 = Up
    }
    
    bool getDirectionalDown() const {
        return SDL_JoystickGetButton(_joystick, 12) != 0;   // Button 12 = Down
    }
    
    bool getDirectionalLeft() const {
        return SDL_JoystickGetButton(_joystick, 13) != 0;   // Button 13 = Left
    }
    
    bool getDirectionalRight() const {
        return SDL_JoystickGetButton(_joystick, 14) != 0;   // Button 14 = Right
    }
    
private:
    SDL_Joystick *_joystick;
};

template <class TGraphicManager>
struct ZapperControllerManager {
    
    ZapperControllerManager(TGraphicManager const &graphicManager) : _graphicManager(graphicManager) {
    }
    
    bool getLightSense() const {
        int x;
        int y;
        
        // Get mouse position
        SDL_GetMouseState(&x, &y);
        
        // Get color at position
        uint32_t color = _graphicManager.getPixelAtScreenPosition(x, y);
        
        // Convert to luminance
        uint8_t luminance = ((color >> 16) * 0.3f) + (((color >> 8) & 0xFF) * 0.59f) + ((color & 0xFF) * 0.11f);
        
        // Check threshold
        return (luminance > lightThreshold);
    }
    
    bool getTrigger() {
        // Get current trigger state
        bool triggerState = (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK) != 0;
        
        // If begin pulled
        if (triggerState && (!_lastTriggerState)) {
            // Save current time
            _lastTriggerPulledTime = SDL_GetPerformanceCounter();
        }
        
        // Save trigger state
        _lastTriggerState = triggerState;
        
        // Get elapsed seconds
        float elapsedSeconds = (SDL_GetPerformanceCounter() - _lastTriggerPulledTime) / static_cast<float>(SDL_GetPerformanceFrequency());
        
        // Trigger only stays for 0.1 sec max
        return triggerState && (elapsedSeconds < maxTriggerTime);
    }
    
private:
    static constexpr float maxTriggerTime = 0.1f;
    static constexpr uint8_t lightThreshold = 154;//63  // TODO: 154 pour ne pas detecter le texte vert de la rom test ruder
    
    TGraphicManager const &_graphicManager;
    Uint64 _lastTriggerPulledTime;
    bool _lastTriggerState;
};

struct TimeManager {
    
    TimeManager(SoundManager &soundManager, int maxFps, bool audioContinuousSync) : _soundManager(soundManager), _frameDuration(1.0f / maxFps), _audioContinuousSync(audioContinuousSync), _frameCounter(0) {
        // Start at current time
        _lastFrameTime = SDL_GetPerformanceCounter();
        _lastFrameCounterTime = _lastFrameTime;
    }
    
    void notifyFrameGenerated() {
        // Get performance frequency
        float performanceFrequency = SDL_GetPerformanceFrequency();
        
        // Check to wait for locked fps
        for (;;) {
            float elapsedSeconds = (SDL_GetPerformanceCounter() - _lastFrameTime) / performanceFrequency;
            
            if (elapsedSeconds >= _frameDuration) {
                break;
            }
        }
        
        // Get current time
        Uint64 currentTime = SDL_GetPerformanceCounter();
        
        // Update sampler frequency if necessary
        if (_audioContinuousSync) {
            //29780.5 = cycles CPU par frame // TODO: voir si PAL !
            _soundManager.setSamplerFrequency(29780.5f / ((currentTime - _lastFrameTime) / performanceFrequency));
        }
        
        // Update frame counter
        ++_frameCounter;
        if (_lastFrameCounterTime < (currentTime - performanceFrequency)) {
            _lastFrameCounterTime = currentTime;
            
            // Show fps
            std::cout << "FPS = " << _frameCounter << "\n";
            
            // Reset counter
            _frameCounter = 0;
        }
        
        // Save last frame time
        _lastFrameTime = currentTime;
    }
    
private:
    SoundManager &_soundManager;
    Uint64 _lastFrameTime;
    Uint64 _lastFrameCounterTime;
    float _frameDuration;
    int _frameCounter;
    bool _audioContinuousSync;
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
        if ((_event.type == SDL_QUIT) || (_event.type == SDL_DROPFILE) || (_x == 0)) {
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
    //unsigned int _x = 35795460;   // TODO: pour tester en profile
    bool resetState = false;
    
    for (;;) {
        nes.clockFull();
        //nes.clock();
        
        //--_x;
        if ((event.type == SDL_QUIT) || (event.type == SDL_DROPFILE)/* || (_x == 0)*/) {
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

int main(int argc, char *argv[]) {
    SDL_Event event;
    
    // Init SDL
    SDL_Init(SDL_INIT_EVENTS);
    
    // Create sound manager for 44100hz and 2048bytes buffer size
    SoundManager soundManager(44100, 2048);
    
    // Create time manager for 60FPS and audio continuous sync
    TimeManager timeManager(soundManager, 60/*1000*/, true);
    
    // Create graphic manager
    GraphicManager<TimeManager> graphicManager(event, timeManager);
    
    // Create standard controller
    KeyboardStandardControllerManager standardControllerManager;
    auto standardController = std::make_unique<NESEmu::Controller::Standard<KeyboardStandardControllerManager>>(standardControllerManager);
    
    // Create zapper controller
    ZapperControllerManager<GraphicManager<TimeManager>> zapperControllerManager(graphicManager);
    auto zapperController = std::make_unique<NESEmu::Controller::Zapper<decltype(zapperControllerManager)>>(zapperControllerManager);
    
    // Choose ROM
    std::string romFilename = (argc > 1) ? argv[1] : "../UnitTestFiles/SMB.nes";
    //"../UnitTestFiles/SMB.nes"                                                    // Ok
    //"../UnitTestFiles/Spelunker.nes"                                              // Ok
    //"../UnitTestFiles/Ms. Pac-Man (Tengen).nes"                                   // Ok
    //"../UnitTestFiles/DK.nes"                                                     // Ok, voir http://forums.nesdev.com/viewtopic.php?f=3&t=19792
    //"../UnitTestFiles/Castlevania.nes"                                            // Ok
    //"../UnitTestFiles/Duck Tales.nes"                                             // Ok
    //"../UnitTestFiles/Battletoads.nes"                                            // Ok
    //"../UnitTestFiles/Paperboy.nes"                                               // Ok
    //"../UnitTestFiles/Huge Insect.nes"                                            // Ok
    //"../UnitTestFiles/SMB2.nes"                                                   // Ok
    //"../UnitTestFiles/SMB3.nes"                                                   // Ok
    //"../UnitTestFiles/Young Indiana Jones Chronicles.nes"                         // Ok
    //"../UnitTestFiles/Adventures of Lolo 2.nes"                                   // Ok
    //"../UnitTestFiles/Crystalis.nes"                                              // Ok
    //"../UnitTestFiles/Silver Surfer.nes"                                          // Ok
    //"../UnitTestFiles/Metroid.nes"                                                // Ok
    //"../UnitTestFiles/Final Fantasy.nes"                                          // Ok
    //"../UnitTestFiles/Zelda.nes"                                                  // Ok
    //"../UnitTestFiles/Zelda 2.nes"                                                // Ok
    //"../UnitTestFiles/Simpsons - Bart Vs the Space Mutants.nes"                   // Ok
    //"../UnitTestFiles/Bill & Ted's Excellent Video Game Adventure.nes"            // Ok
    //"../UnitTestFiles/1942.nes"                                                   // Ok
    //"../UnitTestFiles/Batman - The Video Game.nes"                                // Ok
    //"../UnitTestFiles/Dragon Quest.nes"                                           // Ok
    //"../UnitTestFiles/Duck Hunt.nes"                                              // Ok
    //"../UnitTestFiles/Ghosts'n Goblins.nes"                                       // Ok
    //"../UnitTestFiles/Kid Icarus.nes"                                             // Ok
    //"../UnitTestFiles/Mega Man.nes"                                               // Ok
    //"../UnitTestFiles/R.C. Pro-Am.nes"                                            // Ok
    //"../UnitTestFiles/Teenage Mutant Ninja Turtles.nes"                           // Ok
    //"../UnitTestFiles/Wild Gunman.nes"                                            // Ok
    //"../UnitTestFiles/Contra.nes"                                                 // Ok
    //"../UnitTestFiles/Darkman.nes"                                                // Ok
    //"../UnitTestFiles/Die Hard.nes"                                               // Ok
    //"../UnitTestFiles/Eliminator Boat Duel (USA).nes"                             // Ok
    //"../UnitTestFiles/Isolated Warrior.nes"                                       // Ok
    //"../UnitTestFiles/Terminator 2 - Judgment Day.nes"                            // Ok
    //"../UnitTestFiles/Star Gate.nes"                                              // Ok
    //"../UnitTestFiles/Punch Out.nes"                                              // Ok
    //"../UnitTestFiles/Famicom Wars Bonne Version.nes"                             // Ok
    //"../UnitTestFiles/Famicom Wars.nes"                                           // TODO Voir ci dessous
    /*
     Les roms sont corrompues, la taille ne correspond pas aux infos du header et ce ne sont pas des puissances de 2 :
     
     La vraie rom fait : 128ko de PRG-ROM + 64ko de CHR-ROM (+ 16 octets du header) = 192ko (196624 octets).
     
     Meme si le jeu n'a que 16 (0x10) banques de 4ko, il écrit bien les valeurs supérieures a 0x10 donc il faut que le masque fasse effet mais si la rom est corrompue, la taille est corrompue et le masque aussi et c'est pour ca que ca n'allait pas, maintenant il faut avoir la possibilité de corriger la rom en ayant dans data un flag isCorrupted et si oui avoir la possibilité de charger la rom en specifiant d'autres tailles.
     */
    //"../UnitTestFiles/Fire Emblem Gaiden.nes"                                     // Ok
    //"../UnitTestFiles/Fire Emblem - Ankoku Ryuu to Hikari no Tsurugi.nes"         // Ok
    //"../UnitTestFiles/Megaman 6.nes"                                              // Ok, MMC3 avec chr-ram !
    //"../UnitTestFiles/Metal Gear.nes"                                             // Ok
    //"../UnitTestFiles/Tom & Jerry.nes"                                            // Ok
    //"../UnitTestFiles/Time Lord.nes"                                              // Ok
    //"../UnitTestFiles/Tetris.nes"                                                 // Ok
    //"../UnitTestFiles/Marble Madness.nes"                                         // Ok
    //"../UnitTestFiles/Pirates!.nes"                                               // Ok
    //"../UnitTestFiles/Mickey's Safari in Letterland.nes"                          // TODO: probleme avec la barre de statut en bas (MMC3 IRQ ?) : ou peut etre le MMC3 alternatif ? dans nintaco il est marqué MMC3 submapper 3 (normalement oui car cette version lance un irq 4 pixels en retard par rapport au MMC3 normal) : http://forums.nesdev.com/viewtopic.php?f=3&t=15678
    //"../UnitTestFiles/Mickey Mouse.nes"                                           // Ok
    //"../UnitTestFiles/Hook.nes"                                                   // Ok
    //"../UnitTestFiles/Hoops.nes"                                                  // Ok
    //"../UnitTestFiles/Balloon Fight.nes"                                          // Ok
    //"../UnitTestFiles/Bases Loaded II - Second Season.nes"                        // Ok
    //"../UnitTestFiles/Battletoads-Double Dragon.nes"                              // Ok
    //"../UnitTestFiles/Burai Fighter.nes"                                          // Ok
    //"../UnitTestFiles/G.I. Joe - A Real American Hero.nes"                        // Ok
    //"../UnitTestFiles/Galaxian.nes"                                               // Ok
    //"../UnitTestFiles/Galaxian 8Ko PRG.nes"                                       // Pour la version de 8kb, il faut la detecter via un CRC pour avoir la bonne taille car le format ines ne permet d'avoir que des banques de 16kb !!!
    //"../UnitTestFiles/Low G Man - The Low Gravity Man.nes"                        // Ok
    //"../UnitTestFiles/The Magic of Scheherazade.nes"                              // TODO: ne va pas, a cause du Disk Dude dans le header (MMC1 normalement mais lu comme 0x41), le jeu fonctionne bien si forcé en MMC1
    //"../UnitTestFiles/Slalom.nes"                                                 // Ok
    //"../UnitTestFiles/Star Trek - 25th Anniversary.nes"                           // Ok
    //"../UnitTestFiles/Castlevania 2.nes"                                          // Ok
    //"../UnitTestFiles/Castlevania 3.nes"                                          // Ok
    //"../UnitTestFiles/Difference (Unl).nes"                                       // Ok
    //"../UnitTestFiles/Alien Syndrome (J).nes"                                     // TODO : voir http://forums.nesdev.com/viewtopic.php?f=2&t=19957&p=248991&hilit=alien#p248991
    //"../UnitTestFiles/Kirby's Adventure.nes"                                      // Ok
    //"../UnitTestFiles/Gun Nac.nes"                                                // Ok
    
    //"../UnitTestFiles/TestROM/CPU/oc-r1a/oc.nes"                                  // Ok
    //"../UnitTestFiles/TestROM/CPU/NEStress/NEStress.nes"                          // Ok
    //"../UnitTestFiles/TestROM/CPU/nestest.nes"                                    // Ok
    //"../UnitTestFiles/TestROM/CPU/branch_timing_tests/1.Branch_Basics.nes"        // Ok
    //"../UnitTestFiles/TestROM/CPU/branch_timing_tests/2.Backward_Branch.nes"      // Ok
    //"../UnitTestFiles/TestROM/CPU/branch_timing_tests/3.Forward_Branch.nes"       // Ok
    //"../UnitTestFiles/TestROM/CPU/instr_misc/instr_misc.nes"                      // TODO: ne passe pas le 4eme car peut etre parce qu'il teste des unofficial opcode que je n'ai pas encore implementé ?
    //"../UnitTestFiles/TestROM/CPU/cputime.nes"                                    // TODO ?
    //"../UnitTestFiles/TestROM/CPU/instr_timing/rom_singles/1-instr_timing.nes"    // Ok sauf pour les unofficial opcodes non implementé
    //"../UnitTestFiles/TestROM/CPU/cpu_exec_space/test_cpu_exec_space_apu.nes"     // Ok mais ne fonctionne pas si zapper connecté au port 2 !
    //"../UnitTestFiles/TestROM/CPU/cpu_interrupts_v2/cpu_interrupts.nes"           // Ok
    //"../UnitTestFiles/TestROM/CPU/cpu_dummy_writes/cpu_dummy_writes_ppumem.nes"   // Ok
    //"../UnitTestFiles/TestROM/CPU/cpu_reset/registers.nes"                        // Ok
    //"../UnitTestFiles/TestROM/CPU/instr_test-v5/official_only.nes"                // TODO: ne foire que sur les instructions pas encore supportées
    
    //"../UnitTestFiles/TestROM/PPU/oam_read/oam_read.nes"                          // Ok
    //"../UnitTestFiles/TestROM/PPU/ppu_read_buffer/test_ppu_read_buffer.nes"       // Ok
    //"../UnitTestFiles/TestROM/PPU/ppu_sprite_hit/ppu_sprite_hit.nes"              // Ok
    //"../UnitTestFiles/TestROM/PPU/ppu_vbl_nmi/ppu_vbl_nmi.nes"                    // Ok
    //"../UnitTestFiles/TestROM/PPU/ppu_sprite_overflow/ppu_sprite_overflow.nes"    // Ok
    //"../UnitTestFiles/TestROM/PPU/ppu_open_bus/ppu_open_bus.nes"                  // Ok
    //"../UnitTestFiles/TestROM/PPU/nmi_sync/demo_ntsc.nes"                         // Ok
    //"../UnitTestFiles/TestROM/PPU/oam_stress/oam_stress.nes"                      // Ok, juste long a executer, attendre bien la fin
    //"../UnitTestFiles/TestROM/PPU/oamtest3/oam3.nes"                              // TODO ?
    //"../UnitTestFiles/TestROM/PPU/blargg_ppu_tests_2005.09.15b/vram_access.nes"   // Ok
    //"../UnitTestFiles/TestROM/PPU/scanline/scanline.nes"                          // Ok
    //"../UnitTestFiles/TestROM/PPU/palette.nes"                                    // Ok
    //"../UnitTestFiles/TestROM/PPU/full_palette/full_palette.nes"                  // Ok
    
    //"../UnitTestFiles/TestROM/Mapper/mmc3_test_2/rom_singles/5-MMC3.nes"          // TODO: foire sur 6 (je peux faire passer 6 en changeant la facon de lancer l'irq mais ca fait foirer 5, c'est surement parce que 6 alt teste le mapper3 alternatif : oui)
    //"../UnitTestFiles/TestROM/Controller/bntest/bntest_h.nes"                     // TODO: ne va pas !!! : normal, c'est un test de mapper que je n'ai pas encore implémenté
    //"../UnitTestFiles/TestROM/Mapper/holydiverbatman-bin-0.01/testroms/M10_P128K_C64K_S8K.nes"    // Ok mais vérifier tous
    
    //"../UnitTestFiles/TestROM/Controller/allpads.nes"                             // Ok
    //"../UnitTestFiles/TestROM/Controller/zapper/zapper_flip.nes"                  // Ok
    //"../UnitTestFiles/TestROM/Controller/ruder-0.03a/ruder.nes"                   // Ok, voir si sur vraie nes le light detecte le texte vert du menu ou pas
    //"../UnitTestFiles/TestROM/Controller/read_joy3/thorough_test.nes"             // Ok, count_errors_fast: Errors=16/1000, counts_errors: Conflits=47/1000, thorough_test passe mais il faut attendre au moins 60sec, voir si aussi long sur mesen : non sur mesen ca prend 3 secondes
    //"../UnitTestFiles/TestROM/Controller/telling-lys-0.01/telling-lys.nes"        // Ok
    
    //"../UnitTestFiles/TestRom/APU/apu_test/apu_test.nes"                          // Ok
    //"../UnitTestFiles/TestRom/APU/blargg_apu_2005.07.30/11.len_reload_timing.nes" // Ok
    //"../UnitTestFiles/TestRom/APU/test_apu_2/test_10.nes"                         // TODO: 3 fail and pass (reset), 5 6 fail
    //"../UnitTestFiles/TestRom/APU/test_apu_m/test_11.nes"                         // TODO: 11 fail
    //"../UnitTestFiles/TestRom/APU/test_tri_lin_ctr/lin_ctr.nes"                   // Ok
    //"../UnitTestFiles/TestRom/APU/noisy/noisy_v1.nes"                             // TODO: a comparer avec un autre emulateur
    //"../UnitTestFiles/TestRom/APU/DmcPowerOnTest_v3/DmcPowerOnBuzz.nes"           // Ok
    //"../UnitTestFiles/TestRom/APU/apu_mixer/dmc.nes"                              // TODO: pas tres bon pour square, parfait pour triangle
    //"../UnitTestFiles/TestRom/APU/apu_reset/works_immediately.nes"                // Ok (pour 4017_timing, 9 au power et 9/10 au reset, mesen est a 9 pour les 2 (mais voir si ca tombe c'est 9/10 aussi)
    //"../UnitTestFiles/TestRom/APU/FrameCounterIrqAckTest/test.nes"                // TODO: attention car apparemment ca fail sur une vraie nes ! // https://forums.nesdev.com/viewtopic.php?f=3&t=13844&sid=cbcaafaac1023fdf00510a47962184ad&start=300
    //"../UnitTestFiles/TestRom/APU/status.nes"                                     // TODO ? juste un beep a chaque test
    //"../UnitTestFiles/TestRom/APU/dpcmletterbox/dpcmletterbox.nes"                // TODO: Ok mais quand DMC DMA pendant SPR DMA l'image et le son deconne pendant 1 ou 2 frames
    //"../UnitTestFiles/TestRom/APU/apu_phase_reset/apu_phase_reset.nes"            // Ok ( https://forums.nesdev.com/viewtopic.php?t=15346 )
    //"../UnitTestFiles/TestRom/APU/square_timer_div2/square_timer_div2.nes"        // Ok
    //"../UnitTestFiles/TestRom/APU/test_apu_env/test_apu_env.nes"                  // Ok normalement
    //"../UnitTestFiles/TestRom/APU/test_apu_sweep/sweep_sub.nes"                   // Ok
    //"../UnitTestFiles/TestRom/APU/test_apu_timers/dmc_pitch.nes"                  // Ok
    //"../UnitTestFiles/TestRom/APU/volume_tests/volumes.nes"                       // TODO ?
    
    //"../UnitTestFiles/TestROM/DMA/sprdma_and_dmc_dma/sprdma_and_dmc_dma_512.nes"  // Ok
    //"../UnitTestFiles/TestRom/DMA/dmc_dma_during_read4/read_write_2007.nes"       // Ok
    //"../UnitTestFiles/TestRom/DMA/dma_sync_test_loop_delay_badrol.nes"            // Ok, doit devenir blanc mais le probleme est que cette rom de test n'attend pas correctement le PPU warmup et donc je dois le desactiver pour voir le resultat sinon ca reste gris car la couleur de background est ecrite trop tot
    //"../UnitTestFiles/TestRom/DMA/dma_sync_test_loop_delay_goodrol.nes"           // Ok, doit etre noir et devenir blanc si pad right press mais pareil qu'au dessus pour le ppu warm up !
    //"../UnitTestFiles/TestROM/DMA/dma_sync_test_v2/dma_sync_test.nes"             // Ok, Ecran noir puis blanc si pad right
    //"../UnitTestFiles/TestROM/DMA/dmc_dma_start_test/dmc_dma_start_test.nes"      // Ok ? http://forums.nesdev.com/viewtopic.php?f=3&t=19915
    //"../UnitTestFiles/TestROM/DMA/oam_flicker_test_reenable.nes"                  // Ok ? http://forums.nesdev.com/viewtopic.php?f=3&t=19915
    //"../UnitTestFiles/TestROM/DMA/oam_flicker_test/oam_flicker_test.nes"          // Ok ? http://forums.nesdev.com/viewtopic.php?f=3&t=19915
    
    //"../UnitTestFiles/Demo/19.nes"                                                // TODO: qq roms ne fonctionnent pas a cause du PPU warm up mais la 19 ne va pas du tout meme sans le PPU warm up alors qu'elle fonctionne bien dans nintaco !!! la 4 non plus mais c'est a cause du prg-ram size qui est mal detecté dans le header, a voir, pour la 19 je n'arrive pas a la faire aller mais elle ne va pas non plus dans openemu, voir si ca va dans mesen ! : ne fonctionne pas non plus dans mesen, il faut changer l'instruction cli par nop a l'adresse 0x8007 pour qu'elle fonctionne : ok quand fait ca
    
    // Create Nes alias type for conveniance
    using Nes = NESEmu::Nes<NESEmu::Model::Ntsc, GraphicManager<TimeManager>, SoundManager>;
    
    // Create NES
    Nes nes(graphicManager, soundManager);
    
    // Connect controllers
    nes.connectController(0, std::move(standardController));
    nes.connectController(1, std::move(zapperController));
    
    // Create Cartridge factory and register INes loader
    NESEmu::Cartridge::Factory<Nes::CpuHardwareInterfaceType, Nes::PpuHardwareInterfaceType> cartridgeFactory;
    cartridgeFactory.registerLoader(std::shared_ptr<NESEmu::Cartridge::Loader::Interface>(new NESEmu::Cartridge::Loader::INes()));
    
    // Enable drop file
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    
    for (;;) {
        // Open ROM
        std::ifstream ifs(romFilename, std::ios::binary);
        
        decltype(cartridgeFactory.createCartridgeFromStream(ifs)) cartridge;
        
        // Check if file exists
        if (ifs.good()) {
            // Create cartridge
            cartridge = cartridgeFactory.createCartridgeFromStream(ifs);
            
            // Read persistent memory if necessary
            if (cartridge->hasPersistentMemory()) {
                std::ifstream ifs(romFilename + ".sram", std::ios::binary);
                
                if (ifs.good()) {
                    // Get persitent memory
                    auto &persistentMemory = cartridge->getPersistentMemory();
                    
                    // Read from file
                    ifs.read(reinterpret_cast<char *>(persistentMemory.data()), persistentMemory.size());
                }
            }
            
            // Insert cartridge
            nes.insertCartridge(std::move(cartridge));
        }
        
        // Power up NES
        nes.powerUp();
        
        // Run NES
        // Create Loop manager
        //LoopManager loopManager(event);
        //nes.run([&loopManager](Nes &nes){ return loopManager.needToStop(nes); });              // With lambda
        //nes.run(nesLoopNeedToStop<Nes>);                                                       // With function
        //nes.run(loopManager);                                                                  // With functor
        //nes.run(std::bind(&LoopManager::needToStop<Nes>, loopManager, std::placeholders::_1)); // With method (via bind)
        runNes(nes, event);                                                                    // Custom loop
        
        // Check if file exists
        if (ifs.good()) {
            // Remove cartridge
            cartridge = nes.removeCartridge();
            
            // Save persistent memory if necessary
            if (cartridge->hasPersistentMemory()) {
                std::ofstream ofs(romFilename + ".sram", std::ios::binary);
                
                if (ofs.good()) {
                    // Get persistent memory
                    auto const &persistentMemory = cartridge->getPersistentMemory();
                    
                    // Write to file
                    ofs.write(reinterpret_cast<char const *>(persistentMemory.data()), persistentMemory.size());
                }
            }
        }
        
        // If exited
        if (event.type != SDL_DROPFILE) {
            break;
        }
        
        // Get dropped file
        char *dropFile = event.drop.file;
        romFilename = dropFile;
        SDL_free(dropFile);
        
        // Reset event (to avoid exit immediately the runLoop)
        event.type = 0;
    }
    
    // Clean up SDL
    SDL_Quit();
    
    return 0;
}
