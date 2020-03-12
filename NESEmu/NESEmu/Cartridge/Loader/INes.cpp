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
    
    static Mapper::Model mapperModels[] = {
        Mapper::Model::NROM,
        Mapper::Model::MMC1,
        Mapper::Model::UxROM,
        Mapper::Model::CNROM,
        Mapper::Model::MMC3,
        Mapper::Model::MMC5,
        Mapper::Model::FFE,
        Mapper::Model::AxROM
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
        
        // Get prg-ram flag
        bool hasPrgRam = flags6 & 0x2;//TODO: voir pour ca
        
        // Get trainer flag
        bool hasTrainer = flags6 & 0x4;
        
        // Get mirroring type
        data.mirroringType = ((flags6 & 0x8) != 0) ? Mapper::MirroringType::FourScreen : (((flags6 & 0x1) != 0) ? Mapper::MirroringType::Vertical : Mapper::MirroringType::Horizontal);
        
        // Get mapper model
        data.mapperModel = mapperModels[(flags7 & 0xF0) | (flags6 >> 4)];//TODO: gerer si le mapper n'est pas dans l'array !
        
        // Exit header and trainer if present
        istream.seekg(8 + ((hasTrainer) ? 512 : 0), std::ios::seekdir::cur);
        
        // Read prg-rom
        unsigned int prgRomSize = prgRomSizeIn16Kb * 16 * 1024;
        data.prgRom.resize(prgRomSize);
        istream.read(reinterpret_cast<char *>(data.prgRom.data()), prgRomSize);
        
        // Read chr-rom if present
        if (chrRomSizeIn8Kb > 0) {
            unsigned int chrRomSize = chrRomSizeIn8Kb * 8 * 1024;
            data.chrRom.resize(chrRomSize);
            istream.read(reinterpret_cast<char *>(data.chrRom.data()), chrRomSize);
        }
        
        return data;
    }
    
} } }
