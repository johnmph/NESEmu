//
//  Flag_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Flag_s_hpp
#define Cpu6502_Internal_Flag_s_hpp


template <class TBus, bool DecimalSupported>
template <typename Chip<TBus, DecimalSupported>::Flag flag>
void Chip<TBus, DecimalSupported>::clearFlag() {
    // Must be called before changing flag to delay possible interrupts after the next instruction
    // See http://wiki.nesdev.com/w/index.php/CPU_interrupts
    fetchOpcode();
    
    _flagsHelper.clear<flag>();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::clc0() {
    _currentInstruction = &Chip::clc1;
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::clc1() {
    clearFlag<Flag::Carry>();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cld0() {
    _currentInstruction = &Chip::cld1;
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cld1() {
    clearFlag<Flag::DecimalMode>();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cli0() {
    _currentInstruction = &Chip::cli1;
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cli1() {
    clearFlag<Flag::InterruptDisable>();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::clv0() {
    _currentInstruction = &Chip::clv1;
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::clv1() {
    clearFlag<Flag::Overflow>();
}

template <class TBus, bool DecimalSupported>
template <typename Chip<TBus, DecimalSupported>::Flag flag>
void Chip<TBus, DecimalSupported>::setFlag() {
    // Must be called before changing flag to delay possible interrupts after the next instruction
    // See http://wiki.nesdev.com/w/index.php/CPU_interrupts
    fetchOpcode();
    
    _flagsHelper.set<flag>(true);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sec0() {
    _currentInstruction = &Chip::sec1;
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sec1() {
    setFlag<Flag::Carry>();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sed0() {
    _currentInstruction = &Chip::sed1;
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sed1() {
    setFlag<Flag::DecimalMode>();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sei0() {
    _currentInstruction = &Chip::sei1;
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sei1() {
    setFlag<Flag::InterruptDisable>();
}

#endif /* Cpu6502_Internal_Flag_s_hpp */
