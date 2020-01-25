//
//  Register_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Register_s_hpp
#define Cpu6502_Internal_Register_s_hpp


template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::transfert0(uint8_t source, uint8_t &destination) {
    destination = source;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(source);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::tax0() {
    _currentInstruction = &Chip::tax1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::tax1() {
    transfert0(_accumulator, _xIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::txa0() {
    _currentInstruction = &Chip::txa1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::txa1() {
    transfert0(_xIndex, _accumulator);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::tay0() {
    _currentInstruction = &Chip::tay1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::tay1() {
    transfert0(_accumulator, _yIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::tya0() {
    _currentInstruction = &Chip::tya1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::tya1() {
    transfert0(_yIndex, _accumulator);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::tsx0() {
    _currentInstruction = &Chip::tsx1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::tsx1() {
    transfert0(_stackPointer, _xIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::txs0() {
    _currentInstruction = &Chip::txs1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::txs1() {
    _stackPointer = _xIndex;
    
    // No flag changed during this instruction
    
    fetchOpcode();
}

#endif /* Cpu6502_Internal_Register_s_hpp */
