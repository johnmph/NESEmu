//
//  Cpu6502InternalViewer.hpp
//  NESEmuUnitTest
//
//  Created by Jonathan Baliko on 5/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502InternalViewer_hpp
#define Cpu6502InternalViewer_hpp

#include <cstdint>
#include "Cpu6502/Chip.hpp"


template <class TBus>
struct Cpu6502InternalViewer {
    
    using Chip = Cpu6502::Chip<Cpu6502::ConfigurationAccurate<TBus, Cpu6502InternalViewer>>;
    
    
    Cpu6502InternalViewer(Chip const &cpu6502) : _cpu6502(cpu6502) {
    }
    
    Chip const &getCpu6502() const {
        return _cpu6502;
    }
    
    uint16_t getProgramCounter() const {
        return (_cpu6502._programCounterHigh << 8) | _cpu6502._programCounterLow;
    }
    
    uint8_t getStackPointer() const {
        return _cpu6502._stackPointer;
    }
    
    uint8_t getAccumulator() const {
        return _cpu6502._accumulator;
    }
    
    uint8_t getXIndex() const {
        return _cpu6502._xIndex;
    }
    
    uint8_t getYIndex() const {
        return _cpu6502._yIndex;
    }
    
    uint8_t getStatusFlags() const {
        return _cpu6502._statusFlags;
    }
    
private:
    Chip const &_cpu6502;
};

#endif /* Cpu6502InternalViewer_hpp */
