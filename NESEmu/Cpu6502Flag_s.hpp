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
void Cpu6502<TBus>::clearFlag(Flags flag) {
    // Must be called before changing flag to delay possible interrupts after the next instruction (see http://wiki.nesdev.com/w/index.php/CPU_interrupts )
    fetchOpcode();
    
    clearStatusFlags({ flag });
}

template <class TBus>
void Cpu6502<TBus>::clc0() {
    _currentInstruction = &Cpu6502::clc1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::clc1() {
    clearFlag(Flags::Carry);
}

template <class TBus>
void Cpu6502<TBus>::cld0() {
    _currentInstruction = &Cpu6502::cld1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::cld1() {
    clearFlag(Flags::DecimalMode);
}

template <class TBus>
void Cpu6502<TBus>::cli0() {
    _currentInstruction = &Cpu6502::cli1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::cli1() {
    clearFlag(Flags::InterruptDisable);
}

template <class TBus>
void Cpu6502<TBus>::clv0() {
    _currentInstruction = &Cpu6502::clv1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::clv1() {
    clearFlag(Flags::Overflow);
}

template <class TBus>
void Cpu6502<TBus>::setFlag(Flags flag) {
    // Must be called before changing flag to delay possible interrupts after the next instruction (see http://wiki.nesdev.com/w/index.php/CPU_interrupts )
    fetchOpcode();
    
    setStatusFlag(flag, true);
}

template <class TBus>
void Cpu6502<TBus>::sec0() {
    _currentInstruction = &Cpu6502::sec1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::sec1() {
    setFlag(Flags::Carry);
}

template <class TBus>
void Cpu6502<TBus>::sed0() {
    _currentInstruction = &Cpu6502::sed1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::sed1() {
    setFlag(Flags::DecimalMode);
}

template <class TBus>
void Cpu6502<TBus>::sei0() {
    _currentInstruction = &Cpu6502::sei1;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::sei1() {
    setFlag(Flags::InterruptDisable);
}

#endif /* Cpu6502Flag_s_hpp */
