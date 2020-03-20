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
Chip<TInterruptHardware, TSoundHardware>::SweepUnit::SweepUnit(bool oneComplementMode, uint16_t &channelPeriod) : _oneComplementMode(oneComplementMode), _channelPeriod(channelPeriod) {
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::SweepUnit::clock() {
    // Update channel period if allowed
    if ((_counter == 0) && _enabled && getOutput()) {
        _channelPeriod = getTargetPeriod();
    }
    
    // If need to reload counter
    if ((_counter == 0) || (_reloadFlag)) {
        // Reload counter and reset reload flag
        _counter = _period;
        _reloadFlag = false;
        
        return;
    }
    
    // Decrement counter
    --_counter;
}

template <class TInterruptHardware, class TSoundHardware>
uint16_t Chip<TInterruptHardware, TSoundHardware>::SweepUnit::getTargetPeriod() const {
    // Shift right
    uint16_t amount = _channelPeriod >> _shiftCount;
    
    // Negate if necessary
    if (_negate) {
        amount = (_oneComplementMode) ? ~amount : -amount;
    }
    
    // Add to channel period
    return _channelPeriod + amount;
}

template <class TInterruptHardware, class TSoundHardware>
bool Chip<TInterruptHardware, TSoundHardware>::SweepUnit::getOutput() const {
    // Output only if not overflow (11 bits timer) and channel period is not less than 8
    return (getTargetPeriod() < 0x800) && (_channelPeriod > 7);
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
bool Chip<TInterruptHardware, TSoundHardware>::LengthCounter::getOutput() const {
    // Output enabled while counter has not reached 0
    return _counter > 0;
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
Chip<TInterruptHardware, TSoundHardware>::Pulse::Pulse(bool sweepOneComplementMode) : _sweepUnit(sweepOneComplementMode, _timer) {
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::Pulse::clock() {
    // If counter reached 0
    if (_counter == 0) {
        // Reload counter
        _counter = _timer;
        
        // Decrement current step
        _currentStep = (_currentStep > 0) ? (_currentStep - 1) : 7;
        
        return;//TODO: a voir
    }
    
    // Decrement counter
    --_counter;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::Pulse::clockFrameCounterQuarterFrame() {
    // Clock envelope
    _envelopeUnit.clock();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::Pulse::clockFrameCounterHalfFrame() {
    // Clock length counter
    _lengthCounter.clock();
    
    // Clock sweep unit
    _sweepUnit.clock();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::Pulse::reset() {
    _lengthCounter.setEnabled(false);
}

template <class TInterruptHardware, class TSoundHardware>
uint8_t Chip<TInterruptHardware, TSoundHardware>::Pulse::getOutput() const {
    /*
                           Sweep -----> Timer
                             |            |
                             |            |
                             |            v
                             |        Sequencer   Length Counter
                             |            |             |
                             |            |             |
                             v            v             v
          Envelope -------> Gate -----> Gate -------> Gate --->(to mixer)
     
     */
    return ((_sweepUnit.getOutput()) && (((_waveform >> _currentStep) & 0x1) != 0) && (_lengthCounter.getOutput())) ? _envelopeUnit.getVolume() : 0;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::Pulse::setRegister(uint8_t registerNumber, uint8_t data) {
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
        // Start envelope
        _envelopeUnit.start();
        
        // Length counter load
        _lengthCounter.setValueIndex(data >> 3);
        
        // Timer high
        _timer = ((data & 0x7) << 8) | (_timer & 0xFF);
        
        // Reset current step
        _currentStep = 0;
    }
}

template <class TInterruptHardware, class TSoundHardware>
bool Chip<TInterruptHardware, TSoundHardware>::Pulse::getLengthCounterOutput() const {
    return _lengthCounter.getOutput();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::Pulse::setLengthCounterEnabled(bool enabled) {
    _lengthCounter.setEnabled(enabled);
}

template <class TInterruptHardware, class TSoundHardware>
uint8_t const Chip<TInterruptHardware, TSoundHardware>::Pulse::_dutyWaveforms[4] = {
    0b00000001,     // 12.5%
    0b00000011,     // 25%
    0b00001111,     // 50%
    0b11111100      // 75%
};

// Triangle

template <class TInterruptHardware, class TSoundHardware>
uint8_t Chip<TInterruptHardware, TSoundHardware>::Triangle::getOutput() const {
    return 0;// TODO: a faire
}

// Noise

template <class TInterruptHardware, class TSoundHardware>
uint8_t Chip<TInterruptHardware, TSoundHardware>::Noise::getOutput() const {
    return 0;// TODO: a faire
}

// DMC

template <class TInterruptHardware, class TSoundHardware>
uint8_t Chip<TInterruptHardware, TSoundHardware>::Dmc::getOutput() const {
    return 0;// TODO: a faire
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
    // Clock pulse channels on APU cycle
    if (_oddCycle) {
        _pulse[0].clock();
        _pulse[1].clock();
    }
    
    // Toggle flag
    _oddCycle = !_oddCycle;
    
    // Clock frame counter
    _frameCounter.clock();
    
    // Check interrupt
    checkInterrupt();
    
    // Add data to sound hardware
    _soundHardware.addData(getMixedOutput());//TODO: a voir
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::reset(bool high) {
    // If line high, exit
    if (high == true) {
        return;
    }
    
    // Reset status register
    _pulse[0].reset();
    _pulse[1].reset();
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
                   (_noise.lengthCounter.getOutput() << 3) |
                   (_triangle.lengthCounter.getOutput() << 2) |
                   (_pulse[1].getLengthCounterOutput() << 1) |
                   _pulse[0].getLengthCounterOutput();//TODO : continuer (DMC)
    
    // Reset frame counter interrupt
    _frameCounter.resetInterrupt();
    
    // TODO: If an interrupt flag was set at the same moment of the read, it will read back as 1 but it will not be cleared.
    
    return data;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::setStatusRegister(uint8_t data) {
    // Get channels enable flags
    _pulse[0].setLengthCounterEnabled((data & 0x1) != 0);
    _pulse[1].setLengthCounterEnabled((data & 0x2) != 0);
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
float Chip<TInterruptHardware, TSoundHardware>::getMixedOutput() const {
    // Sum pulse channels (need this in case of division by 0, uint8_t is enough because pulse channels output no more than 15)
    uint8_t pulseSum = _pulse[0].getOutput() + _pulse[1].getOutput();
    
    // Get pulse channels mixed output
    float pulseOut = (pulseSum > 0) ? (95.88f / ((8128.0f / pulseSum) + 100.0f)) : 0;
    
    // Sum other channels (need this in case of division by 0)
    float triangleNoiseDmcMixedSum = (_triangle.getOutput() / 8227.0f) + (_noise.getOutput() / 12241.0f) + (_dmc.getOutput() / 22638.0f);
    
    // Get triangle, noise and DMC mixed output
    float triangleNoiseDmcOut = (triangleNoiseDmcMixedSum > 0) ? (159.79f / ((1.0f / triangleNoiseDmcMixedSum) + 100.0f)) : 0;
    
    // Get mixed output
    return pulseOut + triangleNoiseDmcOut;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::setPulseChannelRegister(uint8_t pulseChannelNumber, uint8_t registerNumber, uint8_t data) {
    // Set register
    _pulse[pulseChannelNumber].setRegister(registerNumber, data);
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
    // Clock channels
    _pulse[0].clockFrameCounterQuarterFrame();
    _pulse[1].clockFrameCounterQuarterFrame();
    _noise.envelopeUnit.clock();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::clockFrameCounterHalfFrame() {
    // Clock length counter channels
    _pulse[0].clockFrameCounterHalfFrame();
    _pulse[1].clockFrameCounterHalfFrame();
    _triangle.lengthCounter.clock();
    _noise.lengthCounter.clock();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::checkInterrupt() {
    _interruptHardware.irq(!_frameCounter.getInterrupt());
}

#endif /* NESEmu_Apu_s_hpp */
