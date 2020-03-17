//
//  Interface.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Interface_hpp
#define NESEmu_Cartridge_Interface_hpp

#include <cstdint>
#include <vector>


namespace NESEmu { namespace Cartridge {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Interface {
        
        Interface(std::vector<uint8_t> prgRom, std::size_t prgRamSize, std::vector<uint8_t> chrRom, std::size_t chrRamSize);
        virtual ~Interface() = 0;
        
        // Prg-ram
        std::vector<uint8_t> &getPrgRam();
        
        // Clock
        virtual void clock(TCpuHardwareInterface &cpuHardwareInterface, TPpuHardwareInterface &ppuHardwareInterface);
        
        // Cpu memory bus
        virtual void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) = 0;
        virtual void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) = 0;
        
        // Ppu memory bus
        virtual void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) = 0;
        virtual void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) = 0;
        
    protected:
        std::vector<uint8_t> const _prgRom;
        std::vector<uint8_t> _prgRam;
        std::vector<uint8_t> const _chrRom;
        std::vector<uint8_t> _chrRam;
        std::size_t const _prgRomSize;
        std::size_t const _prgRamSize;
        std::size_t const _chrRomSize;
        std::size_t const _chrRamSize;
    };
    
    #include "Interface_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Interface_hpp */
