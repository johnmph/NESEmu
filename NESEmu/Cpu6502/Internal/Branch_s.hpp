//
//  Branch_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Branch_s_hpp
#define Cpu6502_Internal_Branch_s_hpp


template <class TConfiguration>
void Chip<TConfiguration>::bpl0() {
    _currentInstruction = &Chip::bpl1;
    relative0();
}

template <class TConfiguration>
void Chip<TConfiguration>::bpl1() {
    relative1(_flagsHelper.get<Flag::Negative>() == false);
}

template <class TConfiguration>
void Chip<TConfiguration>::bmi0() {
    _currentInstruction = &Chip::bmi1;
    relative0();
}

template <class TConfiguration>
void Chip<TConfiguration>::bmi1() {
    relative1(_flagsHelper.get<Flag::Negative>() == true);
}

template <class TConfiguration>
void Chip<TConfiguration>::bvc0() {
    _currentInstruction = &Chip::bvc1;
    relative0();
}

template <class TConfiguration>
void Chip<TConfiguration>::bvc1() {
    relative1(_flagsHelper.get<Flag::Overflow>() == false);
}

template <class TConfiguration>
void Chip<TConfiguration>::bvs0() {
    _currentInstruction = &Chip::bvs1;
    relative0();
}

template <class TConfiguration>
void Chip<TConfiguration>::bvs1() {
    relative1(_flagsHelper.get<Flag::Overflow>() == true);
}

template <class TConfiguration>
void Chip<TConfiguration>::bcc0() {
    _currentInstruction = &Chip::bcc1;
    relative0();
}

template <class TConfiguration>
void Chip<TConfiguration>::bcc1() {
    relative1(_flagsHelper.get<Flag::Carry>() == false);
}

template <class TConfiguration>
void Chip<TConfiguration>::bcs0() {
    _currentInstruction = &Chip::bcs1;
    relative0();
}

template <class TConfiguration>
void Chip<TConfiguration>::bcs1() {
    relative1(_flagsHelper.get<Flag::Carry>() == true);
}

template <class TConfiguration>
void Chip<TConfiguration>::bne0() {
    _currentInstruction = &Chip::bne1;
    relative0();
}

template <class TConfiguration>
void Chip<TConfiguration>::bne1() {
    relative1(_flagsHelper.get<Flag::Zero>() == false);
}

template <class TConfiguration>
void Chip<TConfiguration>::beq0() {
    _currentInstruction = &Chip::beq1;
    relative0();
}

template <class TConfiguration>
void Chip<TConfiguration>::beq1() {
    relative1(_flagsHelper.get<Flag::Zero>() == true);
}

#endif /* Cpu6502_Internal_Branch_s_hpp */
