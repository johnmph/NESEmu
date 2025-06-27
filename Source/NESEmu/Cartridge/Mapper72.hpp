//
//  Mapper72.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 27/06/25.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper72_hpp
#define NESEmu_Cartridge_Mapper72_hpp

#include <vector>
#include "Interface.hpp"
#include "Common.hpp"

// See https://www.nesdev.org/wiki/INES_Mapper_072

namespace NESEmu { namespace Cartridge { namespace Mapper72 {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Chip : Interface<TCpuHardwareInterface, TPpuHardwareInterface> {
        
        Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> chrRom, MirroringType mirroringType);
        
        // Cpu memory bus
        void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        
        // Ppu memory bus
        void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        
    private:
        uint16_t getVramAddress(uint16_t address);

        MirroringType const _mirroringType;
        
        uint8_t _prgRomBankSelect;
        uint8_t _chrRomBankSelect;
        uint8_t _latch;
    };
    
    #include "Mapper72_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Mapper72_hpp */
