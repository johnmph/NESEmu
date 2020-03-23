//
//  PulseChannel.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "PulseChannel.hpp"

/*
 
                      Sweep ----> Timer
                        |           |
                        |           |
                        |           v
                        |       Sequencer   Length Counter
                        |           |             |
                        |           |             |
                        v           v             v
     Envelope -------> Gate -----> Gate -------> Gate --->(to mixer)
 
 */


namespace NESEmu { namespace Apu {
    
    uint8_t const PulseChannel::_dutyWaveforms[4] = {
        0b00000010,     // 12.5%
        0b00000110,     // 25%
        0b00011110,     // 50%
        0b11111001      // 75%
    };
    
    PulseChannel::PulseChannel(bool sweepOneComplementMode) : _sweepUnit(sweepOneComplementMode, _timer) {
    }
    
    void PulseChannel::powerUp() {
    }
    
    void PulseChannel::reset() {
        _lengthCounter.setEnabled(false);
    }
    
    void PulseChannel::clock() {
        // If counter reached 0
        if (_counter == 0) {
            // Reload counter
            _counter = _timer;
            
            // Increment sequencer current step
            _sequencerCurrentStep = (_sequencerCurrentStep + 1) & 0x7;
            
            return;
        }
        
        // Decrement counter
        --_counter;
    }
    
    void PulseChannel::clockFrameCounterQuarterFrame() {
        // Clock envelope
        _envelopeUnit.clock();
    }
    
    void PulseChannel::clockFrameCounterHalfFrame() {
        // Clock length counter
        _lengthCounter.clock();
        
        // Clock sweep unit
        _sweepUnit.clock();
    }
    
    uint8_t PulseChannel::getOutput() const {
        return ((_sweepUnit.getOutput()) && (((_waveform >> _sequencerCurrentStep) & 0x1) != 0) && (_lengthCounter.getOutput())) ? _envelopeUnit.getVolume() : 0;
    }
    
    void PulseChannel::setRegister(uint8_t registerNumber, uint8_t data) {
        // Duty, envelope loop / length counter halt, constant volume, volume/envelope
        if (registerNumber == 0x0) {
            // Envelope divider period or constant volume
            _envelopeUnit.setDividerPeriodOrConstantVolume(data & 0xF);
            
            // Envelope constant volume mode
            _envelopeUnit.setConstantVolumeMode((data & 0x10) != 0);
            
            // Envelope loop mode
            _envelopeUnit.setLoopMode((data & 0x20) != 0);
            
            // Length counter halt
            _lengthCounter.setHalt((data & 0x20) != 0);
            
            // Duty cycle
            _waveform = _dutyWaveforms[data >> 6];
        }
        // Sweep unit: enabled, period, negate, shift
        else if (registerNumber == 0x1) {
            // Shift count
            _sweepUnit.setShiftCount(data & 0x7);
            
            // Negate
            _sweepUnit.setEnabled((data & 0x8) != 0);
            
            // Period
            _sweepUnit.setPeriod((data >> 4) & 0x7);
            
            // Enabled flag
            _sweepUnit.setEnabled((data & 0x80) != 0);
            
            // Reload sweep unit
            _sweepUnit.reload();
        }
        // Timer low
        else if (registerNumber == 0x2) {
            _timer = (_timer & 0x700) | data;
        }
        // Length counter load, timer high
        else if (registerNumber == 0x3) {
            // Timer high
            _timer = ((data & 0x7) << 8) | (_timer & 0xFF);
            
            // Length counter load
            _lengthCounter.setValueIndex(data >> 3);
            
            // Start envelope
            _envelopeUnit.start();
            
            // Reset sequencer current step
            _sequencerCurrentStep = 0;
        }
    }
    
    bool PulseChannel::getLengthCounterOutput() const {
        return _lengthCounter.getOutput();
    }
    
    void PulseChannel::setLengthCounterEnabled(bool enabled) {
        _lengthCounter.setEnabled(enabled);
    }
    
} }
