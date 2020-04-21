//
//  Mapper5.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 20/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper5_hpp
#define NESEmu_Cartridge_Mapper5_hpp

#include <vector>
#include "Interface.hpp"
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/MMC5

namespace NESEmu { namespace Cartridge { namespace Mapper5 {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Chip : Interface<TCpuHardwareInterface, TPpuHardwareInterface> {
        
        Chip(std::vector<uint8_t> prgRom, std::size_t prgRamSize, std::vector<uint8_t> chrRom, std::size_t chrRamSize);
        
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
        
        bool const _prgRamHasTwoChips;
        
        std::vector<uint8_t> _ram;
        uint8_t _prgMode;
        uint8_t _chrMode;
        bool _ppu2000Bit5;
        bool _ppu2001Bit3;
        bool _ppu2001Bit4;
        uint8_t _prgRamProtect;
        uint8_t _extendedRamMode;
        uint8_t _nametableMapping;
        uint8_t _fillModeTile;
        uint8_t _fillModeColor;
        uint8_t _prgBankSwitch[5];
        uint8_t _chrBankSwitch[12];
        uint8_t _upperChrBankBits;
        uint8_t _verticalSplitMode;
        uint8_t _verticalSplitScroll;
        uint8_t _verticalSplitBank;
        uint8_t _scanlineIrqCompareValue;
        bool _scanlineIrqEnable;
        bool _scanlineIrqPending;
        bool _inFrame;
        uint8_t _8BitMultiplicand;
        uint8_t _8BitMultiplier;
        
        // TODO: il reste le timer, la detection d'irq, ...
    };
    
    #include "Mapper5_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Mapper5_hpp */
