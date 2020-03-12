//
//  Mapper2.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Mapper2_hpp
#define NESEmu_Mapper_Mapper2_hpp

#include <istream>
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/UxROM

namespace NESEmu { namespace Mapper {
    
    template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
    struct Mapper2 {
        
        Mapper2(std::istream &istream);
        
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
        std::vector<uint8_t> _prgRom;
        std::vector<uint8_t> _prgRam;
        std::vector<uint8_t> _chrRom;
        
        uint8_t _prgRomBankSelect;
    };
    
    #include "Mapper2_s.hpp"
    
} }

#endif /* NESEmu_Mapper_Mapper2_hpp */
