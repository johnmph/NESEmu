//
//  Flag_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Flag_s_hpp
#define Cpu6502_Internal_Flag_s_hpp


template <class TBus, class TInternalHardware, bool BDecimalSupported>
template <typename Chip<TBus, TInternalHardware, BDecimalSupported>::Flag flag>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clearFlag() {
    _flagsHelper.clear<flag>();
    
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clc0() {
    _currentInstruction = &Chip::clc1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clc1() {
    clearFlag<Flag::Carry>();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cld0() {
    _currentInstruction = &Chip::cld1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cld1() {
    clearFlag<Flag::DecimalMode>();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cli0() {
    _currentInstruction = &Chip::cli1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cli1() {
    clearFlag<Flag::InterruptDisable>();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clv0() {
    _currentInstruction = &Chip::clv1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clv1() {
    clearFlag<Flag::Overflow>();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
template <typename Chip<TBus, TInternalHardware, BDecimalSupported>::Flag flag>
void Chip<TBus, TInternalHardware, BDecimalSupported>::setFlag() {
    _flagsHelper.set<flag>(true);
    
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sec0() {
    _currentInstruction = &Chip::sec1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sec1() {
    setFlag<Flag::Carry>();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sed0() {
    _currentInstruction = &Chip::sed1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sed1() {
    setFlag<Flag::DecimalMode>();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sei0() {
    _currentInstruction = &Chip::sei1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sei1() {
    setFlag<Flag::InterruptDisable>();
}

#endif /* Cpu6502_Internal_Flag_s_hpp */
