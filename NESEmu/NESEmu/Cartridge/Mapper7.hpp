//
//  Mapper7.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper7_hpp
#define NESEmu_Cartridge_Mapper7_hpp

#include <vector>
#include "Interface.hpp"
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/AxROM
// TODO: par apres, factoriser le code commun des mappers
namespace NESEmu { namespace Cartridge { namespace Mapper7 {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Chip : Interface<TCpuHardwareInterface, TPpuHardwareInterface> {
        
        Chip(std::vector<uint8_t> prgRom, std::size_t chrRamSize);
        
        // Cpu memory bus
        void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        
        // Ppu memory bus
        void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        
    private:
        std::vector<uint8_t> const _prgRom;
        std::vector<uint8_t> _chrRam;
        std::size_t const _prgRomSize;
        std::size_t const _chrRamSize;
        
        uint8_t _prgRomBankSelect;
        uint8_t _vramBankSelect;
    };
    
    #include "Mapper7_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Mapper7_hpp */
