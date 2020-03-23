//
//  LengthCounter.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "LengthCounter.hpp"


namespace NESEmu { namespace Apu {
    
    uint8_t const LengthCounter::_lengthValues[32] = {
        10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
        12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
    };
    
    void LengthCounter::clock() {
        // If disabled, exit
        if (!_enabled) {
            return;
        }
        
        // Decrement counter if not reached 0 and if not halted
        if ((_counter > 0) && (!_halt)) {
            --_counter;
        }
    }
    
    bool LengthCounter::getOutput() const {
        // Output enabled while counter has not reached 0
        return _counter > 0;
    }
    
    void LengthCounter::setEnabled(bool enabled) {
        _enabled = enabled;
        
        // If disabled, reset to 0
        if (!_enabled) {
            _counter = 0;
        }
    }
    
    void LengthCounter::setHalt(bool halt) {
        _halt = halt;
    }
    
    void LengthCounter::setValueIndex(uint8_t index) {
        // If disabled, exit
        if (!_enabled) {
            return;
        }
        
        // Get length from index
        _counter = _lengthValues[index];
    }
    
} }
