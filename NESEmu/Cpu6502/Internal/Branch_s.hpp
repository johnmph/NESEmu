//
//  Branch_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Branch_s_hpp
#define Cpu6502_Internal_Branch_s_hpp


template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bpl0() {
    _currentInstruction = &Chip::bpl1;
    relative0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bpl1() {
    relative1(_flagsHelper.get<Flag::Negative>() == false);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bmi0() {
    _currentInstruction = &Chip::bmi1;
    relative0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bmi1() {
    relative1(_flagsHelper.get<Flag::Negative>() == true);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bvc0() {
    _currentInstruction = &Chip::bvc1;
    relative0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bvc1() {
    relative1(_flagsHelper.get<Flag::Overflow>() == false);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bvs0() {
    _currentInstruction = &Chip::bvs1;
    relative0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bvs1() {
    relative1(_flagsHelper.get<Flag::Overflow>() == true);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bcc0() {
    _currentInstruction = &Chip::bcc1;
    relative0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bcc1() {
    relative1(_flagsHelper.get<Flag::Carry>() == false);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bcs0() {
    _currentInstruction = &Chip::bcs1;
    relative0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bcs1() {
    relative1(_flagsHelper.get<Flag::Carry>() == true);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bne0() {
    _currentInstruction = &Chip::bne1;
    relative0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bne1() {
    relative1(_flagsHelper.get<Flag::Zero>() == false);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::beq0() {
    _currentInstruction = &Chip::beq1;
    relative0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::beq1() {
    relative1(_flagsHelper.get<Flag::Zero>() == true);
}

#endif /* Cpu6502_Internal_Branch_s_hpp */
