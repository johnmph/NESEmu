//
//  INes.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include <cstring>
#include "INes.hpp"


namespace NESEmu { namespace Cartridge { namespace Loader {
    
    static Model mapperModels[] = {
        Model::NROM,
        Model::MMC1,
        Model::UxROM,
        Model::CNROM,
        Model::MMC3,
        Model::MMC5,
        Model::FFE,
        Model::AxROM
        //TODO: continuer
    };
    
    bool INes::isCartridgeSupported(std::istream &istream) {
        // INes format start by "NES\x1A"
        char signature[4];
        istream.read(signature, sizeof(signature));
        
        return std::memcmp(signature, "NES\x1A", sizeof(signature)) == 0;
    }
    
    Data INes::getCartridgeDataFromStream(std::istream &istream) {
        Data data;
        
        // Skip signature
        istream.seekg(4);
        
        // Read prg-rom size (16KB units)
        uint8_t prgRomSizeIn16Kb = istream.get();
        
        // Read chr-rom size (8KB units)
        uint8_t chrRomSizeIn8Kb = istream.get();
        
        // Read Flags 6
        uint8_t flags6 = istream.get();
        
        // Read Flags 7
        uint8_t flags7 = istream.get();
        
        // Read Flags 8
        uint8_t flags8 = istream.get();
        
        // Get prg-ram flag
        bool hasPrgRam = flags6 & 0x2;
        
        // Get trainer flag
        bool hasTrainer = flags6 & 0x4;
        
        // Get mirroring type
        data.mirroringType = ((flags6 & 0x8) != 0) ? MirroringType::FourScreen : (((flags6 & 0x1) != 0) ? MirroringType::Vertical : MirroringType::Horizontal);
        
        // Get mapper model
        data.mapperModel = mapperModels[(flags7 & 0xF0) | (flags6 >> 4)];//TODO: gerer si le mapper n'est pas dans l'array !
        
        // Exit header and trainer if present
        istream.seekg(16 + ((hasTrainer) ? 512 : 0), std::ios::seekdir::beg);
        
        // Read prg-rom
        unsigned int prgRomSize = prgRomSizeIn16Kb * 16 * 1024;
        data.prgRom.resize(prgRomSize);
        istream.read(reinterpret_cast<char *>(data.prgRom.data()), prgRomSize);
        
        // If has prg-ram, flags8 may contain prg-ram size (if 0, we assume there is 8kb)
        data.prgRamSize = (hasPrgRam) ? (((flags8 > 0) ? flags8 : 1) * (8 * 1024)) : 0;
        
        // Read chr-rom if present
        if (chrRomSizeIn8Kb > 0) {
            unsigned int chrRomSize = chrRomSizeIn8Kb * 8 * 1024;
            data.chrRom.resize(chrRomSize);
            istream.read(reinterpret_cast<char *>(data.chrRom.data()), chrRomSize);
            
            // No chr-ram if chr-rom
            data.chrRamSize = 0;
        } else {
            // There is no information about size of chr-ram size in INes 1.0 so we assume that it is 8kb
            data.chrRamSize = 8 * 1024;
        }
        
        return data;
    }
    
} } }
