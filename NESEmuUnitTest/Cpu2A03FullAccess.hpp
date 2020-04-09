//
//  Cpu2A03FullAccess.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 8/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu2A03FullAccess_hpp
#define Cpu2A03FullAccess_hpp

#include <cstdint>
#include "NESEmu/Cpu.hpp"


template <NESEmu::Cpu::Model EModel, class TBus, class TSoundHardware>
struct Cpu2A03FullAccess : NESEmu::Cpu::Chip<EModel, TBus, TSoundHardware> {
    
    using Cpu = NESEmu::Cpu::Chip<EModel, TBus, TSoundHardware>;
    
    Cpu2A03FullAccess(TBus &bus, TSoundHardware &soundHardware) : Cpu(bus, soundHardware) {
    }
    
    uint16_t getAddressBus() const {
        return Cpu::_bus.getAddressBus();
    }
    
    uint8_t getDataBus() const {
        return Cpu::_bus.getDataBus();
    }
    
    void correctDataBusForVisual6502() {//TODO: a voir avec le bus interne !!!
        uint8_t data = this->_bus.getDataBus();
        
        if (Cpu::_readWrite == Cpu6502::ReadWrite::Write) {
            Cpu::_bus.setDataBus(_lastDataBusReadValue);
        } else {
            _lastDataBusReadValue = data;
        }
    }
    
    uint16_t getProgramCounter() const {
        return (Cpu::_programCounterHigh << 8) | Cpu::_programCounterLow;
    }
    
    uint8_t getStackPointer() const {
        return Cpu::_stackPointer;
    }
    
    uint8_t getAccumulator() const {
        return Cpu::_accumulator;
    }
    
    uint8_t getXIndex() const {
        return Cpu::_xIndex;
    }
    
    uint8_t getYIndex() const {
        return Cpu::_yIndex;
    }
    
    uint8_t getStatusFlags() const {
        return Cpu::_statusFlags;
    }
    
    bool getReadyLine() const {
        return Cpu::_readyLine;
    }
    
    bool getSyncSignal() const {
        return Cpu::getSyncSignal();
    }
    
    void setOverflow(bool high) {
        //Cpu::setOverflow(high);   // TODO: ce cpu n'utilise pas setOverflow
    }
    
    void ready(bool high) {
        Cpu::ready(high);
    }
    
    uint16_t getInternalAddressBus() const {
        return Cpu::getAddressBus();
    }
    
    uint8_t getInternalDataBus() const {
        return Cpu::getDataBus();
    }
    
    Cpu6502::ReadWrite getInternalReadWriteSignal() const {
        return Cpu::InternalCpu::getReadWriteSignal();
    }
    
private:
    uint8_t _lastDataBusReadValue;
};

#endif /* Cpu2A03FullAccess_hpp */
