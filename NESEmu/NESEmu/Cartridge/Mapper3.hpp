//
//  Mapper3.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 25/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper3_hpp
#define NESEmu_Cartridge_Mapper3_hpp

#include <vector>
#include "Interface.hpp"
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/INES_Mapper_003

namespace NESEmu { namespace Cartridge { namespace Mapper3 {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Chip : Interface<TCpuHardwareInterface, TPpuHardwareInterface> {
        
        Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> prgRam, std::vector<uint8_t> chrRom, MirroringType mirroringType);
        
        // Cpu memory bus
        void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        
        // Ppu memory bus
        void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        
    private:
        uint16_t getVramAddress(uint16_t address);
        
        std::vector<uint8_t> const _prgRom;
        std::vector<uint8_t> _prgRam;
        std::vector<uint8_t> const _chrRom;
        std::size_t const _prgRomSize;
        std::size_t const _prgRamSize;
        std::size_t const _chrRomSize;
        MirroringType const _mirroringType;
        
        uint8_t _chrRomBankSelect;
    };
    
    #include "Mapper3_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Mapper3_hpp */
