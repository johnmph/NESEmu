//
//  Apu_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 18/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_s_hpp
#define NESEmu_Apu_s_hpp


// Envelope unit

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::EnvelopeUnit::clock() {
    // If started
    if (_startFlag) {
        // Reset flag
        _startFlag = false;
        
        // Reset decay counter
        _decayCounter = 15;
        
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
        _decayCounter = 15;
    }
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::EnvelopeUnit::start() {
    // Set flag
    _startFlag = true;
}

template <class TInterruptHardware, class TSoundHardware>
uint8_t Chip<TInterruptHardware, TSoundHardware>::EnvelopeUnit::getVolume() const {
    return (_constantVolumeMode) ? _dividerPeriodOrConstantVolume : _decayCounter;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::EnvelopeUnit::setDividerPeriodOrConstantVolume(uint8_t value) {
    // Save value
    _dividerPeriodOrConstantVolume = value;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::EnvelopeUnit::setLoopMode(bool loopMode) {
    // Save value
    _loopMode = loopMode;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::EnvelopeUnit::setConstantVolumeMode(bool constantVolumeMode) {
    // Save value
    _constantVolumeMode = constantVolumeMode;
}

// Sweep unit

template <class TInterruptHardware, class TSoundHardware>
Chip<TInterruptHardware, TSoundHardware>::SweepUnit::SweepUnit(bool oneComplementMode) : _oneComplementMode(oneComplementMode) {
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::SweepUnit::clock() {
    
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::SweepUnit::updateChannelPeriod() {
    
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::SweepUnit::setPeriod(uint8_t period) {
    _period = period;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::SweepUnit::setShiftCount(uint8_t shiftCount) {
    _shiftCount = shiftCount;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::SweepUnit::setEnabled(bool enabled) {
    _enabled = enabled;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::SweepUnit::setNegate(bool negate) {
    _negate = negate;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::SweepUnit::reload() {
    _reloadFlag = true;
}

// Length Counter

template <class TInterruptHardware, class TSoundHardware>
uint8_t const Chip<TInterruptHardware, TSoundHardware>::LengthCounter::_lengthValues[32] = {
    10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
    12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
};

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::LengthCounter::clock() {
    // If disabled, exit
    if (!_enabled) {
        return;
    }
    
    // Decrement counter if not reached 0 and if not halted
    if ((_counter > 0) && (!_halt)) {
        --_counter;
    }
}

template <class TInterruptHardware, class TSoundHardware>
uint8_t Chip<TInterruptHardware, TSoundHardware>::LengthCounter::getValue() const {
    return _counter;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::LengthCounter::setEnabled(bool enabled) {
    _enabled = enabled;
    
    // If disabled, reset to 0
    if (!_enabled) {
        _counter = 0;
    }
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::LengthCounter::setHalt(bool halt) {
    _halt = halt;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::LengthCounter::setValueIndex(uint8_t index) {
    // If disabled, exit
    if (!_enabled) {
        return;
    }
    
    // Get length from index
    _counter = _lengthValues[index];
}

// Frame Counter

template <class TInterruptHardware, class TSoundHardware>
Chip<TInterruptHardware, TSoundHardware>::FrameCounter::FrameCounter(Chip &chip) : _chip(chip) {
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::FrameCounter::clock() {
    // Check if need to reset
    if (_needToReset) {
        // Decrement delay
        --_resetDelay;
        
        // If it's time to reset
        if (_resetDelay == 0) {
            // Reset frame counter
            _counter = 0;
            _needToReset = false;
            
            // Clock quarter and half frame if 5-step
            if (_sequence5StepMode) {
                clockQuarterFrame();
                clockHalfFrame();
            }
        }
    }
    
    // Check sequence step
    if (_sequence5StepMode) {
        clock5StepSequence();
    } else {
        clock4StepSequence();
    }
    
    // Increment counter
    ++_counter;
}

template <class TInterruptHardware, class TSoundHardware>
bool Chip<TInterruptHardware, TSoundHardware>::FrameCounter::getInterrupt() const {
    return _interrupt;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::FrameCounter::resetInterrupt() {
    _interrupt = false;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::FrameCounter::setDisableInterrupt(bool disableInterrupt) {
    _disableInterrupt = disableInterrupt;
    
    // If interrupt disabled, reset possible interrupt flag
    if (_disableInterrupt) {
        _interrupt = false;
    }
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::FrameCounter::setSequence5StepMode(bool sequence5StepMode) {
    _sequence5StepMode = sequence5StepMode;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::FrameCounter::requestReset() {
    _needToReset = true;
    _resetDelay = (_counter & 0x1) ? 4 : 3;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::FrameCounter::clock4StepSequence() {
    // Check if last step reached
    if (_counter == 29830) {//TODO: mettre les valeurs dans des constantes
        // Set interrupt if not disabled
        _interrupt = !_disableInterrupt;
        
        // Reset counter
        _counter = 0;
    }
    // Step 1
    else if (_counter == 7457) {
        clockQuarterFrame();
    }
    // Step 2
    else if (_counter == 14913) {
        clockQuarterFrame();
        clockHalfFrame();
    }
    // Step 3
    else if (_counter == 22371) {
        clockQuarterFrame();
    }
    // Step 4 (first cycle for interrupt)
    else if (_counter == 29828) {
        // Set interrupt if not disabled
        _interrupt = !_disableInterrupt;
    }
    // Step 4
    else if (_counter == 29829) {
        // Set interrupt if not disabled
        _interrupt = !_disableInterrupt;
        
        clockQuarterFrame();
        clockHalfFrame();
    }
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::FrameCounter::clock5StepSequence() {
    // Check if last step reached
    if (_counter == 37282) {//TODO: mettre les valeurs dans des constantes
        // Reset counter
        _counter = 0;
    }
    // Step 1
    else if (_counter == 7457) {
        clockQuarterFrame();
    }
    // Step 2
    else if (_counter == 14913) {
        clockQuarterFrame();
        clockHalfFrame();
    }
    // Step 3
    else if (_counter == 22371) {
        clockQuarterFrame();
    }
    // Nothing happen on step 4
    // Step 5
    else if (_counter == 37281) {
        clockQuarterFrame();
        clockHalfFrame();
    }
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::FrameCounter::clockQuarterFrame() {
    _chip.clockFrameCounterQuarterFrame();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::FrameCounter::clockHalfFrame() {
    _chip.clockFrameCounterHalfFrame();
}

// Pulse

template <class TInterruptHardware, class TSoundHardware>
Chip<TInterruptHardware, TSoundHardware>::Pulse::Pulse(bool sweepOneComplementMode) : sweepUnit(sweepOneComplementMode) {
}

// Chip

template <class TInterruptHardware, class TSoundHardware>
Chip<TInterruptHardware, TSoundHardware>::Chip(TInterruptHardware &interruptHardware, TSoundHardware &soundHardware) : _interruptHardware(interruptHardware), _soundHardware(soundHardware), _frameCounter(*this), _pulse{ true, false } {
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::powerUp() {
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::clock() {
    // Clock frame counter
    _frameCounter.clock();
    
    // Check interrupt
    checkInterrupt();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::reset(bool high) {
    // If line high, exit
    if (high == true) {
        return;
    }
    
    // Reset status register
    _pulse[0].lengthCounter.setEnabled(false);
    _pulse[1].lengthCounter.setEnabled(false);
    _triangle.lengthCounter.setEnabled(false);
    _noise.lengthCounter.setEnabled(false);
    _statusEnableDmc = false;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::setChannelRegister(uint16_t address, uint8_t data) {
    // Pulse registers
    if (address < 0x4008) {
        setPulseChannelRegister((address >= 0x4004), (address & 0x3), data);
    }
    // Triangle register
    else if (address < 0x400C) {
        setTriangleChannelRegister((address & 0x3), data);
    }
    // Noise register
    else if (address < 0x4010) {
        setNoiseChannelRegister((address & 0x3), data);
    }
    // DMC register
    else {
        setDmcChannelRegister((address & 0x3), data);
    }
}

template <class TInterruptHardware, class TSoundHardware>
uint8_t Chip<TInterruptHardware, TSoundHardware>::getStatusRegister() {
    uint8_t data = (false << 7) |
                   (_frameCounter.getInterrupt() << 6) |
                   (false << 4) |
                   ((_noise.lengthCounter.getValue() > 0) << 3) |
                   ((_triangle.lengthCounter.getValue() > 0) << 2) |
                   ((_pulse[1].lengthCounter.getValue() > 0) << 1) |
                   (_pulse[0].lengthCounter.getValue() > 0);//TODO : continuer (DMC)
    
    // Reset frame counter interrupt
    _frameCounter.resetInterrupt();
    
    // TODO: If an interrupt flag was set at the same moment of the read, it will read back as 1 but it will not be cleared.
    
    return data;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::setStatusRegister(uint8_t data) {
    // Get channels enable flags
    _pulse[0].lengthCounter.setEnabled((data & 0x1) != 0);
    _pulse[1].lengthCounter.setEnabled((data & 0x2) != 0);
    _triangle.lengthCounter.setEnabled((data & 0x4) != 0);
    _noise.lengthCounter.setEnabled((data & 0x8) != 0);
    _statusEnableDmc = data & 0x10;
    
    // TODO: reseter le dmc interrupt flag + continuer le reste
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::setFrameCounterRegister(uint8_t data) {
    _frameCounter.setDisableInterrupt(data & 0x40);
    _frameCounter.setSequence5StepMode(data & 0x80);
    
    _frameCounter.requestReset();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::setPulseChannelRegister(uint8_t pulseChannelNumber, uint8_t registerNumber, uint8_t data) {
    // Duty, envelope loop / length counter halt, constant volume, volume/envelope
    if (registerNumber == 0x0) {
        // Envelope divider period or constant volume
        _pulse[pulseChannelNumber].envelopeUnit.setDividerPeriodOrConstantVolume(data & 0xF);
        
        // Envelope constant volume mode
        _pulse[pulseChannelNumber].envelopeUnit.setConstantVolumeMode((data & 0x10) != 0);
        
        // Envelope loop mode
        _pulse[pulseChannelNumber].envelopeUnit.setLoopMode((data & 0x20) != 0);
        
        // Length counter halt
        _pulse[pulseChannelNumber].lengthCounter.setHalt((data & 0x20) != 0);
    }
    // Sweep unit: enabled, period, negate, shift
    else if (registerNumber == 0x1) {
        // Shift count
        _pulse[pulseChannelNumber].sweepUnit.setShiftCount(data & 0x7);
        
        // Negate
        _pulse[pulseChannelNumber].sweepUnit.setEnabled((data & 0x8) != 0);
        
        // Period
        _pulse[pulseChannelNumber].sweepUnit.setPeriod((data >> 4) & 0x7);
        
        // Enabled flag
        _pulse[pulseChannelNumber].sweepUnit.setEnabled((data & 0x80) != 0);
        
        // Reload sweep unit
        _pulse[pulseChannelNumber].sweepUnit.reload();
    }
    // Timer low
    else if (registerNumber == 0x2) {
        
    }
    // Length counter load, timer high
    else if (registerNumber == 0x3) {
        // Start envelope
        _pulse[pulseChannelNumber].envelopeUnit.start();
        
        // Length counter load
        _pulse[pulseChannelNumber].lengthCounter.setValueIndex(data >> 3);
    }
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::setTriangleChannelRegister(uint8_t registerNumber, uint8_t data) {
    // Length counter halt / linear counter control, linear counter load
    if (registerNumber == 0x0) {
        // Length counter halt
        _triangle.lengthCounter.setHalt((data & 0x80) != 0);
    }
    // Register 0x1 unused
    // Timer low
    else if (registerNumber == 0x2) {
        
    }
    // Length counter load, timer high
    else if (registerNumber == 0x3) {
        // Length counter load
        _triangle.lengthCounter.setValueIndex(data >> 3);
    }
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::setNoiseChannelRegister(uint8_t registerNumber, uint8_t data) {
    // Envelope loop / length counter halt, constant volume, volume/envelope
    if (registerNumber == 0x0) {
        // Envelope divider period or constant volume
        _noise.envelopeUnit.setDividerPeriodOrConstantVolume(data & 0xF);
        
        // Envelope constant volume mode
        _noise.envelopeUnit.setConstantVolumeMode((data & 0x10) != 0);
        
        // Envelope loop mode
        _noise.envelopeUnit.setLoopMode((data & 0x20) != 0);
        
        // Length counter halt
        _noise.lengthCounter.setHalt((data & 0x20) != 0);
    }
    // Register 0x1 unused
    // Loop noise, noise period
    else if (registerNumber == 0x2) {
        
    }
    // Length counter load
    else if (registerNumber == 0x3) {
        // Start envelope
        _noise.envelopeUnit.start();
        
        // Length counter load
        _noise.lengthCounter.setValueIndex(data >> 3);
    }
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::setDmcChannelRegister(uint8_t registerNumber, uint8_t data) {
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

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::clockFrameCounterQuarterFrame() {
    // Clock envelope channels
    _pulse[0].envelopeUnit.clock();
    _pulse[1].envelopeUnit.clock();
    _noise.envelopeUnit.clock();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::clockFrameCounterHalfFrame() {
    // Clock length counter channels
    _pulse[0].lengthCounter.clock();
    _pulse[1].lengthCounter.clock();
    _triangle.lengthCounter.clock();
    _noise.lengthCounter.clock();
    
    // Clock sweep unit channels
    _pulse[0].sweepUnit.clock();
    _pulse[1].sweepUnit.clock();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::checkInterrupt() {
    _interruptHardware.irq(!_frameCounter.getInterrupt());
}

#endif /* NESEmu_Apu_s_hpp */
