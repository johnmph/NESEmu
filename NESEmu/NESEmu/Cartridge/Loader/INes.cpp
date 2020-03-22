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
        
        // Read flags
        char flags[16];
        istream.read(flags, sizeof(flags));
        
        // Get persistent memory flag
        data.hasPersistentMemory = (flags[6] & 0x2) != 0;
        
        // Get trainer flag
        bool hasTrainer = (flags[6] & 0x4) != 0;
        
        // Get mirroring type
        data.mirroringType = ((flags[6] & 0x8) != 0) ? MirroringType::FourScreen : (((flags[6] & 0x1) != 0) ? MirroringType::Vertical : MirroringType::Horizontal);
        
        // Get mapper model
        data.mapperModel = mapperModels[(flags[7] & 0xF0) | (flags[6] >> 4)];//TODO: gerer si le mapper n'est pas dans l'array ! : pas besoin si on les mets tous dans l'array (256)
        
        // Exit header and trainer if present
        istream.seekg(16 + ((hasTrainer) ? 512 : 0), std::ios::seekdir::beg);
        
        // Read prg-rom
        unsigned int prgRomSize = flags[4] * 16 * 1024;
        data.prgRom.resize(prgRomSize);
        istream.read(reinterpret_cast<char *>(data.prgRom.data()), prgRomSize);
        
        // Get hasPrgRam flag
        bool hasPrgRam = (flags[10] & 0x10) == 0;
        
        // If has prg-ram, flags8 may contain prg-ram size (if 0, we assume there is 8kb)
        data.prgRamSize = (hasPrgRam) ? (((flags[8] > 0) ? flags[8] : 1) * (8 * 1024)) : 0;
        
        // Read chr-rom if present
        if (flags[5] > 0) {
            unsigned int chrRomSize = flags[5] * 8 * 1024;
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
