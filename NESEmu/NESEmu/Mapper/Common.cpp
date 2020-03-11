//
//  Common.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "Common.hpp"


namespace NESEmu { namespace Mapper {
    
    template <>
    uint16_t getMirroredAddress<MirroringType::Horizontal>(uint16_t address) {
        return ((address & 0x800) >> 1) | (address & 0x3FF);
    }
    
    template <>
    uint16_t getMirroredAddress<MirroringType::Vertical>(uint16_t address) {
        return address & 0x7FF;
    }
    
    template <>
    uint16_t getMirroredAddress<MirroringType::SingleScreen>(uint16_t address) {
        return address & 0x3FF;
    }
    
    template <>
    uint16_t getMirroredAddress<MirroringType::FourScreen>(uint16_t address) {
        return address & 0xFFF;
    }
    
} }
