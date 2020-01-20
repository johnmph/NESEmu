//
//  Arithmetic_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Arithmetic_s_hpp
#define Cpu6502_Internal_Arithmetic_s_hpp


template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::arithmetic1() {
    // Store ALU result in accumulator
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Overflow, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adc0() {
    // Adding data to accumulator with possible carry
    _alu.performSum<DecimalSupported, false>(_accumulator, _inputDataLatch, _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::arithmetic1);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcImm0() {
    _currentInstruction = &Chip::adc0;
    immediate();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcZp0() {
    _currentInstruction = &Chip::adcZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcZp1() {
    _currentInstruction = &Chip::adc0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcZpX0() {
    _currentInstruction = &Chip::adcZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcZpX1() {
    _currentInstruction = &Chip::adcZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcZpX2() {
    _currentInstruction = &Chip::adc0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcAbs0() {
    _currentInstruction = &Chip::adcAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcAbs1() {
    _currentInstruction = &Chip::adcAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcAbs2() {
    _currentInstruction = &Chip::adc0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcAbsX0() {
    _currentInstruction = &Chip::adcAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcAbsX1() {
    _currentInstruction = &Chip::adcAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::adcAbsX3;
    absoluteIndexedLoad0(&Chip::adc0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcAbsX3() {
    _currentInstruction = &Chip::adc0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcAbsY0() {
    _currentInstruction = &Chip::adcAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcAbsY1() {
    _currentInstruction = &Chip::adcAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::adcAbsY3;
    absoluteIndexedLoad0(&Chip::adc0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcAbsY3() {
    _currentInstruction = &Chip::adc0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcIndX0() {
    _currentInstruction = &Chip::adcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcIndX1() {
    _currentInstruction = &Chip::adcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcIndX2() {
    _currentInstruction = &Chip::adcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcIndX3() {
    _currentInstruction = &Chip::adcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcIndX4() {
    _currentInstruction = &Chip::adc0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcIndY0() {
    _currentInstruction = &Chip::adcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcIndY1() {
    _currentInstruction = &Chip::adcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcIndY2() {
    _currentInstruction = &Chip::adcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::adcIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::adc0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::adcIndY4() {
    _currentInstruction = &Chip::adc0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbc0() {
    // Substracting data to accumulator with possible carry by inverting bInput
    _alu.performSum<DecimalSupported, true>(_accumulator, _inputDataLatch, _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::arithmetic1);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcImm0() {
    _currentInstruction = &Chip::sbc0;
    immediate();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcZp0() {
    _currentInstruction = &Chip::sbcZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcZp1() {
    _currentInstruction = &Chip::sbc0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcZpX0() {
    _currentInstruction = &Chip::sbcZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcZpX1() {
    _currentInstruction = &Chip::sbcZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcZpX2() {
    _currentInstruction = &Chip::sbc0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcAbs0() {
    _currentInstruction = &Chip::sbcAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcAbs1() {
    _currentInstruction = &Chip::sbcAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcAbs2() {
    _currentInstruction = &Chip::sbc0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcAbsX0() {
    _currentInstruction = &Chip::sbcAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcAbsX1() {
    _currentInstruction = &Chip::sbcAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::sbcAbsX3;
    absoluteIndexedLoad0(&Chip::sbc0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcAbsX3() {
    _currentInstruction = &Chip::sbc0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcAbsY0() {
    _currentInstruction = &Chip::sbcAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcAbsY1() {
    _currentInstruction = &Chip::sbcAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::sbcAbsY3;
    absoluteIndexedLoad0(&Chip::sbc0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcAbsY3() {
    _currentInstruction = &Chip::sbc0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcIndX0() {
    _currentInstruction = &Chip::sbcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcIndX1() {
    _currentInstruction = &Chip::sbcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcIndX2() {
    _currentInstruction = &Chip::sbcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcIndX3() {
    _currentInstruction = &Chip::sbcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcIndX4() {
    _currentInstruction = &Chip::sbc0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcIndY0() {
    _currentInstruction = &Chip::sbcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcIndY1() {
    _currentInstruction = &Chip::sbcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcIndY2() {
    _currentInstruction = &Chip::sbcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::sbcIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::sbc0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::sbcIndY4() {
    _currentInstruction = &Chip::sbc0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cp0(uint8_t data) {
    // Substracting data fetched to data to compare them
    _alu.performSum<DecimalSupported, true>(data, _inputDataLatch, false, true);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::cp1);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cp1() {
    // Don't save result, it's just to set the flags
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmp0() {
    cp0(_accumulator);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpImm0() {
    _currentInstruction = &Chip::cmp0;
    immediate();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpZp0() {
    _currentInstruction = &Chip::cmpZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpZp1() {
    _currentInstruction = &Chip::cmp0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpZpX0() {
    _currentInstruction = &Chip::cmpZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpZpX1() {
    _currentInstruction = &Chip::cmpZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpZpX2() {
    _currentInstruction = &Chip::cmp0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpAbs0() {
    _currentInstruction = &Chip::cmpAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpAbs1() {
    _currentInstruction = &Chip::cmpAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpAbs2() {
    _currentInstruction = &Chip::cmp0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpAbsX0() {
    _currentInstruction = &Chip::cmpAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpAbsX1() {
    _currentInstruction = &Chip::cmpAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::cmpAbsX3;
    absoluteIndexedLoad0(&Chip::cmp0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpAbsX3() {
    _currentInstruction = &Chip::cmp0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpAbsY0() {
    _currentInstruction = &Chip::cmpAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpAbsY1() {
    _currentInstruction = &Chip::cmpAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::cmpAbsY3;
    absoluteIndexedLoad0(&Chip::cmp0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpAbsY3() {
    _currentInstruction = &Chip::cmp0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpIndX0() {
    _currentInstruction = &Chip::cmpIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpIndX1() {
    _currentInstruction = &Chip::cmpIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpIndX2() {
    _currentInstruction = &Chip::cmpIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpIndX3() {
    _currentInstruction = &Chip::cmpIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpIndX4() {
    _currentInstruction = &Chip::cmp0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpIndY0() {
    _currentInstruction = &Chip::cmpIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpIndY1() {
    _currentInstruction = &Chip::cmpIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpIndY2() {
    _currentInstruction = &Chip::cmpIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::cmpIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::cmp0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cmpIndY4() {
    _currentInstruction = &Chip::cmp0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpx0() {
    cp0(_xIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpxImm0() {
    _currentInstruction = &Chip::cpx0;
    immediate();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpxZp0() {
    _currentInstruction = &Chip::cpxZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpxZp1() {
    _currentInstruction = &Chip::cpx0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpxAbs0() {
    _currentInstruction = &Chip::cpxAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpxAbs1() {
    _currentInstruction = &Chip::cpxAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpxAbs2() {
    _currentInstruction = &Chip::cpx0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpy0() {
    cp0(_yIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpyImm0() {
    _currentInstruction = &Chip::cpy0;
    immediate();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpyZp0() {
    _currentInstruction = &Chip::cpyZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpyZp1() {
    _currentInstruction = &Chip::cpy0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpyAbs0() {
    _currentInstruction = &Chip::cpyAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpyAbs1() {
    _currentInstruction = &Chip::cpyAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::cpyAbs2() {
    _currentInstruction = &Chip::cpy0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::dec(uint8_t data) {
    // Removing 1 from data using ALU (Add 0xFF without carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _alu.performSum<DecimalSupported, false>(0xFF, data, false, false);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::inc(uint8_t data) {
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _alu.performSum<DecimalSupported, false>(0x0, data, false, true);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incdecMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decMemory0() {
    _currentInstruction = &Chip::incdecMemory1;
    
    dec(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decZp0() {
    _currentInstruction = &Chip::decZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decZp1() {
    _currentInstruction = &Chip::decMemory0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decZpX0() {
    _currentInstruction = &Chip::decZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decZpX1() {
    _currentInstruction = &Chip::decZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decZpX2() {
    _currentInstruction = &Chip::decMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decAbs0() {
    _currentInstruction = &Chip::decAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decAbs1() {
    _currentInstruction = &Chip::decAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decAbs2() {
    _currentInstruction = &Chip::decMemory0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decAbsX0() {
    _currentInstruction = &Chip::decAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decAbsX1() {
    _currentInstruction = &Chip::decAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decAbsX2() {
    _currentInstruction = &Chip::decAbsX3;
    absoluteIndexedLoad0(&Chip::decAbsX3);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decAbsX3() {
    _currentInstruction = &Chip::decMemory0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incMemory0() {
    _currentInstruction = &Chip::incdecMemory1;
    
    inc(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incZp0() {
    _currentInstruction = &Chip::incZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incZp1() {
    _currentInstruction = &Chip::incMemory0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incZpX0() {
    _currentInstruction = &Chip::incZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incZpX1() {
    _currentInstruction = &Chip::incZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incZpX2() {
    _currentInstruction = &Chip::incMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incAbs0() {
    _currentInstruction = &Chip::incAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incAbs1() {
    _currentInstruction = &Chip::incAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incAbs2() {
    _currentInstruction = &Chip::incMemory0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incAbsX0() {
    _currentInstruction = &Chip::incAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incAbsX1() {
    _currentInstruction = &Chip::incAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incAbsX2() {
    _currentInstruction = &Chip::incAbsX3;
    absoluteIndexedLoad0(&Chip::incAbsX3);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incAbsX3() {
    _currentInstruction = &Chip::incMemory0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incdecRegister2(uint8_t &data) {
    // Write result back
    data = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::decRegister1(OpcodeInstruction nextInstruction, uint8_t data) {
    dec(data);
    
    // Fetch opcode during performing ALU
    fetchOpcode(nextInstruction);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::dex0() {
    _currentInstruction = &Chip::dex1;
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::dex1() {
    decRegister1(&Chip::dex2, _xIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::dex2() {
    incdecRegister2(_xIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::dey0() {
    _currentInstruction = &Chip::dey1;
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::dey1() {
    decRegister1(&Chip::dey2, _yIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::dey2() {
    incdecRegister2(_yIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::incRegister1(OpcodeInstruction nextInstruction, uint8_t data) {
    inc(data);
    
    // Fetch opcode during performing ALU
    fetchOpcode(nextInstruction);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::inx0() {
    _currentInstruction = &Chip::inx1;
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::inx1() {
    incRegister1(&Chip::inx2, _xIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::inx2() {
    incdecRegister2(_xIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::iny0() {
    _currentInstruction = &Chip::iny1;
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::iny1() {
    incRegister1(&Chip::iny2, _yIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::iny2() {
    incdecRegister2(_yIndex);
}

#endif /* Cpu6502_Internal_Arithmetic_s_hpp */
