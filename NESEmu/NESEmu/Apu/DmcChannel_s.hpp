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
    //428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54   // TODO: attention, ce sont des cpu cycles et pas des apu cycles, si on clock le dmc par apu cycle il faudra diviser ces valeurs par 2 (et on pourra passer en uint8_t pour _rate et _rates) !!!
    214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27
};


template <class TChip>
DmcChannel<TChip>::DmcChannel(TChip &chip) : _chip(chip) {
}

template <class TChip>
void DmcChannel<TChip>::powerUp() {//TODO: voir pour les powerUp et reset de chaque channel et unit
    _outputLevel = 0;
}

template <class TChip>
void DmcChannel<TChip>::clock() {
    // If counter reached 0
    if (_counter == 0) {
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
        --_shiftRegisterRemainingBitsCounter;
        
        // If need to start a new cycle
        if (_shiftRegisterRemainingBitsCounter == 0) {
            _shiftRegisterRemainingBitsCounter = 8;
            
            // If sample buffer is filled
            if (_sampleBufferFilled) {
                // Empty the buffer into the shift register
                _shiftRegister = _sampleBuffer;
                _sampleBufferFilled = false;
                
                // Reset silence flag
                _silenceFlag = false;
            } else {
                // Set silence flag
                _silenceFlag = true;
            }
        }
    } else {
        // Decrement counter
        --_counter;
    }
    
    // If sample buffer empty
    if (!_sampleBufferFilled) {
        // Load a sample
        loadSample();
    }
}

template <class TChip>
void DmcChannel<TChip>::reset() {
}

template <class TChip>
uint16_t DmcChannel<TChip>::getSampleRemainingBytesCount() const {
    return _sampleRemainingBytes;
}

template <class TChip>
void DmcChannel<TChip>::setEnabled(bool enabled) {//TODO: a voir
    // Enabled
    if (enabled) {
        // Restart sample if there is no more bytes remaining
        if (_sampleRemainingBytes == 0) {
            _currentSampleAddress = _sampleAddress;
            _sampleRemainingBytes = _sampleLength;
        }
        
        // TODO: voir si appeler ici load sample ou si c'est bon niveau timing juste dans le clock !!!
    }
    // Disabled
    else {
        // Reset sample remaining bytes
        _sampleRemainingBytes = 0;
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
    else if (registerNumber == 0x3) {
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
    if (_currentSampleAddress == 0x0) {
        _currentSampleAddress = 0x8000;
    }
    
    // Decrement remaining bytes counter
    --_sampleRemainingBytes;
    
    // If no more remaining bytes
    if (_sampleRemainingBytes == 0) {
        // If sample looped
        if (_loopFlag) {
            // Reload current sample
            _currentSampleAddress = _sampleAddress;
            _sampleRemainingBytes = _sampleLength;
            
            return;
        }
        
        // If IRQ enabled
        if (_enableInterrupt) {
            _interrupt = true;
        }
    }
}

template <class TChip>
void DmcChannel<TChip>::loadSample() {
    // Don't load if buffer not empty of if no remaining bytes
    if (/*(_sampleBufferFilled) || */(_sampleRemainingBytes == 0)) {//TODO: pas besoin de la condition car l'appel est deja conditionné avec
        return;
    }
    
    // Request a sample
    _chip.requestDmcSample(_currentSampleAddress);
}

#endif /* NESEmu_Apu_DmcChannel_s_hpp */
