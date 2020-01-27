//
//  Register_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Register_s_hpp
#define Cpu6502_Internal_Register_s_hpp


template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::transfert0(uint8_t source, uint8_t &destination) {
    destination = source;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(source);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::tax0() {
    _currentInstruction = &Chip::tax1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::tax1() {
    transfert0(_accumulator, _xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::txa0() {
    _currentInstruction = &Chip::txa1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::txa1() {
    transfert0(_xIndex, _accumulator);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::tay0() {
    _currentInstruction = &Chip::tay1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::tay1() {
    transfert0(_accumulator, _yIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::tya0() {
    _currentInstruction = &Chip::tya1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::tya1() {
    transfert0(_yIndex, _accumulator);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::tsx0() {
    _currentInstruction = &Chip::tsx1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::tsx1() {
    transfert0(_stackPointer, _xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::txs0() {
    _currentInstruction = &Chip::txs1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::txs1() {
    _stackPointer = _xIndex;
    
    // No flag changed during this instruction
    
    fetchOpcode();
}

#endif /* Cpu6502_Internal_Register_s_hpp */
