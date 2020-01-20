//
//  Chip.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 20/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "Chip.hpp"


using namespace Cpu6502::_Detail;


FlagsHelper::FlagsHelper(uint8_t &value, bool &carry, bool &overflow) : _value(value), _carry(carry), _overflow(overflow) {
}

template <>
void FlagsHelper::refreshImpl<FlagsHelper::Flag::Negative>(uint8_t data) {
    // Set if data is negative (MSB is 1)
    set<Flag::Negative>(data & 0x80);
}

template <>
void FlagsHelper::refreshImpl<FlagsHelper::Flag::Zero>(uint8_t data) {
    // Set if data is zero
    set<Flag::Zero>(data == 0);
}

template <>
void FlagsHelper::refreshImpl<FlagsHelper::Flag::Carry>() {
    // It's a copy of the ALU carry
    set<Flag::Carry>(_carry);
}

template <>
void FlagsHelper::refreshImpl<FlagsHelper::Flag::Overflow>() {
    // It's a copy of the ALU overflow
    set<Flag::Overflow>(_overflow);
}
/*
template <>
void FlagsHelper::refreshImpl<FlagsHelper::Flag::UnusedHigh>() {
    // It is always set
    set<Flag::UnusedHigh>(true);
}*/
