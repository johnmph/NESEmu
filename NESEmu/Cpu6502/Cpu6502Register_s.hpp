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
    destination = source;
    
    // Update status
    clearStatusFlags({ Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Zero, (source == 0));
    setStatusFlag(Flags::Negative, (source & 0x80));
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::tax0() {
    _currentInstruction = &Cpu6502::tax1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::tax1() {
    transfert0(_accumulator, _xIndex);
}

template <class TBus>
void Cpu6502<TBus>::txa0() {
    _currentInstruction = &Cpu6502::txa1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::txa1() {
    transfert0(_xIndex, _accumulator);
}

template <class TBus>
void Cpu6502<TBus>::tay0() {
    _currentInstruction = &Cpu6502::tay1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::tay1() {
    transfert0(_accumulator, _yIndex);
}

template <class TBus>
void Cpu6502<TBus>::tya0() {
    _currentInstruction = &Cpu6502::tya1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::tya1() {
    transfert0(_yIndex, _accumulator);
}

template <class TBus>
void Cpu6502<TBus>::tsx0() {
    _currentInstruction = &Cpu6502::tsx1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::tsx1() {
    transfert0(_stackPointer, _xIndex);
}

template <class TBus>
void Cpu6502<TBus>::txs0() {
    _currentInstruction = &Cpu6502::txs1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::txs1() {
    _stackPointer = _xIndex;
    
    // No flag changed during this instruction
    
    fetchOpcode();
}

#endif /* Cpu6502Register_s_hpp */
