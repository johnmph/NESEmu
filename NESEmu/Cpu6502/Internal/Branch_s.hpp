//
//  Branch_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Branch_s_hpp
#define Cpu6502_Internal_Branch_s_hpp


template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bpl0() {
    _currentInstruction = &Chip::bpl1;
    relative0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bpl1() {
    relative1(_flagsHelper.get<Flag::Negative>() == false);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bmi0() {
    _currentInstruction = &Chip::bmi1;
    relative0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bmi1() {
    relative1(_flagsHelper.get<Flag::Negative>() == true);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bvc0() {
    _currentInstruction = &Chip::bvc1;
    relative0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bvc1() {
    relative1(_flagsHelper.get<Flag::Overflow>() == false);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bvs0() {
    _currentInstruction = &Chip::bvs1;
    relative0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bvs1() {
    relative1(_flagsHelper.get<Flag::Overflow>() == true);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bcc0() {
    _currentInstruction = &Chip::bcc1;
    relative0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bcc1() {
    relative1(_flagsHelper.get<Flag::Carry>() == false);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bcs0() {
    _currentInstruction = &Chip::bcs1;
    relative0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bcs1() {
    relative1(_flagsHelper.get<Flag::Carry>() == true);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bne0() {
    _currentInstruction = &Chip::bne1;
    relative0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::bne1() {
    relative1(_flagsHelper.get<Flag::Zero>() == false);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::beq0() {
    _currentInstruction = &Chip::beq1;
    relative0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::beq1() {
    relative1(_flagsHelper.get<Flag::Zero>() == true);
}

#endif /* Cpu6502_Internal_Branch_s_hpp */
