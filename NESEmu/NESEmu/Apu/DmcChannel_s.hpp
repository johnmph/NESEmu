//
//  DmcChannel_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 25/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_DmcChannel_s_hpp
#define NESEmu_Apu_DmcChannel_s_hpp

/*
 
                              Timer
                                |
                                v
     Reader ---> Buffer ---> Shifter ---> Output level ---> (to the mixer)
 
 */


template <class TChip>
uint16_t const DmcChannel<TChip>::_rates[16] = {
    428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54
};


template <class TChip>
DmcChannel<TChip>::DmcChannel(TChip &chip) : _chip(chip) {
}

template <class TChip>
void DmcChannel<TChip>::powerUp() {
    // Start silenced
    _silenceFlag = true;    // TODO: voir si ok
    
    // Initialize timer at its first rate value
    _timer = _rates[0];
    
    // The first shift register clock happen after 1024 CPU clock cycles (due to LFSR start state)
    // See : http://forums.nesdev.com/viewtopic.php?f=3&t=19910
    _counter = 1024;
    
    // Initialize sample address and length at their minimal values
    // See https://forums.nesdev.com/viewtopic.php?f=3&t=18278
    _sampleAddress = 0xC000;
    _sampleLength = 1;
    
    // Initialize remaining at zero
    _shiftRegister = 0xFF;
    _shiftRegisterShiftedBitsCounter = 0;
    _outputLevel = 0;
    _currentSampleAddress = 0x0;
    _sampleRemainingBytes = 0;
    _sampleBuffer = 0x0;
    _sampleBufferFilled = false;
    _loopFlag = false;
    _interrupt = false;
    _enableInterrupt = false;
}

template <class TChip>
void DmcChannel<TChip>::clock() {
    // If need to request a sample
    if (_needRequestSample && ((_counter & 0x1) == 0)) {
        // Request a sample
        _chip.requestDmcSample(_currentSampleAddress, true);
        
        // Reset flag
        _needRequestSample = false;
    }
    
    // Decrement counter
    --_counter;
    
    // Exit if counter not reached 0
    if (_counter > 0) {
        return;
    }
    
    // Reload counter
    _counter = _timer;
    
    // Change output level if not silenced
    if (!_silenceFlag) {
        // Add
        if ((_shiftRegister & 0x1) != 0) {
            if (_outputLevel <= 125) {
                _outputLevel += 2;
            }
        }
        // Substract
        else {
            if (_outputLevel >= 2) {
                _outputLevel -= 2;
            }
        }
    }
    
    // Update shift register
    _shiftRegister >>= 1;
    ++_shiftRegisterShiftedBitsCounter;
    
    // Exit if current cycle not terminated
    if (_shiftRegisterShiftedBitsCounter < 8) {
        return;
    }
    
    // Reset shifted bit counter
    _shiftRegisterShiftedBitsCounter = 0;
    
    // If sample buffer is filled
    if (_sampleBufferFilled) {
        // Empty the buffer into the shift register
        _shiftRegister = _sampleBuffer;
        _sampleBufferFilled = false;
        
        // Reset silence flag
        _silenceFlag = false;
        
        // Request a sample if necessary
        if (_sampleRemainingBytes > 0) {
            //_needRequestSample = true;
            _chip.requestDmcSample(_currentSampleAddress, false);
        }
        
    } else {
        // Set silence flag
        _silenceFlag = true;
    }
}

template <class TChip>
bool DmcChannel<TChip>::isEnabled() const {
    return _sampleRemainingBytes > 0;
}

template <class TChip>
void DmcChannel<TChip>::setEnabled(bool enabled) {
    // If disabled
    if (!enabled) {
        // Reset sample remaining bytes
        _sampleRemainingBytes = 0;
        
        // Exit
        return;
    }
    
    // Exit if remaining bytes
    if (_sampleRemainingBytes > 0) {
        return;
    }
    
    // Restart sample
    _currentSampleAddress = _sampleAddress;
    _sampleRemainingBytes = _sampleLength;
    
    // Request a sample if necessary
    if (!_sampleBufferFilled) {
        _needRequestSample = true;
    }
}

template <class TChip>
bool DmcChannel<TChip>::getInterrupt() const {
    return _interrupt;
}

template <class TChip>
void DmcChannel<TChip>::resetInterrupt() {
    _interrupt = false;
}

template <class TChip>
uint8_t DmcChannel<TChip>::getOutput() const {
    return _outputLevel;
}

template <class TChip>
void DmcChannel<TChip>::setRegister(uint8_t registerNumber, uint8_t data) {
    // IRQ enable, loop, frequency
    if (registerNumber == 0x0) {
        // Get timer from rate index
        _timer = _rates[data & 0xF];
        
        // Get loop flag
        _loopFlag = (data & 0x40) != 0;
        
        // Get interrupt enable
        _enableInterrupt = (data & 0x80) != 0;
        
        // If interrupt disabled, reset possible interrupt flag
        if (!_enableInterrupt) {
            _interrupt = false;
        }
    }
    // Load counter
    else if (registerNumber == 0x1) {
        _outputLevel = data & 0x7F; // TODO: If the timer is outputting a clock at the same time, the output level is occasionally not changed properly
    }
    // Sample address
    else if (registerNumber == 0x2) {
        _sampleAddress = 0xC000 + (data << 6);
    }
    // Sample length
    else {
        _sampleLength = (data << 4) | 0x1;
    }
}

template <class TChip>
void DmcChannel<TChip>::sampleFetched(uint8_t data) {
    // Set sample buffer
    _sampleBuffer = data;
    
    // Set sample buffer filled flag
    _sampleBufferFilled = true;
    
    // Update address
    ++_currentSampleAddress;
    _currentSampleAddress |= 0x8000;
    
    // Decrement remaining bytes counter
    --_sampleRemainingBytes;
    
    // Exit if remaining bytes
    if (_sampleRemainingBytes > 0) {
        return;
    }
    
    // If sample looped
    if (_loopFlag) {
        // Reload current sample
        _currentSampleAddress = _sampleAddress;
        _sampleRemainingBytes = _sampleLength;
        
        // Exit
        return;
    }
    
    // If IRQ enabled
    if (_enableInterrupt) {
        _interrupt = true;
    }
}

#endif /* NESEmu_Apu_DmcChannel_s_hpp */
