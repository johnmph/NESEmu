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
#include <memory>
#include "Cpu.hpp"
#include "Ppu.hpp"
#include "Controller/Nothing.hpp"


namespace NESEmu {
    
    enum class Model {
        Ntsc,
        Pal
    };
    
    template <Model EModel>
    struct Constants;
    
    template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
    struct Nes {
        
        Nes(TCartridgeHardware &cartridgeHardware, TGraphicHardware &graphicHardware);
        
        void powerUp();
        
        void clock();
        
        void reset(bool high);
        
        void connectController(unsigned int portNumber, std::unique_ptr<Controller::Interface> controller);
        
    private:
        
        struct CpuBus {
            CpuBus(Nes &nes);
            
            uint16_t getAddressBus() const;
            void setAddressBus(uint16_t address);
            
            uint8_t getDataBus() const;
            void setDataBus(uint8_t data);
            void setDataBus(uint8_t data, uint8_t mask);
            
            void performRead();
            void performWrite();
            
            void readControllerPort(unsigned int number);
            
        private:
            Nes &_nes;
            
            uint16_t _address;
            uint8_t _data;
        };
        
        struct PpuBus {
            PpuBus(Nes &nes);
            
            uint16_t getAddressBus() const;
            void setAddressBus(uint16_t address);
            
            uint8_t getDataBus() const;
            void setDataBus(uint8_t data);
            void setDataBus(uint8_t data, uint8_t mask);
            
            void performRead();
            void performWrite();
            
            std::vector<uint8_t> &getVram();
            
            void interrupt(bool high);
            
        private:
            Nes &_nes;
            
            uint16_t _address;
            // Data bus not really exists on the PPU but on a external octal latch
            uint8_t _externalOctalLatch;
        };
        
        
        using Constants = Constants<EModel>;
        
        // Set Cpu as friend to keep data bus methods private
        //friend Cpu::Chip<Constants::cpuModel, Nes>;
        
        // Set Ppu as friend to keep interrupt method private
        //friend Ppu::Chip<Constants::ppuModel, Nes, Nes, Nes>;
        
        // CPU memory bus
        //uint8_t cpuRead(uint16_t address);
        //void cpuWrite(uint16_t address, uint8_t data);
        
        // PPU memory bus
        uint8_t ppuRead(uint16_t address);
        void ppuWrite(uint16_t address, uint8_t data);
        
        // PPU interrupt hardware
        void ppuInterrupt(bool high);
        
        // Chips
        Cpu::Chip<Constants::cpuModel, CpuBus> _cpu;
        Ppu::Chip<Constants::ppuModel, PpuBus, PpuBus, TGraphicHardware> _ppu;     // TODO: changer par apres, ca doit etre cartridge pour le 2eme parametre et pour le dernier ca doit etre la classe qui gerera le rendu
        
        // 2kb of RAM
        std::vector<uint8_t> _ram;
        
        // 2kb of VRAM
        std::vector<uint8_t> _vram;
        
        // Controllers
        std::unique_ptr<Controller::Interface> _controllerPorts[2];    // TODO: 2 controllers ports, voir si mettre 2 dans un const
        
        // Internals
        TCartridgeHardware &_cartridgeHardware;
        CpuBus _cpuBus;
        PpuBus _ppuBus;
        
        int _currentClockForCpu;
        int _currentClockForPpu;
        
        // TODO: il faut aussi une classe Cartridge qui prendra en compte les mappers, et une classe CartridgeLoader qui convertira une rom en cartridge (qui aura le bon mapper) (peut etre meme que CartridgeLoader retournera une Nes complete pour pouvoir avoir les read/write au compile time plutot qu'en polymorphisme dynamique (voir EmulatorDesign)
        // TODO: pour les joysticks, avoir des methodes pour "brancher" un joystick, qui communiquera via les OUT0/2 OE1 et OE2 du CPU
    };
    
    #include "Nes_s.hpp"
    
}

#endif /* NESEmu_Nes_hpp */
