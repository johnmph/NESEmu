//
//  Chip_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 18/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_Chip_s_hpp
#define NESEmu_Apu_Chip_s_hpp


template <class TInterruptHardware, class TSoundHardware>
Chip<TInterruptHardware, TSoundHardware>::Chip(TInterruptHardware &interruptHardware, TSoundHardware &soundHardware) : _interruptHardware(interruptHardware), _soundHardware(soundHardware), _frameCounter(*this), _pulseChannel{ true, false } {
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::powerUp() {
    _pulseChannel[0].powerUp();
    _pulseChannel[1].powerUp();
    _triangleChannel.powerUp();
    _noiseChannel.powerUp();
    _dmcChannel.powerUp();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::reset(bool high) {
    // If line high, exit
    if (high == true) {
        return;
    }
    
    // Reset status register
    _pulseChannel[0].reset();
    _pulseChannel[1].reset();
    _triangleChannel.reset();
    _noiseChannel.reset();
    _dmcChannel.reset();
    _statusEnableDmc = false;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::clock() {
    // Clock pulse, noise and DMC channels on APU cycle
    if (_oddCycle) {
        _pulseChannel[0].clock();
        _pulseChannel[1].clock();
        _noiseChannel.clock();
        _dmcChannel.clock();
    }
    
    // Clock triangle channel on CPU cycle
    _triangleChannel.clock();
    
    // Clock frame counter
    _frameCounter.clock();
    
    // Toggle flag
    _oddCycle = !_oddCycle;
    
    // Check interrupt
    checkInterrupt();
    
    // Add data to sound hardware if necessary
    if (_soundHardware.askForAddingSample()) {
        _soundHardware.addSample(getMixedOutput());
    }
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::setChannelRegister(uint16_t address, uint8_t data) {
    // Pulse registers
    if (address < 0x4008) {
        _pulseChannel[(address >= 0x4004)].setRegister((address & 0x3), data);
    }
    // Triangle register
    else if (address < 0x400C) {
        _triangleChannel.setRegister((address & 0x3), data);
    }
    // Noise register
    else if (address < 0x4010) {
        _noiseChannel.setRegister((address & 0x3), data);
    }
    // DMC register
    else {
        _dmcChannel.setRegister((address & 0x3), data);
    }
}

template <class TInterruptHardware, class TSoundHardware>
uint8_t Chip<TInterruptHardware, TSoundHardware>::getStatusRegister() {
    uint8_t data = (false << 7) |
                   (_frameCounter.getInterrupt() << 6) |
                   (false << 4) |
                   (_noiseChannel.getLengthCounterOutput() << 3) |
                   (_triangleChannel.getLengthCounterOutput() << 2) |
                   (_pulseChannel[1].getLengthCounterOutput() << 1) |
                   _pulseChannel[0].getLengthCounterOutput();//TODO : continuer (DMC)
    
    // Reset frame counter interrupt
    _frameCounter.resetInterrupt();
    
    // TODO: If an interrupt flag was set at the same moment of the read, it will read back as 1 but it will not be cleared.
    
    return data;
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::setStatusRegister(uint8_t data) {
    // Get channels enable flags
    _pulseChannel[0].setLengthCounterEnabled((data & 0x1) != 0);
    _pulseChannel[1].setLengthCounterEnabled((data & 0x2) != 0);
    _triangleChannel.setLengthCounterEnabled((data & 0x4) != 0);
    _noiseChannel.setLengthCounterEnabled((data & 0x8) != 0);
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
float Chip<TInterruptHardware, TSoundHardware>::getMixedOutput() const {//TODO: voir si plus optimisé avec les approximations
    // Sum pulse channels (need this in case of division by 0, uint8_t is enough because pulse channels output no more than 15)
    uint8_t pulseSum = _pulseChannel[0].getOutput() + _pulseChannel[1].getOutput();
    
    // Get pulse channels mixed output
    float pulseOut = (pulseSum > 0) ? (95.88f / ((8128.0f / pulseSum) + 100.0f)) : 0;
    
    // Sum other channels (need this in case of division by 0)
    float triangleNoiseDmcMixedSum = (_triangleChannel.getOutput() / 8227.0f)/* + (_noiseChannel.getOutput() / 12241.0f) + (_dmcChannel.getOutput() / 22638.0f)*/;
    
    // Get triangle, noise and DMC mixed output
    float triangleNoiseDmcOut = (triangleNoiseDmcMixedSum > 0) ? (159.79f / ((1.0f / triangleNoiseDmcMixedSum) + 100.0f)) : 0;
    
    // Get mixed output
    return /*pulseOut + */triangleNoiseDmcOut;
    /*
    float pulseOut = 0.00752f * (_pulseChannel[0].getOutput() + _pulseChannel[1].getOutput());
    
    float triangleNoiseDmcOut = (0.00851f * _triangleChannel.getOutput()) + (0.00494f * _noiseChannel.getOutput()) + (0.00335f * _dmcChannel.getOutput());
    
    return pulseOut + triangleNoiseDmcOut;*/
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::clockFrameCounterQuarterFrame() {
    // Clock channels
    _pulseChannel[0].clockFrameCounterQuarterFrame();
    _pulseChannel[1].clockFrameCounterQuarterFrame();
    _triangleChannel.clockFrameCounterQuarterFrame();
    _noiseChannel.clockFrameCounterQuarterFrame();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::clockFrameCounterHalfFrame() {
    // Clock length counter channels
    _pulseChannel[0].clockFrameCounterHalfFrame();
    _pulseChannel[1].clockFrameCounterHalfFrame();
    _triangleChannel.clockFrameCounterHalfFrame();
    _noiseChannel.clockFrameCounterHalfFrame();
}

template <class TInterruptHardware, class TSoundHardware>
void Chip<TInterruptHardware, TSoundHardware>::checkInterrupt() {
    _interruptHardware.apuIrq(!_frameCounter.getInterrupt());
}

#endif /* NESEmu_Apu_Chip_s_hpp */
