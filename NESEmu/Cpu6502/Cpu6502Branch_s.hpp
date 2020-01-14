//
//  Cpu6502Branch_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Branch_s_hpp
#define Cpu6502Branch_s_hpp


template <class TBus>
void Cpu6502<TBus>::bpl0() {
    _currentInstruction = &Cpu6502::bpl1;
    relative0();
}

template <class TBus>
void Cpu6502<TBus>::bpl1() {
    relative1(getStatusFlag(Flags::Negative) == false);
}

template <class TBus>
void Cpu6502<TBus>::bmi0() {
    _currentInstruction = &Cpu6502::bmi1;
    relative0();
}

template <class TBus>
void Cpu6502<TBus>::bmi1() {
    relative1(getStatusFlag(Flags::Negative) == true);
}

template <class TBus>
void Cpu6502<TBus>::bvc0() {
    _currentInstruction = &Cpu6502::bvc1;
    relative0();
}

template <class TBus>
void Cpu6502<TBus>::bvc1() {
    relative1(getStatusFlag(Flags::Overflow) == false);
}

template <class TBus>
void Cpu6502<TBus>::bvs0() {
    _currentInstruction = &Cpu6502::bvs1;
    relative0();
}

template <class TBus>
void Cpu6502<TBus>::bvs1() {
    relative1(getStatusFlag(Flags::Overflow) == true);
}

template <class TBus>
void Cpu6502<TBus>::bcc0() {
    _currentInstruction = &Cpu6502::bcc1;
    relative0();
}

template <class TBus>
void Cpu6502<TBus>::bcc1() {
    relative1(getStatusFlag(Flags::Carry) == false);
}

template <class TBus>
void Cpu6502<TBus>::bcs0() {
    _currentInstruction = &Cpu6502::bcs1;
    relative0();
}

template <class TBus>
void Cpu6502<TBus>::bcs1() {
    relative1(getStatusFlag(Flags::Carry) == true);
}

template <class TBus>
void Cpu6502<TBus>::bne0() {
    _currentInstruction = &Cpu6502::bne1;
    relative0();
}

template <class TBus>
void Cpu6502<TBus>::bne1() {
    relative1(getStatusFlag(Flags::Zero) == false);
}

template <class TBus>
void Cpu6502<TBus>::beq0() {
    _currentInstruction = &Cpu6502::beq1;
    relative0();
}

template <class TBus>
void Cpu6502<TBus>::beq1() {
    relative1(getStatusFlag(Flags::Zero) == true);
}

#endif /* Cpu6502Branch_s_hpp */
