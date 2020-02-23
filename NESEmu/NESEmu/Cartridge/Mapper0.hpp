//
//  Cartridge.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper0_hpp
#define NESEmu_Cartridge_Mapper0_hpp


namespace NESEmu { namespace Cartridge {
    
    // TODO: rajouter des template parameters pour la taille PRG-ROM, PRG-RAM (battery backup) et le type de nametable mirroring
    struct Mapper0 {    // TODO: comment avoir acces au 2k de vram de nes ici ? + Il faut la possibilité d'envoyer un irq sur le cpu
        
        // Cpu memory bus
        uint8_t cpuRead(uint16_t address);
        void cpuWrite(uint16_t address, uint8_t data);
        
        // Ppu memory bus
        uint8_t ppuRead(uint16_t address);
        void ppuWrite(uint16_t address, uint8_t data);
        
    };
    
} }

#endif /* NESEmu_Cartridge_Mapper0_hpp */
