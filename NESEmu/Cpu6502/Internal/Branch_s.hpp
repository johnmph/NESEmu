//
//  Branch_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Branch_s_hpp
#define Cpu6502_Internal_Branch_s_hpp


template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bpl0() {
    _currentInstruction = &Chip::bpl1;
    relative0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bpl1() {
    relative1(_flagsHelper.get<Flag::Negative>() == false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bmi0() {
    _currentInstruction = &Chip::bmi1;
    relative0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bmi1() {
    relative1(_flagsHelper.get<Flag::Negative>() == true);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bvc0() {
    _currentInstruction = &Chip::bvc1;
    relative0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bvc1() {
    relative1(_flagsHelper.get<Flag::Overflow>() == false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bvs0() {
    _currentInstruction = &Chip::bvs1;
    relative0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bvs1() {
    relative1(_flagsHelper.get<Flag::Overflow>() == true);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bcc0() {
    _currentInstruction = &Chip::bcc1;
    relative0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bcc1() {
    relative1(_flagsHelper.get<Flag::Carry>() == false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bcs0() {
    _currentInstruction = &Chip::bcs1;
    relative0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bcs1() {
    relative1(_flagsHelper.get<Flag::Carry>() == true);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bne0() {
    _currentInstruction = &Chip::bne1;
    relative0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bne1() {
    relative1(_flagsHelper.get<Flag::Zero>() == false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::beq0() {
    _currentInstruction = &Chip::beq1;
    relative0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::beq1() {
    relative1(_flagsHelper.get<Flag::Zero>() == true);
}

#endif /* Cpu6502_Internal_Branch_s_hpp */
