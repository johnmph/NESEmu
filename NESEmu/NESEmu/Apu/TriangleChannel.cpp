//
//  TriangleChannel.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "TriangleChannel.hpp"

/*
 
           Linear Counter   Length Counter
                 |                |
                 v                v
     Timer ---> Gate ----------> Gate ---> Sequencer ---> (to mixer)
 
 */


namespace NESEmu { namespace Apu {
    
    void TriangleChannel::powerUp() {
    }
    
    void TriangleChannel::clock() {
        // Update length counter
        _lengthCounter.update();
        
        // If counter reached 0
        if ((_counter == 0) && (_timer > 1)) {//TODO: j'ai mis le test timer > 1 pour retirer les high frequency
            // Reload counter
            _counter = _timer;
            
            // Sequencer is clocked if both linear and length counter are non zero
            if ((_linearCounter > 0) && _lengthCounter.getOutput()) {
                _sequencerCurrentStep = (_sequencerCurrentStep + 1) & sequencerStepsMask;
            }
            
            return;
        }
        
        // Decrement counter
        --_counter;
    }
    
    void TriangleChannel::reset() {
        _lengthCounter.setEnabled(false);
    }
    
    void TriangleChannel::clockFrameCounterQuarterFrame() {
        // Reload linear counter if necessary
        if (_linearCounterReloadFlag) {
            _linearCounter = _linearCounterReloadValue;
        }
        // Decrement linear counter if necessary
        else if (_linearCounter > 0) {
            --_linearCounter;
        }
        
        // Reset linear counter reload flag if necessary
        if (!_linearCounterControl) {
            _linearCounterReloadFlag = false;
        }
    }
    
    void TriangleChannel::clockFrameCounterHalfFrame() {
        // Clock length counter
        _lengthCounter.clock();
    }
    
    uint8_t TriangleChannel::getOutput() const {
        return (_sequencerCurrentStep < 16) ? (15 - _sequencerCurrentStep) : (_sequencerCurrentStep - 16);
    }
    
    void TriangleChannel::setRegister(uint8_t registerNumber, uint8_t data) {
        // Length counter halt / linear counter control, linear counter load
        if (registerNumber == 0x0) {
            // Linear counter reload value
            _linearCounterReloadValue = data & 0x7F;
            
            // Control flag
            _linearCounterControl = (data & 0x80) != 0;
            
            // Length counter halt
            _lengthCounter.setHalt((data & 0x80) != 0);
        }
        // Register 0x1 unused
        else if (registerNumber == 0x1) {
        }
        // Timer low
        else if (registerNumber == 0x2) {
            _timer = (_timer & 0x700) | data;
        }
        // Length counter load, timer high
        else {
            // Timer high
            _timer = ((data & 0x7) << 8) | (_timer & 0xFF);
            
            // Length counter load
            _lengthCounter.setValueIndex(data >> 3);
            
            // Set linear counter reload flag
            _linearCounterReloadFlag = true;
            
            //_counter = _timer;//TODO: a mettre ??? si c'est comme pulse, non !
        }
    }
    
    bool TriangleChannel::getLengthCounterOutput() const {
        return _lengthCounter.getOutput();
    }
    
    void TriangleChannel::setLengthCounterEnabled(bool enabled) {
        _lengthCounter.setEnabled(enabled);
    }
    
} }
