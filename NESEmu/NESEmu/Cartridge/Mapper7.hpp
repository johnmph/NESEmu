//
//  Mapper7.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper7_hpp
#define NESEmu_Cartridge_Mapper7_hpp

#include <istream>
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/NROM
// TODO: par apres, factoriser le code commun des mappers
namespace NESEmu { namespace Cartridge {
    
    template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb>
    struct Mapper7 {
        
        Mapper7(std::istream &istream);
        
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
        std::vector<uint8_t> _prgRom;
        std::vector<uint8_t> _prgRam;
        std::vector<uint8_t> _chrRom;
        uint8_t _bankSelect;
        uint8_t _vramSelect;
    };
    
#include "Mapper7_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Mapper7_hpp */
