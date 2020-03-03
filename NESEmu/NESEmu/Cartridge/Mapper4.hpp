//
//  Mapper4.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper4_hpp
#define NESEmu_Cartridge_Mapper4_hpp

#include <istream>
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/MMC3
// See https://forums.nesdev.com/viewtopic.php?f=9&t=14072

namespace NESEmu { namespace Cartridge {
    
    template <unsigned int IPrgRomSizeInKb, unsigned int IChrRomSizeInKb>
    struct Mapper4 {
        
        Mapper4(std::istream &istream);
        
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
        
        template <class TConnectedBus>
        void processIrqCounter(TConnectedBus &connectedBus, bool a12);
        
        std::vector<uint8_t> _prgRom;
        std::vector<uint8_t> _prgRam;
        std::vector<uint8_t> _chrRom;
        
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
    
} }

#endif /* NESEmu_Cartridge_Mapper4_hpp */
