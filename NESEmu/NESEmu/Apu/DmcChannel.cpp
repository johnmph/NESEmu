//
//  DmcChannel.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "DmcChannel.hpp"

/*
 
                              Timer
                                |
                                v
     Reader ---> Buffer ---> Shifter ---> Output level ---> (to the mixer)
 
 */


namespace NESEmu { namespace Apu {
    
    void DmcChannel::powerUp() {
    }
    
    void DmcChannel::reset() {
    }
    
    void DmcChannel::clock() {
    }
    
    uint8_t DmcChannel::getOutput() const {
        return 0;// TODO: a faire
    }
    
    void DmcChannel::setRegister(uint8_t registerNumber, uint8_t data) {
        // IRQ enable, loop, frequency
        if (registerNumber == 0x0) {
            
        }
        // Load counter
        else if (registerNumber == 0x1) {
            
        }
        // Sample address
        else if (registerNumber == 0x2) {
            
        }
        // Sample length
        else if (registerNumber == 0x3) {
        }
    }
    
} }
