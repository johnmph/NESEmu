//
//  INes2.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include <cstring>
#include "INes2.hpp"


namespace NESEmu { namespace Cartridge { namespace Loader {
    
    bool INes2::isCartridgeSupported(std::istream &istream) {
        // INes format start by "NES\x1A" and byte 7 has bit 2 == 0 and bit 3 == 1
        uint8_t flags[8];
        istream.read(reinterpret_cast<char *>(flags), sizeof(flags));
        
        return (std::memcmp(flags, "NES\x1A", 4) == 0) && ((flags[7] & 0xC) == 0x8);
    }
    
    Data INes2::getCartridgeDataFromStream(std::istream &istream) {
        Data data;
        
        // Read flags
        uint8_t flags[16];
        istream.read(reinterpret_cast<char *>(flags), sizeof(flags));
        
        // Get persistent memory flag
        data.hasPersistentMemory = (flags[6] & 0x2) != 0;
        
        // Get trainer flag
        bool hasTrainer = (flags[6] & 0x4) != 0;
        
        // Get mirroring type
        data.mirroringType = ((flags[6] & 0x8) != 0) ? MirroringType::FourScreen : (((flags[6] & 0x1) != 0) ? MirroringType::Vertical : MirroringType::Horizontal);
        
        // Get mapper model
        data.mapperModel = static_cast<Model>(((flags[8] & 0xF) << 8) | (flags[7] & 0xF0) | (flags[6] >> 4));

        // TODO: le submapper number
        data.submapperNumber = flags[8] >> 4;
        
        // Exit header and trainer if present
        istream.seekg(16 + ((hasTrainer) ? 512 : 0), std::ios::beg);
        
        // Read prg-rom
        std::size_t prgRomSize = getPrgRomSize(flags[4], flags[9] & 0xF);
        data.prgRom.resize(prgRomSize);
        istream.read(reinterpret_cast<char *>(data.prgRom.data()), prgRomSize);
        
        // If has prg-ram, get its size
        data.prgRamSize = (flags[10] != 0x0) ? (64 << ((flags[10] > 0xF) ? (flags[10] >> 4) : (flags[10] & 0xF))) : 0;
        
        // Read chr-rom if present
        std::size_t chrRomSize = getChrRomSize(flags[5], flags[9] >> 4);
        if (chrRomSize > 0) {
            data.chrRom.resize(chrRomSize);
            istream.read(reinterpret_cast<char *>(data.chrRom.data()), chrRomSize);
        }

        // If has chr-ram, get its size
        data.chrRamSize = (flags[11] != 0x0) ? (64 << ((flags[11] > 0xF) ? (flags[11] >> 4) : (flags[11] & 0xF))) : 0;

        // TODO: voir le reste
        
        return data;
    }

    std::size_t INes2::getPrgRomSize(uint8_t lsb, uint8_t msb) {
        return getRomSize(lsb, msb, 16 * 1024);
    }

    std::size_t INes2::getChrRomSize(uint8_t lsb, uint8_t msb) {
        return getRomSize(lsb, msb, 8 * 1024);
    }

    std::size_t INes2::getRomSize(uint8_t lsb, uint8_t msb, unsigned int bankSize) {
        // Exponent notation
        if (msb == 0xF) {
            uint8_t exponent = lsb >> 2;
            uint8_t multiplier = ((lsb & 0x3) << 1) + 1;

            return (1 << exponent) * multiplier;
        }

        // Normal notation
        return ((msb << 8) | lsb) * bankSize;
    }
    
} } }
