//
//  Common.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 20/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "Common.hpp"


namespace NESEmu { namespace Common {
    
    // Taken from here : https://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
    uint8_t reverseBits(uint8_t data) {
        data = (data & 0xF0) >> 4 | (data & 0x0F) << 4;
        data = (data & 0xCC) >> 2 | (data & 0x33) << 2;
        data = (data & 0xAA) >> 1 | (data & 0x55) << 1;
        
        return data;
    }
    
} }
