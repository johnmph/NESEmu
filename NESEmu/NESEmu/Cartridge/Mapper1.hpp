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

#include <istream>
#include "Common.hpp"

// TODO: a faire :
namespace NESEmu { namespace Cartridge {
    
    template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>//TODO: comment limiter le mirroring a H et V seulement ?
    struct Mapper1 {    // TODO: comment avoir acces au 2k de vram de nes ici ? + Il faut la possibilité d'envoyer un irq sur le cpu + il faut avoir acces au bus !!! gros probleme : le bus est Nes mais Nes a son parametre template qui est cette classe je ne peux pas donc passer Nes comme parametre template a cette classe (cyclic dependency) : voir si ok avec ce systeme (template sur methodes)
        
        Mapper1(std::istream &istream);
        
        // Cpu memory bus
        template <class TConnectedBus>
        void cpuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void cpuWritePerformed(TConnectedBus &connectedBus);
        
        // Ppu memory bus
        template <class TConnectedBus>
        void ppuAddressBusChanged(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void ppuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void ppuWritePerformed(TConnectedBus &connectedBus);
        
    private:
        std::vector<uint8_t> _prgRom;
        std::vector<uint8_t> _prgRam;
        std::vector<uint8_t> _chrRom;
    };
    
    #include "Mapper1_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Mapper1_hpp */
