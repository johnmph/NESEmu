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
    
    _enabled = false;
    _requestDisable = false;
}

template <class TChip>
void DmcChannel<TChip>::clock() {
    // Decrement counter
    --_counter;
    
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
        ++_shiftRegisterShiftedBitsCounter;
        
        // If need to start a new cycle
        if (_shiftRegisterShiftedBitsCounter == 8) {
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
                    _chip.requestDmcSample(_currentSampleAddress, false);
                }

            } else {
                // Set silence flag
                _silenceFlag = true;
            }
        }
    }
    
    // Request a sample if necessary (only on even cycle)
/*    if ((!_sampleBufferFilled) && (_sampleRemainingBytes > 0)/* && ((_counter & 0x1) != 0)/) {
        _chip.requestDmcSample(_currentSampleAddress, false);
    }*/
    
    // Check if need to disable
    if (_requestDisable > 0) {
        --_requestDisable;
        
        // Disable
        if (_requestDisable == 0) {
            _enabled = false;
            _requestDisable = false;
            
            // If IRQ enabled
            if (_enableInterrupt) {
                _interrupt = true;
            }
        }
    }
}

template <class TChip>
bool DmcChannel<TChip>::isEnabled() const {
    return _enabled;
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
            
            if (!_sampleBufferFilled) {//TODO: ici plutot ? est ce possible d'avoir remainingBytes > 0 et d'avoir sampleBufferFilled false quand reenabled ???
                _chip.requestDmcSample(_currentSampleAddress, true);
            }
        }
        
        // TODO: voir si appeler ici load sample ou si c'est bon niveau timing juste dans le clock !!!
        // Request a sample if necessary
        //if (!_sampleBufferFilled) {   // TODO: voir si c'est pas le probleme du spr et dma dmc mais reflechir si ca revient pas au meme de l'avoir ou non ici avec l'autre dans le clock (mais voir si quand 2 d'affilée si ca annule le premier !!!)
            //_chip.requestDmcSample(_currentSampleAddress);
        //}
    }
    // Disabled
    else {
        // Reset sample remaining bytes
        _sampleRemainingBytes = 0;
    }
    
    // Set enabled flag
    _enabled = enabled; // TODO: activer ou desactiver le canal par ici le fait directement, pas de delai, donc c'est bon ainsi
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
    
    // If no more remaining bytes
    if (_sampleRemainingBytes == 0) {
        // If sample looped
        if (_loopFlag) {
            // Reload current sample
            _currentSampleAddress = _sampleAddress;
            _sampleRemainingBytes = _sampleLength;
            
            return;
        }
        
        _requestDisable = 2 + (_counter & 0x1);//TODO: ainsi ??? normalement toujours 3 non ? : pour simuler le vrai comportement d'apres Visual mais a cause de ca ca ne passe plus le apu_test (one byte buffer)
        if ((_counter & 0x1) == 0) {
            std::cout << "Counter even\n";
        }
        
        // If IRQ enabled
        /*if (_enableInterrupt) {
            _interrupt = true;
        }*/
    }
}

#endif /* NESEmu_Apu_DmcChannel_s_hpp */
