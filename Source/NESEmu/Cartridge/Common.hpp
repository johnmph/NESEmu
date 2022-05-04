//
//  Common.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Common_hpp
#define NESEmu_Cartridge_Common_hpp

// See https://wiki.nesdev.com/w/index.php/CPU_memory_map
// See https://wiki.nesdev.com/w/index.php/PPU_memory_map
// See https://wiki.nesdev.com/w/index.php/Cartridge_connector

#include <cstdint>
#include <vector>


namespace NESEmu { namespace Cartridge {
    
    // See https://wiki.nesdev.com/w/index.php/Mirroring
    enum class MirroringType {
        Horizontal,
        Vertical,
        SingleScreen,
        FourScreen
    };
    
    template <MirroringType EMirroring>
    uint16_t getMirroredAddress(uint16_t address);
    
    // Function template explicit instantiation of the specialization
    template <>
    uint16_t getMirroredAddress<MirroringType::Horizontal>(uint16_t address);
    
    template <>
    uint16_t getMirroredAddress<MirroringType::Vertical>(uint16_t address);
    
    template <>
    uint16_t getMirroredAddress<MirroringType::SingleScreen>(uint16_t address);
    
    template <>
    uint16_t getMirroredAddress<MirroringType::FourScreen>(uint16_t address);
    
} }

#endif /* NESEmu_Cartridge_Common_hpp */
