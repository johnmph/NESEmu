//
//  Mapper1.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper1_hpp
#define NESEmu_Cartridge_Mapper1_hpp

// See https://wiki.nesdev.com/w/index.php/MMC1

#include <istream>
#include "Common.hpp"

// TODO: a faire :
namespace NESEmu { namespace Cartridge {
    
    template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb>
    struct Mapper1 {
        
        Mapper1(std::istream &istream);
        
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
        
        uint8_t _shiftRegister;
        uint8_t _shiftCount;
        uint8_t _internalRegisters[4];
    };
    
    #include "Mapper1_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Mapper1_hpp */
