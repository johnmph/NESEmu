//
//  Cpu6502FullAccess.hpp
//  NESEmuUnitTest
//
//  Created by Jonathan Baliko on 5/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502FullAccess_hpp
#define Cpu6502FullAccess_hpp

#include <cstdint>
#include "Cpu6502/Chip.hpp"


template <class TBus>
struct Cpu6502FullAccess : Cpu6502::Chip<Cpu6502::ConfigurationAccurate<TBus, Cpu6502FullAccess<TBus>>> {
    
    using Cpu = Cpu6502::Chip<Cpu6502::ConfigurationAccurate<TBus, Cpu6502FullAccess>>;
    
    Cpu6502FullAccess(TBus &bus) : Cpu(bus) {
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
};

#endif /* Cpu6502FullAccess_hpp */
