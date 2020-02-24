//
//  Cartridge.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper0_hpp
#define NESEmu_Cartridge_Mapper0_hpp

#include <cstdint>
#include <istream>


namespace NESEmu { namespace Cartridge {
    
    enum class MirroringType {  // TODO: taper ca dans un Cartridge.hpp ? avec en plus SingleScreen et FourScreen ?
        Horizontal,
        Vertical
    };
    
    // TODO: rajouter des template parameters pour la taille PRG-ROM, PRG-RAM (battery backup) et le type de nametable mirroring
    template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
    struct Mapper0 {    // TODO: comment avoir acces au 2k de vram de nes ici ? + Il faut la possibilité d'envoyer un irq sur le cpu + il faut avoir acces au bus !!! gros probleme : le bus est Nes mais Nes a son parametre template qui est cette classe je ne peux pas donc passer Nes comme parametre template a cette classe (cyclic dependency)
        
        Mapper0(std::istream &istream);
        
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
    };
    
    #include "Mapper0_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Mapper0_hpp */
