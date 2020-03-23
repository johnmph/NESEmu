//
//  NoiseChannel.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "NoiseChannel.hpp"

/*
 
        Timer --> Shift Register   Length Counter
                        |                |
                        v                v
     Envelope -------> Gate ----------> Gate --> (to mixer)
 
 */


namespace NESEmu { namespace Apu {
    
    uint16_t const NoiseChannel::_timerPeriods[16] = {
        4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
    };
    
    void NoiseChannel::powerUp() {
        _shiftRegister = 0x1;
    }
    
    void NoiseChannel::reset() {
        _lengthCounter.setEnabled(false);
    }
    
    void NoiseChannel::clock() {
        // If counter reached 0
        if (_counter == 0) {
            // Reload counter
            _counter = _timer;
            
            // Shift register
            bool feedback = (_shiftRegister & 0x1) ^ (((_mode) ? (_shiftRegister & 0x40) : (_shiftRegister & 0x2)) != 0);
            _shiftRegister = (feedback << 14) | (_shiftRegister >> 1);
            
            return;//TODO: a voir
        }
        
        // Decrement counter
        --_counter;
    }
    
    void NoiseChannel::clockFrameCounterQuarterFrame() {
        // Clock envelope
        _envelopeUnit.clock();
    }
    
    void NoiseChannel::clockFrameCounterHalfFrame() {
        // Clock length counter
        _lengthCounter.clock();
    }
    
    uint8_t NoiseChannel::getOutput() const {
        return (((_shiftRegister & 0x1) == 0) && (_lengthCounter.getOutput())) ? _envelopeUnit.getVolume() : 0;
    }
    
    void NoiseChannel::setRegister(uint8_t registerNumber, uint8_t data) {
        // Envelope loop / length counter halt, constant volume, volume/envelope
        if (registerNumber == 0x0) {
            // Envelope divider period or constant volume
            _envelopeUnit.setDividerPeriodOrConstantVolume(data & 0xF);
            
            // Envelope constant volume mode
            _envelopeUnit.setConstantVolumeMode((data & 0x10) != 0);
            
            // Envelope loop mode
            _envelopeUnit.setLoopMode((data & 0x20) != 0);
            
            // Length counter halt
            _lengthCounter.setHalt((data & 0x20) != 0);
        }
        // Register 0x1 unused
        // Loop noise, noise period
        else if (registerNumber == 0x2) {
            // Period
            uint8_t period = data & 0xF;
            
            // Get timer from period
            _timer = _timerPeriods[period];
            
            // Mode
            _mode = (data & 0x80) != 0;
        }
        // Length counter load
        else if (registerNumber == 0x3) {
            // Length counter load
            _lengthCounter.setValueIndex(data >> 3);
            
            // Start envelope
            _envelopeUnit.start();
        }
    }
    
    bool NoiseChannel::getLengthCounterOutput() const {
        return _lengthCounter.getOutput();
    }
    
    void NoiseChannel::setLengthCounterEnabled(bool enabled) {
        _lengthCounter.setEnabled(enabled);
    }
    
} }
