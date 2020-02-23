//
//  Nothing.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 23/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "Nothing.hpp"


namespace NESEmu { namespace Controller {
    
    uint8_t Nothing::read() {// TODO: pour l'open bus il faudrait passer uint8_t &data et ne modifier que le bit 0 et faire pareil pour tous les read
        // When nothing is connected, there is a pull-up resistor that drive line high
        return true;
    }
    
    void Nothing::write(bool data) {
        // Does nothing
    }
    
    void Nothing::update() {
        // Does nothing
    }
    
} }
