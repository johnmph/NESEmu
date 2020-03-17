//
//  Mapper2.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper2_hpp
#define NESEmu_Cartridge_Mapper2_hpp

#include <vector>
#include "Interface.hpp"
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/UxROM

namespace NESEmu { namespace Cartridge { namespace Mapper2 {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Chip : Interface<TCpuHardwareInterface, TPpuHardwareInterface> {
        
        Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> prgRam, std::vector<uint8_t> chrRom, std::size_t chrRamSize, MirroringType mirroringType);
        
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
        std::vector<uint8_t> _chrRam;
        std::size_t const _prgRomSize;
        std::size_t const _prgRamSize;
        bool const _hasChrRam;
        std::size_t const _chrRomOrRamSize;
        MirroringType const _mirroringType;
        
        uint8_t _prgRomBankSelect;
    };
    
    #include "Mapper2_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Mapper2_hpp */
