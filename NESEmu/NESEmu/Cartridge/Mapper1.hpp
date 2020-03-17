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

#include <vector>
#include "Interface.hpp"
#include "Common.hpp"


namespace NESEmu { namespace Cartridge { namespace Mapper1 {
    
    enum class Model {
        SNROM,
        SOROM,
        SUROM,
        SXROM,
        SZROM
    };
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface, Model EModel>
    struct Chip : Interface<TCpuHardwareInterface, TPpuHardwareInterface> {
        
        Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> prgRam, std::vector<uint8_t> chrRom, std::size_t chrRamSize);
        
        // Cpu memory bus
        void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        
        // Ppu memory bus
        void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        
    private:
        std::size_t getChrRamAddress(uint16_t address);
        uint16_t getVramAddress(uint16_t address);
        
        std::vector<uint8_t> const _prgRom;
        std::vector<uint8_t> _prgRam;
        std::vector<uint8_t> const _chrRom;
        std::vector<uint8_t> _chrRam;
        std::size_t const _prgRomSize;
        std::size_t const _prgRamSize;
        bool const _hasChrRam;
        std::size_t const _chrRomOrRamSize;
        
        uint8_t _shiftRegister;
        uint8_t _shiftCount;
        uint8_t _internalRegisters[4];
        bool _lastCycleWasWrite;
    };
    
    #include "Mapper1_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Mapper1_hpp */
