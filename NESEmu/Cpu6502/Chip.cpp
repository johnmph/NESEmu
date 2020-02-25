//
//  Chip.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 20/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "Chip.hpp"


using namespace Cpu6502::_Detail;


// Alu

uint8_t Alu::getAInput() const {
    return _aInput;
}

uint8_t Alu::getBInput() const {
    return _bInput;
}

uint8_t Alu::getAdderHold() const {
    return _adderHold;
}

bool Alu::getOverflow() const {
    return _overflow;
}

bool Alu::getCarry() const {
    return _carry;
}

bool Alu::getHalfCarry() const {
    return _halfCarry;
}

void Alu::performAnd(uint8_t aInput, uint8_t bInput) {
    _aInput = aInput;
    _bInput = bInput;
    
    _adderHold = _aInput & _bInput;
}
void Alu::performOr(uint8_t aInput, uint8_t bInput) {
    _aInput = aInput;
    _bInput = bInput;
    
    _adderHold = _aInput | _bInput;
}

void Alu::performEor(uint8_t aInput, uint8_t bInput) {
    _aInput = aInput;
    _bInput = bInput;
    
    _adderHold = _aInput ^ _bInput;
}

void Alu::performShiftRight(uint8_t aInput, bool carryIn) {
    _aInput = aInput;
    
    _adderHold = (_aInput >> 1) | (carryIn << 7);
    
    // In real 6502, it's not _carry signal which is set but it's the first bit of DB bus which is used
    _carry = _aInput & 0x1;
}

void Alu::invertBInput() {
    _bInput = ~_bInput;
}

template <>
void Alu::performSubstractMode<false>() {
    // Does nothing
}

template <>
void Alu::performSubstractMode<true>() {
    invertBInput();
}


// Flags Helper

FlagsHelper::FlagsHelper(uint8_t &value, Alu &alu) : _value(value), _alu(alu) {
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
    set<Flag::Carry>(_alu.getCarry());
}

template <>
void FlagsHelper::refreshImpl<FlagsHelper::Flag::Overflow>() {
    // It's a copy of the ALU overflow
    set<Flag::Overflow>(_alu.getOverflow());
}
/*
template <>
void FlagsHelper::refreshImpl<FlagsHelper::Flag::UnusedHigh>() {
    // It is always set
    set<Flag::UnusedHigh>(true);
}*/
