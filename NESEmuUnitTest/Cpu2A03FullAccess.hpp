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


template <NESEmu::Cpu::Model EModel, class TBus>
struct Cpu2A03FullAccess : NESEmu::Cpu::Chip<EModel, TBus> {
    
    using Cpu = NESEmu::Cpu::Chip<EModel, TBus>;
    
    Cpu2A03FullAccess(TBus &bus) : Cpu(bus) {
    }
    
    uint16_t getAddressBus() const {
        return this->_bus.getAddressBus();
    }
    
    uint8_t getDataBus() const {
        return this->_bus.getDataBus();
    }
    
    uint16_t getProgramCounter() const {
        return (this->_programCounterHigh << 8) | this->_programCounterLow;
    }
    
    uint8_t getStackPointer() const {
        return this->_stackPointer;
    }
    
    uint8_t getAccumulator() const {
        return this->_accumulator;
    }
    
    uint8_t getXIndex() const {
        return this->_xIndex;
    }
    
    uint8_t getYIndex() const {
        return this->_yIndex;
    }
    
    uint8_t getStatusFlags() const {
        return this->_statusFlags;
    }
    
    bool getReadyLine() const {
        return this->_readyLine;
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
    
};

#endif /* Cpu2A03FullAccess_hpp */
