//
//  Mapper3.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 25/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper3_hpp
#define NESEmu_Cartridge_Mapper3_hpp

#include <istream>
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/INES_Mapper_003

namespace NESEmu { namespace Cartridge {
    
    template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
    struct Mapper3 {
        
        Mapper3(std::istream &istream);
        
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
        std::vector<uint8_t> _prgRom;
        std::vector<uint8_t> _prgRam;
        std::vector<uint8_t> _chrRom;
        uint8_t _bankSelect;
    };
    
    #include "Mapper3_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Mapper3_hpp */
