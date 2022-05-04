//
//  Flag_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Flag_s_hpp
#define Cpu6502_Internal_Flag_s_hpp


template <class TConfiguration>
template <typename Chip<TConfiguration>::Flag flag>
void Chip<TConfiguration>::clearFlag() {
    _flagsHelper.clear<flag>();
    
    fetchOpcode();
}

template <class TConfiguration>
void Chip<TConfiguration>::clc0() {
    _currentInstruction = &Chip::clc1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::clc1() {
    clearFlag<Flag::Carry>();
}

template <class TConfiguration>
void Chip<TConfiguration>::cld0() {
    _currentInstruction = &Chip::cld1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::cld1() {
    clearFlag<Flag::DecimalMode>();
}

template <class TConfiguration>
void Chip<TConfiguration>::cli0() {
    _currentInstruction = &Chip::cli1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::cli1() {
    clearFlag<Flag::InterruptDisable>();
}

template <class TConfiguration>
void Chip<TConfiguration>::clv0() {
    _currentInstruction = &Chip::clv1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::clv1() {
    clearFlag<Flag::Overflow>();
}

template <class TConfiguration>
template <typename Chip<TConfiguration>::Flag flag>
void Chip<TConfiguration>::setFlag() {
    _flagsHelper.set<flag>(true);
    
    fetchOpcode();
}

template <class TConfiguration>
void Chip<TConfiguration>::sec0() {
    _currentInstruction = &Chip::sec1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::sec1() {
    setFlag<Flag::Carry>();
}

template <class TConfiguration>
void Chip<TConfiguration>::sed0() {
    _currentInstruction = &Chip::sed1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::sed1() {
    setFlag<Flag::DecimalMode>();
}

template <class TConfiguration>
void Chip<TConfiguration>::sei0() {
    _currentInstruction = &Chip::sei1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::sei1() {
    setFlag<Flag::InterruptDisable>();
}

#endif /* Cpu6502_Internal_Flag_s_hpp */
