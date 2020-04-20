//
//  Mapper10.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 19/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper10_hpp
#define NESEmu_Cartridge_Mapper10_hpp

#include <vector>
#include "Interface.hpp"
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/MMC4

namespace NESEmu { namespace Cartridge { namespace Mapper10 {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Chip : Interface<TCpuHardwareInterface, TPpuHardwareInterface> {
        
        Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> chrRom);
        
        // Cpu memory bus
        void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        
        // Ppu memory bus
        void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        
    private:
        uint16_t getVramAddress(uint16_t address);
        
        uint8_t _prgRomBankSelect;
        uint8_t _chrRomBankSelect[4];
        bool _chrRomLatch[2];
        MirroringType _mirroringType;
    };
    
    #include "Mapper10_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Mapper10_hpp */
