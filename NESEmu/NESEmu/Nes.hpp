//
//  Nes.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 10/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Nes_hpp
#define NESEmu_Nes_hpp

//TODO: voir pour noexcept sur les methodes
#include <cstdint>
#include <vector>
#include <memory>
#include "Cpu.hpp"
#include "Ppu.hpp"
#include "Cartridge/Nothing.hpp"
#include "Controller/Nothing.hpp"


namespace NESEmu {
    
    enum class Model {
        Ntsc,
        Pal
    };
    
}

namespace NESEmu {
    
    template <Model EModel>
    struct Constants;
    
    template <Model EModel, class TGraphicHardware, class TLoopManager>
    struct Nes {
        
    private:
        
        struct CpuHardwareInterface {
            CpuHardwareInterface(Nes &nes);
            
            uint16_t getAddressBus() const;
            void setAddressBus(uint16_t address);
            
            uint8_t getDataBus() const;
            void setDataBus(uint8_t data);
            void setDataBus(uint8_t data, uint8_t mask);
            
            void performRead();
            void performWrite();
            
            void readControllerPort(unsigned int number);
            
            void irq(bool high);
            
        private:
            Nes &_nes;
            
            uint16_t _address;
            uint8_t _data;
        };
        
        struct PpuHardwareInterface {
            PpuHardwareInterface(Nes &nes);
            
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
        
    public:
        
        // Keep Hardware interface classes private but allow external code to get their types for Cartridge::Interface type
        using CpuHardwareInterface = CpuHardwareInterface;//TODO: voir si ok ce genre de code
        using PpuHardwareInterface = PpuHardwareInterface;
        
        
        Nes(TGraphicHardware &graphicHardware, TLoopManager &loopManager);
        
        void powerUp();
        
        void reset(bool high);  // TODO: a la place d'avoir ca, peut etre avoir un template parameter TResetHardware qui aura une methode qui retourne un bool pour le reset (pour mieux decoupler ?), a voir (ca simule le fait d'avoir le bouton reset sur la Nes), ou peut etre laisser ainsi
        
        void connectController(unsigned int portNumber, std::unique_ptr<Controller::Interface> controller);
        std::unique_ptr<Controller::Interface> disconnectController(unsigned int portNumber);
        
        void insertCartridge(std::unique_ptr<Cartridge::Interface<CpuHardwareInterface, PpuHardwareInterface>> cartridge);
        std::unique_ptr<Cartridge::Interface<CpuHardwareInterface, PpuHardwareInterface>> removeCartridge();
        
        void run();
        
        void clock();
        void clockFull();
        
    private:
        
        using Constants = Constants<EModel>;
        
        // PPU memory bus
        uint8_t ppuRead(uint16_t address);
        void ppuWrite(uint16_t address, uint8_t data);
        
        // PPU interrupt hardware
        void ppuInterrupt(bool high);
        
        // Cartridge interrupt hardware
        void cartridgeInterrupt(bool high);
        
        // Chips
        Cpu::Chip<Constants::cpuModel, CpuHardwareInterface> _cpu;
        Ppu::Chip<Constants::ppuModel, PpuHardwareInterface, PpuHardwareInterface, TGraphicHardware> _ppu;
        
        // 2kb of RAM
        std::vector<uint8_t> _ram;
        
        // 2kb of VRAM
        std::vector<uint8_t> _vram;
        
        // Controllers
        std::unique_ptr<Controller::Interface> _controllerPorts[2];    // TODO: 2 controllers ports, voir si mettre 2 dans un const
        
        // Internals
        std::unique_ptr<Cartridge::Interface<CpuHardwareInterface, PpuHardwareInterface>> _cartridge;
        TLoopManager &_loopManager;
        CpuHardwareInterface _cpuHardwareInterface;
        PpuHardwareInterface _ppuHardwareInterface;
        
        int _currentClockForCpu;
        int _currentClockForPpu;
        
        // TODO: pour les joysticks, avoir des methodes pour "brancher" un joystick, qui communiquera via les OUT0/2 OE1 et OE2 du CPU
    };
    
    #include "Nes_s.hpp"
    
}

#endif /* NESEmu_Nes_hpp */
