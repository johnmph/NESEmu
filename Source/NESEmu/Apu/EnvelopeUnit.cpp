//
//  EnvelopeUnit.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "EnvelopeUnit.hpp"


namespace NESEmu { namespace Apu {
    
    void EnvelopeUnit::powerUp() {
        _dividerPeriodOrConstantVolume = 0;
        _counter = 0;
        _decayCounter = 0;
        _startFlag = false;
        _loopMode = false;
        _constantVolumeMode = false;
    }
    
    void EnvelopeUnit::clock() {
        // If started
        if (_startFlag) {
            // Reset flag
            _startFlag = false;
            
            // Reset decay counter
            _decayCounter = decayCounterReloadValue;
            
            // Reload counter
            _counter = _dividerPeriodOrConstantVolume;
            
            // Exit
            return;
        }
        
        // If counter not reached 0
        if (_counter > 0) {
            // Decrement and exit
            --_counter;
            
            return;
        }
        
        // Reload counter
        _counter = _dividerPeriodOrConstantVolume;
        
        // If decay counter not reached 0
        if (_decayCounter > 0) {
            // Decrement and exit
            --_decayCounter;
            
            return;
        }
        
        // If loop, reload decay counter
        if (_loopMode) {
            _decayCounter = decayCounterReloadValue;
        }
    }
    
    void EnvelopeUnit::start() {
        // Set flag
        _startFlag = true;
    }
    
    uint8_t EnvelopeUnit::getVolume() const {
        return (_constantVolumeMode) ? _dividerPeriodOrConstantVolume : _decayCounter;
    }
    
    void EnvelopeUnit::setDividerPeriodOrConstantVolume(uint8_t value) {
        // Save value
        _dividerPeriodOrConstantVolume = value;
    }
    
    void EnvelopeUnit::setLoopMode(bool loopMode) {
        // Save value
        _loopMode = loopMode;
    }
    
    void EnvelopeUnit::setConstantVolumeMode(bool constantVolumeMode) {
        // Save value
        _constantVolumeMode = constantVolumeMode;
    }
    
} }
