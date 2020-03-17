//
//  Mapper4.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper4_hpp
#define NESEmu_Cartridge_Mapper4_hpp

#include <vector>
#include "Interface.hpp"
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/MMC3
// See https://forums.nesdev.com/viewtopic.php?f=9&t=14072

namespace NESEmu { namespace Cartridge { namespace Mapper4 {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Chip : Interface<TCpuHardwareInterface, TPpuHardwareInterface> {
        
        Chip(std::vector<uint8_t> prgRom, std::size_t prgRamSize, std::vector<uint8_t> chrRom, MirroringType mirroringType);
        
        void clock(TCpuHardwareInterface &cpuHardwareInterface, TPpuHardwareInterface &ppuHardwareInterface) override;
        
        // Cpu memory bus
        void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        
        // Ppu memory bus
        void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        
    private:
        
        uint16_t getVramAddress(uint16_t address);
        void processIrqCounter(TCpuHardwareInterface &cpuHardwareInterface, bool a12);
        
        
        MirroringType const _mirroringType;
        
        uint8_t _bankRegisterSelect;
        bool _prgRomBankMode;
        bool _chrRomBankMode;
        uint8_t _bankSelect[8];
        bool _nametableMirroring;
        bool _prgRamWriteProtection;
        bool _prgRamChipEnable;
        uint8_t _lastA12;
        bool _irqEnable;
        bool _irqReload;
        uint8_t _irqLatch;
        uint8_t _irqCounter;
    };
    
    #include "Mapper4_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Mapper4_hpp */
