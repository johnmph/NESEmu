//
//  ArchaicINes.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include <cstring>
#include "ArchaicINes.hpp"


namespace NESEmu { namespace Cartridge { namespace Loader {
    
    Model ArchaicINes::mapperModels[] = {
        Model::NROM,
        Model::MMC1,
        Model::UxROM,
        Model::CNROM,
        Model::MMC3,
        Model::MMC5,
        Model::FFE,
        Model::AxROM,
        Model::M008,
        Model::MMC2,
        Model::MMC4
        //TODO: continuer
    };
    
    bool ArchaicINes::isCartridgeSupported(std::istream &istream) {
        // INes format start by "NES\x1A"
        uint8_t flags[4];
        istream.read(reinterpret_cast<char *>(flags), sizeof(flags));
        
        return (std::memcmp(flags, "NES\x1A", 4) == 0);
    }
    
    Data ArchaicINes::getCartridgeDataFromStream(std::istream &istream) {
        Data data;
        
        // Read flags
        uint8_t flags[7];
        istream.read(reinterpret_cast<char *>(flags), sizeof(flags));
        
        // Get persistent memory flag
        data.hasPersistentMemory = (flags[6] & 0x2) != 0;
        
        // Get trainer flag
        bool hasTrainer = (flags[6] & 0x4) != 0;
        
        // Get mirroring type
        data.mirroringType = ((flags[6] & 0x8) != 0) ? MirroringType::FourScreen : (((flags[6] & 0x1) != 0) ? MirroringType::Vertical : MirroringType::Horizontal);
        
        // Get mapper model
        data.mapperModel = mapperModels[flags[6] >> 4];//TODO: gerer si le mapper n'est pas dans l'array ! : pas besoin si on les mets tous dans l'array (16)

        // No submapper number
        data.submapperNumber = 0;
        
        // Exit header and trainer if present
        istream.seekg(16 + ((hasTrainer) ? 512 : 0), std::ios::beg);
        
        // Read prg-rom
        std::size_t prgRomSize = flags[4] * 16 * 1024;
        data.prgRom.resize(prgRomSize);
        istream.read(reinterpret_cast<char *>(data.prgRom.data()), prgRomSize);
        
        // There is no information about size of prg-ram size in ArchaicINes so we assume that it is 8kb
        data.prgRamSize = 8 * 1024;     // TODO: voir si ca ou 0
        
        // Read chr-rom if present
        if (flags[5] > 0) {
            std::size_t chrRomSize = flags[5] * 8 * 1024;
            data.chrRom.resize(chrRomSize);
            istream.read(reinterpret_cast<char *>(data.chrRom.data()), chrRomSize);
            
            // No chr-ram if chr-rom
            data.chrRamSize = 0;
        } else {
            // There is no information about size of chr-ram size in ArchaicINes so we assume that it is 8kb
            data.chrRamSize = 8 * 1024;
        }
        
        return data;
    }
    
} } }
