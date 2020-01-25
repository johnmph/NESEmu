//
//  Flag_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Flag_s_hpp
#define Cpu6502_Internal_Flag_s_hpp


template <class TBus, bool BDecimalSupported>
template <typename Chip<TBus, BDecimalSupported>::Flag flag>
void Chip<TBus, BDecimalSupported>::clearFlag() {
    // Must be called before changing flag to delay possible interrupts after the next instruction
    // See http://wiki.nesdev.com/w/index.php/CPU_interrupts
    fetchOpcode();
    
    _flagsHelper.clear<flag>();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::clc0() {
    _currentInstruction = &Chip::clc1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::clc1() {
    clearFlag<Flag::Carry>();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cld0() {
    _currentInstruction = &Chip::cld1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cld1() {
    clearFlag<Flag::DecimalMode>();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cli0() {
    _currentInstruction = &Chip::cli1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cli1() {
    clearFlag<Flag::InterruptDisable>();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::clv0() {
    _currentInstruction = &Chip::clv1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::clv1() {
    clearFlag<Flag::Overflow>();
}

template <class TBus, bool BDecimalSupported>
template <typename Chip<TBus, BDecimalSupported>::Flag flag>
void Chip<TBus, BDecimalSupported>::setFlag() {
    // Must be called before changing flag to delay possible interrupts after the next instruction
    // See http://wiki.nesdev.com/w/index.php/CPU_interrupts
    fetchOpcode();
    
    _flagsHelper.set<flag>(true);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sec0() {
    _currentInstruction = &Chip::sec1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sec1() {
    setFlag<Flag::Carry>();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sed0() {
    _currentInstruction = &Chip::sed1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sed1() {
    setFlag<Flag::DecimalMode>();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sei0() {
    _currentInstruction = &Chip::sei1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sei1() {
    setFlag<Flag::InterruptDisable>();
}

#endif /* Cpu6502_Internal_Flag_s_hpp */
