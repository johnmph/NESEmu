//
//  Cpu6502Register_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Register_s_hpp
#define Cpu6502Register_s_hpp


template <class TBus>
void Cpu6502<TBus>::transfert0(uint8_t source, uint8_t &destination) {
    _currentInstruction = &Cpu6502::fetchOpcode;
    
    destination = source;
    
    // Update status
    clearStatusFlags({ Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Zero, (source == 0));
    setStatusFlag(Flags::Negative, (source & 0x80));
    
    implied();
}

template <class TBus>
void Cpu6502<TBus>::tax0() {
    transfert0(_accumulator, _xIndex);
}

template <class TBus>
void Cpu6502<TBus>::txa0() {
    transfert0(_xIndex, _accumulator);
}

template <class TBus>
void Cpu6502<TBus>::tay0() {
    transfert0(_accumulator, _yIndex);
}

template <class TBus>
void Cpu6502<TBus>::tya0() {
    transfert0(_yIndex, _accumulator);
}

template <class TBus>
void Cpu6502<TBus>::tsx0() {
    transfert0(_stackPointer, _xIndex);
}

template <class TBus>
void Cpu6502<TBus>::txs0() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    
    _stackPointer = _xIndex;
    
    // No flag changed during this instruction
    
    implied();
}

#endif /* Cpu6502Register_s_hpp */