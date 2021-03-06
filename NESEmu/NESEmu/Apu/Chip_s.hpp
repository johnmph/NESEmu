//
//  Chip_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 18/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_Chip_s_hpp
#define NESEmu_Apu_Chip_s_hpp


template <class TCpu, class TSoundManager>
Chip<TCpu, TSoundManager>::Chip(TCpu &cpu, TSoundManager &soundManager) : _cpu(cpu), _soundManager(soundManager), _frameCounter(*this), _pulseChannel{ PulseChannel { true }, PulseChannel { false } }, _dmcChannel(*this) {
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::powerUp() {
    // Power up channels
    _pulseChannel[0].powerUp();
    _pulseChannel[1].powerUp();
    _triangleChannel.powerUp();
    _noiseChannel.powerUp();
    _dmcChannel.powerUp();
    
    // Set frame counter register with 0
    setFrameCounterRegister(0x0);
    
    // Reset line is high
    _resetLine = true;
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::clock() {
    // Clock frame counter
    _frameCounter.clock();//TODO: je l'ai mis avant les channels, a voir
    
    // Clock channels
    _pulseChannel[0].clock();
    _pulseChannel[1].clock();
    _triangleChannel.clock();
    _noiseChannel.clock();
    _dmcChannel.clock();
    
    // Check interrupt
    checkInterrupt();
    
    // Check reset
    checkReset();
    
    // Add data to sound manager if necessary
    if (_soundManager.askForAddingSample()) {
        _soundManager.addSample(getMixedOutput());
    }
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::reset(bool high) {
    // Save signal
    _resetLine = high;
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::setChannelRegister(uint16_t address, uint8_t data) {
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

template <class TCpu, class TSoundManager>
uint8_t Chip<TCpu, TSoundManager>::getStatusRegister() {
    uint8_t data = (_dmcChannel.getInterrupt() << 7) |
                   (_frameCounter.getInterrupt() << 6) |
                   (_dmcChannel.isEnabled() << 4) |
                   (_noiseChannel.getLengthCounterOutput() << 3) |
                   (_triangleChannel.getLengthCounterOutput() << 2) |
                   (_pulseChannel[1].getLengthCounterOutput() << 1) |
                   _pulseChannel[0].getLengthCounterOutput();
    
    // Reset frame counter interrupt
    _frameCounter.resetInterrupt();
    
    // TODO: If an interrupt flag was set at the same moment of the read, it will read back as 1 but it will not be cleared.
    
    return data;
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::setStatusRegister(uint8_t data) {
    // Get channels enable flags
    _pulseChannel[0].setLengthCounterEnabled((data & 0x1) != 0);
    _pulseChannel[1].setLengthCounterEnabled((data & 0x2) != 0);
    _triangleChannel.setLengthCounterEnabled((data & 0x4) != 0);
    _noiseChannel.setLengthCounterEnabled((data & 0x8) != 0);
    _dmcChannel.setEnabled((data & 0x10) != 0);
    
    // Reset Dmc interrupt flag
    _dmcChannel.resetInterrupt();
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::setFrameCounterRegister(uint8_t data) {
    _frameCounter.setDisableInterrupt(data & 0x40);
    _frameCounter.setSequence5StepMode(data & 0x80);
    
    _frameCounter.requestReset();
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::dmcSampleFetched(uint8_t data) {
    _dmcChannel.sampleFetched(data);
}

template <class TCpu, class TSoundManager>
float Chip<TCpu, TSoundManager>::getPulsesOutput() const {
    // Sum pulse channels (need this in case of division by 0, uint8_t is enough because pulse channels output no more than 15)
    uint8_t pulseSum = _pulseChannel[0].getOutput() + _pulseChannel[1].getOutput();
    
    // Get pulse channels mixed output
    return (pulseSum > 0) ? (95.88f / ((8128.0f / pulseSum) + 100.0f)) : 0;
}

template <class TCpu, class TSoundManager>
float Chip<TCpu, TSoundManager>::getTriangleNoiseDmcOutput() const {
    // Sum other channels (need this in case of division by 0)
    float triangleNoiseDmcMixedSum = (_triangleChannel.getOutput() / 8227.0f) + (_noiseChannel.getOutput() / 12241.0f) + (_dmcChannel.getOutput() / 22638.0f);
    
    // Get triangle, noise and DMC mixed output
    return (triangleNoiseDmcMixedSum > 0) ? (159.79f / ((1.0f / triangleNoiseDmcMixedSum) + 100.0f)) : 0;
}

template <class TCpu, class TSoundManager>
float Chip<TCpu, TSoundManager>::getMixedOutput() const {//TODO: voir si plus optimisé avec les approximations, TODO: plutot dans NES ?
    // Get mixed output
    return getPulsesOutput() + getTriangleNoiseDmcOutput();
    
    /*float pulseOut = 0.00752f * (_pulseChannel[0].getOutput() + _pulseChannel[1].getOutput());      // TODO: version avec approximation
    
    float triangleNoiseDmcOut = (0.00851f * _triangleChannel.getOutput()) + (0.00494f * _noiseChannel.getOutput()) + (0.00335f * _dmcChannel.getOutput());
    
    return pulseOut + triangleNoiseDmcOut;*/
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::clockFrameCounterQuarterFrame() {
    // Clock channels
    _pulseChannel[0].clockFrameCounterQuarterFrame();
    _pulseChannel[1].clockFrameCounterQuarterFrame();
    _triangleChannel.clockFrameCounterQuarterFrame();
    _noiseChannel.clockFrameCounterQuarterFrame();
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::clockFrameCounterHalfFrame() {
    // Clock length counter channels
    _pulseChannel[0].clockFrameCounterHalfFrame();
    _pulseChannel[1].clockFrameCounterHalfFrame();
    _triangleChannel.clockFrameCounterHalfFrame();
    _noiseChannel.clockFrameCounterHalfFrame();
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::requestDmcSample(uint16_t address) {
    _cpu.apuDmcRequestSample(address);
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::checkInterrupt() {
    _cpu.apuIrq(!(_frameCounter.getInterrupt() || _dmcChannel.getInterrupt()));
}

template <class TCpu, class TSoundManager>
void Chip<TCpu, TSoundManager>::checkReset() {
    // Only reset if reset line low
    if (_resetLine) {
        return;
    }
    
    // Reset channels
    setStatusRegister(0x0);
    
    // Reset frame counter
    _frameCounter.resetInterrupt();
    _frameCounter.requestReset();
}

#endif /* NESEmu_Apu_Chip_s_hpp */
