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
#include <random>//TODO: pour le rafraichissement aleatoire des inputs pour telling lys
#include <cassert>
#include <SDL.h>
#include "NESEmu/Cartridge/Loader/INes.hpp"
#include "NESEmu/Cartridge/Factory.hpp"
#include "NESEmu/Nes.hpp"
#include "NESEmu/Controller/Standard.hpp"
#include "NESEmu/Controller/Zapper.hpp"

// TODO: voir pour l'optimisation, mesen tourne a 512 fps sur le MSI, NESEmu tourne a 205 (en x64) :(
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
        _pixels = (uint32_t *) malloc(sizeof(uint32_t) * 256 * 240);
        
        // Read palette
        std::ifstream ifs("../UnitTestFiles/ntscpalette.pal", std::ios::binary);
        // TODO: par apres emuler aussi le signal NTSC : http://wiki.nesdev.com/w/index.php/NTSC_video
        // TODO: possible par shader ?
        
        // Check that file exists
        assert(ifs.good());
        
        for (int i = 0; i < 64; ++i) {
            uint8_t rgb[3];
            ifs.read(reinterpret_cast<char *>(&rgb), 3);
            
            _palette[i] = (rgb[0] << 16) | (rgb[1] << 8) | rgb[2];
        }
    }
    
    ~GraphicManager() {
        // Free allocated memory
        free(_pixels);
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
    
    uint32_t getPixelAtScreenPosition(unsigned int x, unsigned int y) const {
        // Convert screen position to real position (window is 2x the real size)
        x >>= 1;
        y >>= 1;
        
        return _pixels[(y * 256) + x];
    }
    
    void plotPixel(unsigned int x, unsigned int y, uint8_t color, bool emphasizeRed, bool emphasizeGreen, bool emphasizeBlue) {
        assert(x < 256);
        assert(y < 240);
        assert(color < 64);
        
        // Get color on palette
        uint32_t paletteColor = _palette[color];
        
        //paletteColor = (((paletteColor & 0xFF0000) << emphasizeRed) & 0xFF0000) | (((paletteColor & 0xFF00) << emphasizeGreen) & 0xFF00) | (((paletteColor & 0xFF) << emphasizeBlue) & 0xFF);//TODO: voir apres http://wiki.nesdev.com/w/index.php/Colour-emphasis_games
        
        // Set pixel on video buffer
        _pixels[(y * 256) + x] = paletteColor;
        
        // TODO: pour passer telling lys et avoir les input qui se rafraichisse n'importe quand et pas tout le temps au debut du vblank
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
        
        // Reset pixels memory to simulate CRT monitors (needed by zapper)
        // Simulate the electron gun drawing pixel by pixel, without this, the pixels stays lighted between frames)
        memset(_pixels, 0, sizeof(uint32_t) * 256 * 240);
        
        // Calculate next time for polling event
        _pollEventPixelCounter = _distribution(_generator);
        
        // Notify that a frame has been generated
        _frameListener.notifyFrameGenerated();
    }
    
private:
    SDL_Event &_event;
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    
    uint32_t *_palette;
    uint32_t *_pixels;
    
    TFrameListener &_frameListener;
    
    std::random_device _randomDevice;
    std::mt19937 _generator;
    std::uniform_int_distribution<> _distribution;
    uint32_t _pollEventPixelCounter;
};

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

struct StandardControllerManager {
    
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
        
        // TODO: ce n'est nécessaire que pour synchroniser +- le son quand le frame rate est different de 60fps : ca devra etre une option a activer / desactiver
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
        if /*(*/(event.type == SDL_QUIT)/* || (_x == 0))*/ {
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
    
    SoundManager soundManager(44100, 2048);
    TimeManager timeManager(soundManager, 60, /*false*/true);
    GraphicManager<TimeManager> graphicManager(event, timeManager);
    StandardControllerManager standardControllerManager;
    ZapperControllerManager<GraphicManager<TimeManager>> zapperControllerManager(graphicManager);
    auto standardController = std::make_unique<NESEmu::Controller::Standard<StandardControllerManager>>(standardControllerManager);
    auto zapperController = std::make_unique<NESEmu::Controller::Zapper<decltype(zapperControllerManager)>>(zapperControllerManager);
    
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
    //std::ifstream ifs("../UnitTestFiles/SMB3.nes", std::ios::binary);  // Mapper4, 256kb de prg-rom, 128kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/SMB2.nes", std::ios::binary);  // Mapper4, 128kb de prg-rom, 128kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/Young Indiana Jones Chronicles.nes", std::ios::binary);  // Mapper4, 128kb de prg-rom, 128kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/Adventures of Lolo 2.nes", std::ios::binary);  // Mapper4, 32kb de prg-rom, 32kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/Crystalis.nes", std::ios::binary);  // Mapper4, 256kb de prg-rom, 128kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/Silver Surfer.nes", std::ios::binary);  // Mapper4, 128kb de prg-rom, 256kb de chr-rom
    //std::ifstream ifs("../UnitTestFiles/Metroid.nes", std::ios::binary);  // Mapper1, 128kb de prg-rom
    //std::ifstream ifs("../UnitTestFiles/Final Fantasy.nes", std::ios::binary);  // Mapper1, 256kb de prg-rom
    //std::ifstream ifs("../UnitTestFiles/Zelda.nes", std::ios::binary);  // Mapper1, 128kb de prg-rom
    //std::ifstream ifs("../UnitTestFiles/Zelda 2.nes", std::ios::binary);  // Mapper1, 128kb de prg-rom, 128 de chr-rom // TODO: bug mais peut etre parce que le mapper1 n'est pas complet (selon les versions du mapper !) deja c du chr-ram et ici c du chr-rom : oui en plus il y a 8ko dans le mapper1 et ici 128 !!! : OK
    //std::ifstream ifs("../UnitTestFiles/Simpsons - Bart Vs the Space Mutants.nes", std::ios::binary);  // Mapper1, 128kb de prg-rom, 128 de chr-rom, IL Y A LE BUG DU SCOREBAR QUI SHAKE UN PEU, a voir (timing ppu ?) : ca ne vibre plus a chaque fois : ok, avait besoin du delai pour la copie de temporary sur address
    //std::ifstream ifs("../UnitTestFiles/Bill & Ted's Excellent Video Game Adventure.nes", std::ios::binary);  // Mapper1, 128kb de prg-rom, 128 de chr-rom // TODO : ne fonctionne pas car on doit gerer la double ecriture dans le MMC1 : ok
    
    //std::ifstream ifs("../UnitTestFiles/1942.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Batman - The Video Game.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Dragon Quest.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Duck Hunt.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Ghosts'n Goblins.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Kid Icarus.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Mega Man.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/R.C. Pro-Am.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Teenage Mutant Ninja Turtles.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Wild Gunman.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Contra.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Darkman.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Die Hard.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Eliminator Boat Duel (USA).nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Isolated Warrior.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Terminator 2 - Judgment Day.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Star Gate.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Punch Out.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Famicom Wars Bonne Version.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Famicom Wars.nes", std::ios::binary);//TODO: Voir ci dessous
    /*
     Les roms sont corrompues, la taille ne correspond pas aux infos du header et ce ne sont pas des puissances de 2 :
     
     La vraie rom fait : 128ko de PRG-ROM + 64ko de CHR-ROM (+ 16 octets du header) = 192ko (196624 octets).
     
     Meme si le jeu n'a que 16 (0x10) banques de 4ko, il écrit bien les valeurs supérieures a 0x10 donc il faut que le masque fasse effet mais si la rom est corrompue, la taille est corrompue et le masque aussi et c'est pour ca que ca n'allait pas, maintenant il faut avoir la possibilité de corriger la rom en ayant dans data un flag isCorrupted et si oui avoir la possibilité de charger la rom en specifiant d'autres tailles.
     */
    //std::ifstream ifs("../UnitTestFiles/Fire Emblem Gaiden.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Fire Emblem - Ankoku Ryuu to Hikari no Tsurugi.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Megaman 6.nes", std::ios::binary);// Ok, MMC3 avec chr-ram !
    //std::ifstream ifs("../UnitTestFiles/Metal Gear.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Tom & Jerry.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Time Lord.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Tetris.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Marble Madness.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Pirates!.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Mickey's Safari in Letterland.nes", std::ios::binary);//TODO: probleme avec la barre de statut en bas (MMC3 IRQ ?) : ou peut etre le MMC3 alternatif ? dans nintaco il est marqué MMC3 submapper 3 (normalement oui car cette version lance un irq 4 pixels en retard par rapport au MMC3 normal) : http://forums.nesdev.com/viewtopic.php?f=3&t=15678
    //std::ifstream ifs("../UnitTestFiles/Mickey Mouse.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Hook.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Hoops.nes", std::ios::binary);
    
    //std::ifstream ifs("../UnitTestFiles/Balloon Fight.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Bases Loaded II - Second Season.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Battletoads-Double Dragon.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Burai Fighter.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/G.I. Joe - A Real American Hero.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Galaxian.nes", std::ios::binary);// Ok car version avec 16kb de PRG
    //std::ifstream ifs("../UnitTestFiles/Galaxian 8Ko PRG.nes", std::ios::binary);// Pour la version de 8kb, il faut la detecter via un CRC pour avoir la bonne taille car le format ines ne permet d'avoir que des banques de 16kb !!!
    //std::ifstream ifs("../UnitTestFiles/Low G Man - The Low Gravity Man.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/The Magic of Scheherazade.nes", std::ios::binary);//TODO: ne va pas, a cause du Disk Dude dans le header (MMC1 normalement mais lu comme 0x41), le jeu fonctionne bien si forcé en MMC1
    //std::ifstream ifs("../UnitTestFiles/Slalom.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Star Trek - 25th Anniversary.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/Castlevania 2.nes", std::ios::binary);
    std::ifstream ifs("../UnitTestFiles/Castlevania 3.nes", std::ios::binary);
    

    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/nestest.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/branch_timing_tests/1.Branch_Basics.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/branch_timing_tests/2.Backward_Branch.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/branch_timing_tests/3.Forward_Branch.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/instr_misc/instr_misc.nes", std::ios::binary);  // Mapper1, 64kb de prg-rom, horizontal mirroring TODO: ne passe pas le 4eme car besoin de l'APU : ne va tjs pas pour le 4eme avec le frame counter implementé !!! -> peut etre parce qu'il teste des unofficial opcode que je n'ai pas encore implementé ?
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cputime.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, Horizontal mirroring // ?
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/instr_timing/rom_singles/1-instr_timing.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // TODO: a voir une fois le APU length counter terminé : ok sauf pour les unofficial opcodes non implementé
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_exec_space/test_cpu_exec_space_apu.nes", std::ios::binary);  // Ok mais ne fonctionne pas si zapper connecté au port 2 !
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_interrupts_v2/cpu_interrupts.nes", std::ios::binary);  // Mapper1, 80kb de prg-rom // Ok!
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_dummy_writes/cpu_dummy_writes_ppumem.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/cpu_reset/registers.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/CPU/instr_test-v5/official_only.nes", std::ios::binary);  // Mapper1, 256kb de prg-rom, vertical mirroring // TODO: ne foire que sur les instructions pas encore supportées
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/oam_read/oam_read.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_read_buffer/test_ppu_read_buffer.nes", std::ios::binary);  // Mapper3, 16kb de prg-rom, 32 de chr-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_sprite_hit/ppu_sprite_hit.nes", std::ios::binary);  // Mapper1, 256kb de prg-rom, vertical mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_vbl_nmi/ppu_vbl_nmi.nes", std::ios::binary);  // Mapper1, 256kb de prg-rom, vertical mirroring OK !!!
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_sprite_overflow/ppu_sprite_overflow.nes", std::ios::binary);// Mapper1, 128kb de prg-rom, vertical mirroring OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/ppu_open_bus/ppu_open_bus.nes", std::ios::binary); // Ok !
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/nmi_sync/demo_ntsc.nes", std::ios::binary);// Mapper0, 32kb de prg-rom, horizontal mirroring TODO: ne fonctionne pas correctement ! : maintenant oui presque correctement !!!
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/oam_stress/oam_stress.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring // Ok, juste long a executer, attendre bien la fin
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/oamtest3/oam3.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, vertical mirroring chr-ram // ???
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/blargg_ppu_tests_2005.09.15b/vram_access.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, horizontal mirroring chr-ram  // OK
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/scanline/scanline.nes", std::ios::binary);  // Mapper0, 16kb de prg-rom, vertical mirroring // TODO: pour qu'il fonctionne bien comme openEmu (juste 2 pixels de stars et pas 3), il faut faire les ppu io avant le process line seulement pour les io write, voir si c'est tjs bon par rapport aux autres tests roms
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/palette.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, horizontal mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/PPU/full_palette/full_palette.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, horizontal mirroring
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/Mapper/mmc3_test_2/rom_singles/5-MMC3.nes", std::ios::binary);  // Mapper4, 32kb de prg-rom, 8kb de chr-rom  // TODO: foire sur 6 (je peux faire passer 6 en changeant la facon de lancer l'irq mais ca fait foirer 5, c'est surement parce que 6 alt teste le mapper3 alternatif : oui)
    //std::ifstream ifs("../UnitTestFiles/TestROM/Controller/bntest/bntest_h.nes", std::ios::binary);//TODO: ne va pas !!! : normal, c'est un test de mapper que je n'ai pas encore implémenté
    //std::ifstream ifs("../UnitTestFiles/TestROM/Mapper/holydiverbatman-bin-0.01/testroms/M10_P128K_C64K_S8K.nes", std::ios::binary);
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/Controller/allpads.nes", std::ios::binary);  // Ok !
    //std::ifstream ifs("../UnitTestFiles/TestROM/Controller/zapper/zapper_flip.nes", std::ios::binary); // Ok !
    //std::ifstream ifs("../UnitTestFiles/TestROM/Controller/ruder-0.03a/ruder.nes", std::ios::binary); // Ok !, voir si sur vraie nes le light detecte le texte vert du menu ou pas
    //std::ifstream ifs("../UnitTestFiles/TestROM/Controller/read_joy3/thorough_test.nes", std::ios::binary);//count_errors_fast: Errors=16/1000, counts_errors: Conflits=47/1000, thorough_test passe mais il faut attendre au moins 60sec, voir si aussi long sur mesen : non sur mesen ca prend 3 secondes
    //std::ifstream ifs("../UnitTestFiles/TestROM/Controller/telling-lys-0.01/telling-lys.nes", std::ios::binary); // Ok !
    
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/apu_test/apu_test.nes", std::ios::binary);    // Ok !
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/blargg_apu_2005.07.30/11.len_reload_timing.nes", std::ios::binary); // Ok !
    
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/test_apu_2/test_10.nes", std::ios::binary);//TODO: 3 fail and pass (reset), 5 6 fail
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/test_apu_m/test_11.nes", std::ios::binary);//TODO: 11 fail
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/test_tri_lin_ctr/lin_ctr.nes", std::ios::binary);
    
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/noisy/noisy_v1.nes", std::ios::binary);//TODO: a comparer avec un autre emulateur
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/DmcPowerOnTest_v3/DmcPowerOnBuzz.nes", std::ios::binary);//TODO: a comparer avec un autre emulateur
    
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/apu_mixer/dmc.nes", std::ios::binary);//TODO: pas tres bon pour square, parfait pour triangle
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/apu_reset/works_immediately.nes", std::ios::binary);    // Ok ! (pour 4017_timing, 9 au power et 9/10 au reset, mesen est a 9 pour les 2 (mais voir si ca tombe c'est 9/10 aussi)
    
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/FrameCounterIrqAckTest/test.nes", std::ios::binary);//TODO: attention car apparemment ca fail sur une vraie nes ! // https://forums.nesdev.com/viewtopic.php?f=3&t=13844&sid=cbcaafaac1023fdf00510a47962184ad&start=300
    
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/status.nes", std::ios::binary);//? juste un beep a chaque test
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/dpcmletterbox/dpcmletterbox.nes", std::ios::binary);//TODO: vibre un peu (surement le meme probleme que simpsons !!! Ca ne vibre presque plus, surement un probleme de dmc dma avec le spr dma !!!
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/apu_phase_reset/apu_phase_reset.nes", std::ios::binary);    // Ok ! ( https://forums.nesdev.com/viewtopic.php?t=15346 )
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/square_timer_div2/square_timer_div2.nes", std::ios::binary);  // Ok !
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/test_apu_env/test_apu_env.nes", std::ios::binary);  // Ok normalement !
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/test_apu_sweep/sweep_sub.nes", std::ios::binary);  // Ok !
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/test_apu_timers/dmc_pitch.nes", std::ios::binary);  // Ok pour tous (mais reverifier le DMC une fois bien implementé avec les timing CPU/DMA)
    //std::ifstream ifs("../UnitTestFiles/TestRom/APU/volume_tests/volumes.nes", std::ios::binary);//?
    
    //std::ifstream ifs("../UnitTestFiles/TestROM/DMA/sprdma_and_dmc_dma/sprdma_and_dmc_dma_512.nes", std::ios::binary);  // Mapper0, 32kb de prg-rom, vertical mirroring
    //std::ifstream ifs("../UnitTestFiles/TestRom/DMA/dmc_dma_during_read4/read_write_2007.nes", std::ios::binary);   // Ok sauf pour le double_2007_read
    //std::ifstream ifs("../UnitTestFiles/TestRom/DMA/dma_sync_test_loop_delay_badrol.nes", std::ios::binary);    // Ok, doit devenir blanc mais le probleme est que cette rom de test n'attend pas correctement le PPU warmup et donc je dois le desactiver pour voir le resultat sinon ca reste gris car la couleur de background est ecrite trop tot
    //std::ifstream ifs("../UnitTestFiles/TestRom/DMA/dma_sync_test_loop_delay_goodrol.nes", std::ios::binary); // Ok, doit etre noir et devenir blanc si pad right press mais pareil qu'au dessus pour le ppu warm up !
    //std::ifstream ifs("../UnitTestFiles/TestROM/DMA/dma_sync_test_v2/dma_sync_test.nes", std::ios::binary); // Ok, Ecran noir puis blanc si pad right
    
    //std::ifstream ifs("../UnitTestFiles/Demo/19.nes", std::ios::binary);    // TODO: qq roms ne fonctionnent pas a cause du PPU warm up mais la 19 ne va pas du tout meme sans le PPU warm up alors qu'elle fonctionne bien dans nintaco !!! la 4 non plus mais c'est a cause du prg-ram size qui est mal detecté dans le header, a voir, pour la 19 je n'arrive pas a la faire aller mais elle ne va pas non plus dans openemu, voir si ca va dans mesen ! : ne fonctionne pas non plus dans mesen, il faut changer l'instruction cli par nop a l'adresse 0x8007 pour qu'elle fonctionne : ok quand fait ca
    
    //std::ifstream ifs("../UnitTestFiles/Difference (Unl).nes", std::ios::binary);
    
    // Check that file exists
    assert(ifs.good());
    
    // Loop manager
    LoopManager loopManager(event);
    
    // Cartridge loader
    //NESEmu::Cartridge::Loader::INes inesLoader;//TODO: a la place d'avoir plusieurs loaders qui vont créer la cartridge (ines, ines2, tnes, ...), avoir un seul CartridgeFactory qui crée les cartridges (ne les crée meme pas, on fait cartridgeFactory.insertCartridgeIntoNes(Nes, std::istream) et les loaders vont seulement retourner une information sur la rom (num de mapper, nombre de banque, la mémoire des banques, ...) donc le cartridgeFactory ne devra pas avoir de methodes virtuelles et donc pas de soucis d'avoir la methode template et donc plus de redondance de template parameter comme cette solution que j'ai pour l'instant
    // TODO: et pour que le factory choisisse correctement le loader, il y a des priorités entre loader + une methode qui dit s'il supporte la rom ou non (bool isSupported(std::istream) const et int getPriority() const)
    // TODO: on peut combiner les 2 solutions (ce que j'ai mis en comm au dessus avec la solution deja faite pour le coté d'inserer des cartridge), on aurait juste plus besoin de specifier les template parameter dans le loader car ce serait une methode template (mais a voir car comment faire si Cartridge n'a plus les template parameters ?)
    
    // TODO: comme les mappers sont resolus au compile-time, a chaque mapper ajouté dans le code, il faut l'instantier (dans Factory) et donc il va avoir une duplication de NesImplementation pour chaque mapper, ca va augmenter la taille du code a fond et peut etre foutre la merde dans l'instruction cache ? si ca tombe la version avec virtual dispatch (runtime) sera au final plus rapide car moins de code meme si les appels de methodes sont indirects !!! A TESTER : ok mapper au runtime
    
    using Nes = NESEmu::Nes<NESEmu::Model::Ntsc, GraphicManager<TimeManager>, SoundManager>;
    
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
    Nes nes(graphicManager, soundManager);
    
    // Insert cartridge
    nes.insertCartridge(std::move(cartridge));
    
    // Connect controller
    nes.connectController(0, std::move(standardController));
    nes.connectController(1, std::move(zapperController));
    
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
