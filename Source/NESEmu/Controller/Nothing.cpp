//
//  Nothing.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 23/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "Nothing.hpp"


namespace NESEmu { namespace Controller {
    
    void Nothing::clock(uint8_t &data) {
        // When nothing is connected, there is a pull-up resistor that drive line high
        data |= 0x1;
    }
    
    void Nothing::out(bool high) {
        // Does nothing
    }
    
    void Nothing::update() {
        // Does nothing
    }
    
} }
