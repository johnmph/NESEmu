//
//  Cpu6502Flag_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Flag_s_hpp
#define Cpu6502Flag_s_hpp


template <class TBus>
template <typename Cpu6502<TBus>::Flag flag>
void Cpu6502<TBus>::clearFlag() {
    // Must be called before changing flag to delay possible interrupts after the next instruction
    // See http://wiki.nesdev.com/w/index.php/CPU_interrupts
    fetchOpcode();
    
    _flagsHelper.clear<flag>();
}

template <class TBus>
void Cpu6502<TBus>::clc0() {
    _currentInstruction = &Cpu6502::clc1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::clc1() {
    clearFlag<Flag::Carry>();
}

template <class TBus>
void Cpu6502<TBus>::cld0() {
    _currentInstruction = &Cpu6502::cld1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::cld1() {
    clearFlag<Flag::DecimalMode>();
}

template <class TBus>
void Cpu6502<TBus>::cli0() {
    _currentInstruction = &Cpu6502::cli1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::cli1() {
    clearFlag<Flag::InterruptDisable>();
}

template <class TBus>
void Cpu6502<TBus>::clv0() {
    _currentInstruction = &Cpu6502::clv1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::clv1() {
    clearFlag<Flag::Overflow>();
}

template <class TBus>
template <typename Cpu6502<TBus>::Flag flag>
void Cpu6502<TBus>::setFlag() {
    // Must be called before changing flag to delay possible interrupts after the next instruction
    // See http://wiki.nesdev.com/w/index.php/CPU_interrupts
    fetchOpcode();
    
    _flagsHelper.set<flag>(true);
}

template <class TBus>
void Cpu6502<TBus>::sec0() {
    _currentInstruction = &Cpu6502::sec1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::sec1() {
    setFlag<Flag::Carry>();
}

template <class TBus>
void Cpu6502<TBus>::sed0() {
    _currentInstruction = &Cpu6502::sed1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::sed1() {
    setFlag<Flag::DecimalMode>();
}

template <class TBus>
void Cpu6502<TBus>::sei0() {
    _currentInstruction = &Cpu6502::sei1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::sei1() {
    setFlag<Flag::InterruptDisable>();
}

#endif /* Cpu6502Flag_s_hpp */
