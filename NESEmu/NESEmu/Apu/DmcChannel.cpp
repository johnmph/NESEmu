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
    
} }
