//
//  Nes.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 21/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Nes_hpp
#define NESEmu_Nes_hpp

#include <cstdint>
#include <vector>
#include "Cpu.hpp"
#include "Ppu.hpp"


namespace NESEmu {
    
    enum class Model {
        Ntsc,
        Pal
    };
    
    template <Model EModel>
    struct Constants;
    
    template <Model EModel>
    struct Nes {
        
        Nes();
        
        void clock();
        
        void reset(bool high);
        
    private:
        
        using Constants = Constants<EModel>;
        
        // Set Cpu as friend to keep data bus methods private
        friend Cpu::Chip<Constants::cpuModel, Nes>;
        
        // Set Ppu as friend to keep interrupt method private
        friend Ppu::Chip<Constants::ppuModel, Nes, Nes, Nes>;
        
        // Memory bus
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        
        // PPU interrupt hardware
        void ppuInterrupt(bool high);
        
        // Chips
        Cpu::Chip<Constants::cpuModel, Nes> _cpu;
        Ppu::Chip<Constants::ppuModel, Nes, Nes, Nes> _ppu;     // TODO: changer par apres, ca doit etre cartridge pour le 2eme parametre et pour le dernier ca doit etre la classe qui gerera le rendu
        
        // 2kb of RAM
        std::vector<uint8_t> _ram;
        
        // Internals
        int _currentClockForCpu;
        int _currentClockForPpu;
        
        // TODO: il faut aussi une classe Cartridge qui prendra en compte les mappers, et une classe CartridgeLoader qui convertira une rom en cartridge (qui aura le bon mapper) (peut etre meme que CartridgeLoader retournera une Nes complete pour pouvoir avoir les read/write au compile time plutot qu'en polymorphisme dynamique (voir EmulatorDesign)
        // TODO: pour les joysticks, avoir des methodes pour "brancher" un joystick, qui communiquera via les OUT0/2 OE1 et OE2 du CPU
    };
    
    #include "Nes_s.hpp"
    
}

#endif /* NESEmu_Nes_hpp */
