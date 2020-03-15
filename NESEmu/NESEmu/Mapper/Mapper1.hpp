//
//  Mapper1.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Mapper1_hpp
#define NESEmu_Mapper_Mapper1_hpp

// See https://wiki.nesdev.com/w/index.php/MMC1

#include <istream>
#include "Common.hpp"

// TODO: a faire :
namespace NESEmu { namespace Mapper { namespace Mapper1 {
    
    enum class Model {
        SNROM,
        SOROM,
        SUROM,
        SXROM,
        SZROM
    };
    
    struct MMC1 {
        
        MMC1();
        
        bool isPrgRamReadEnabled() const;
        bool isPrgRamWriteEnabled() const;
        std::size_t getPrgRomAddress(uint16_t address, std::size_t size) const;
        std::size_t getPrgRamAddress(uint16_t address, std::size_t size) const;
        std::size_t getChrRomAddress(uint16_t address, std::size_t size) const;
        std::size_t getChrRamAddress(uint16_t address, std::size_t size) const;
        uint16_t getMirroredVRamAddress(uint16_t address) const;
        
        // Cpu memory bus
        template <class TConnectedBus>
        void cpuWritePerformed(MMC1 const &mmc, TConnectedBus &connectedBus);
        
    private:
        uint8_t _shiftRegister;
        uint8_t _shiftCount;
        uint8_t _internalRegisters[4];
    };
    
    
    template <Model EModel>
    struct Chip {
        
        Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> prgRam);
        
        template <class TConnectedBus, class TInterruptHardware>
        void clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware);
        
        // Cpu memory bus
        template <class TConnectedBus>
        void cpuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void cpuWritePerformed(TConnectedBus &connectedBus);
        
        // Ppu memory bus
        template <class TConnectedBus>
        void ppuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void ppuWritePerformed(TConnectedBus &connectedBus);
        
    private:
        uint16_t getChrRamAddress(uint16_t address);
        uint16_t getVramAddress(uint16_t address);
        
        std::vector<uint8_t> _prgRom;
        std::vector<uint8_t> _prgRam;
        std::vector<uint8_t> _chrRam;
        
        unsigned int _prgRomSize;
        unsigned int _prgRamSize;
        
        uint8_t _shiftRegister;
        uint8_t _shiftCount;
        uint8_t _internalRegisters[4];
    };
    
    #include "Mapper1_s.hpp"
    
} } }

#endif /* NESEmu_Mapper_Mapper1_hpp */
