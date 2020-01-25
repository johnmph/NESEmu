//
//  Arithmetic_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Arithmetic_s_hpp
#define Cpu6502_Internal_Arithmetic_s_hpp


template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::arithmetic1() {
    // Store ALU result in accumulator
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Overflow, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adc0() {
    // Adding data to accumulator with possible carry
    _alu.performSum<BDecimalSupported, false>(_accumulator, _inputDataLatch, _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::arithmetic1);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcImm0() {
    _currentInstruction = &Chip::adc0;
    immediate();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcZp0() {
    _currentInstruction = &Chip::adcZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcZp1() {
    _currentInstruction = &Chip::adc0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcZpX0() {
    _currentInstruction = &Chip::adcZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcZpX1() {
    _currentInstruction = &Chip::adcZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcZpX2() {
    _currentInstruction = &Chip::adc0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcAbs0() {
    _currentInstruction = &Chip::adcAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcAbs1() {
    _currentInstruction = &Chip::adcAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcAbs2() {
    _currentInstruction = &Chip::adc0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcAbsX0() {
    _currentInstruction = &Chip::adcAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcAbsX1() {
    _currentInstruction = &Chip::adcAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::adcAbsX3;
    absoluteIndexedLoad0(&Chip::adc0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcAbsX3() {
    _currentInstruction = &Chip::adc0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcAbsY0() {
    _currentInstruction = &Chip::adcAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcAbsY1() {
    _currentInstruction = &Chip::adcAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::adcAbsY3;
    absoluteIndexedLoad0(&Chip::adc0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcAbsY3() {
    _currentInstruction = &Chip::adc0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcIndX0() {
    _currentInstruction = &Chip::adcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcIndX1() {
    _currentInstruction = &Chip::adcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcIndX2() {
    _currentInstruction = &Chip::adcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcIndX3() {
    _currentInstruction = &Chip::adcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcIndX4() {
    _currentInstruction = &Chip::adc0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcIndY0() {
    _currentInstruction = &Chip::adcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcIndY1() {
    _currentInstruction = &Chip::adcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcIndY2() {
    _currentInstruction = &Chip::adcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::adcIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::adc0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::adcIndY4() {
    _currentInstruction = &Chip::adc0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbc0() {
    // Substracting data to accumulator with possible carry by inverting bInput
    _alu.performSum<BDecimalSupported, true>(_accumulator, _inputDataLatch, _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::arithmetic1);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcImm0() {
    _currentInstruction = &Chip::sbc0;
    immediate();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcZp0() {
    _currentInstruction = &Chip::sbcZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcZp1() {
    _currentInstruction = &Chip::sbc0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcZpX0() {
    _currentInstruction = &Chip::sbcZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcZpX1() {
    _currentInstruction = &Chip::sbcZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcZpX2() {
    _currentInstruction = &Chip::sbc0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcAbs0() {
    _currentInstruction = &Chip::sbcAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcAbs1() {
    _currentInstruction = &Chip::sbcAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcAbs2() {
    _currentInstruction = &Chip::sbc0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcAbsX0() {
    _currentInstruction = &Chip::sbcAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcAbsX1() {
    _currentInstruction = &Chip::sbcAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::sbcAbsX3;
    absoluteIndexedLoad0(&Chip::sbc0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcAbsX3() {
    _currentInstruction = &Chip::sbc0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcAbsY0() {
    _currentInstruction = &Chip::sbcAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcAbsY1() {
    _currentInstruction = &Chip::sbcAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::sbcAbsY3;
    absoluteIndexedLoad0(&Chip::sbc0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcAbsY3() {
    _currentInstruction = &Chip::sbc0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcIndX0() {
    _currentInstruction = &Chip::sbcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcIndX1() {
    _currentInstruction = &Chip::sbcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcIndX2() {
    _currentInstruction = &Chip::sbcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcIndX3() {
    _currentInstruction = &Chip::sbcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcIndX4() {
    _currentInstruction = &Chip::sbc0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcIndY0() {
    _currentInstruction = &Chip::sbcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcIndY1() {
    _currentInstruction = &Chip::sbcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcIndY2() {
    _currentInstruction = &Chip::sbcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::sbcIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::sbc0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::sbcIndY4() {
    _currentInstruction = &Chip::sbc0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cp0(uint8_t data) {
    // Substracting data fetched to data to compare them
    _alu.performSum<BDecimalSupported, true>(data, _inputDataLatch, false, true);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::cp1);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cp1() {
    // Don't save result, it's just to set the flags
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmp0() {
    cp0(_accumulator);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpImm0() {
    _currentInstruction = &Chip::cmp0;
    immediate();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpZp0() {
    _currentInstruction = &Chip::cmpZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpZp1() {
    _currentInstruction = &Chip::cmp0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpZpX0() {
    _currentInstruction = &Chip::cmpZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpZpX1() {
    _currentInstruction = &Chip::cmpZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpZpX2() {
    _currentInstruction = &Chip::cmp0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpAbs0() {
    _currentInstruction = &Chip::cmpAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpAbs1() {
    _currentInstruction = &Chip::cmpAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpAbs2() {
    _currentInstruction = &Chip::cmp0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpAbsX0() {
    _currentInstruction = &Chip::cmpAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpAbsX1() {
    _currentInstruction = &Chip::cmpAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::cmpAbsX3;
    absoluteIndexedLoad0(&Chip::cmp0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpAbsX3() {
    _currentInstruction = &Chip::cmp0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpAbsY0() {
    _currentInstruction = &Chip::cmpAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpAbsY1() {
    _currentInstruction = &Chip::cmpAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::cmpAbsY3;
    absoluteIndexedLoad0(&Chip::cmp0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpAbsY3() {
    _currentInstruction = &Chip::cmp0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpIndX0() {
    _currentInstruction = &Chip::cmpIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpIndX1() {
    _currentInstruction = &Chip::cmpIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpIndX2() {
    _currentInstruction = &Chip::cmpIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpIndX3() {
    _currentInstruction = &Chip::cmpIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpIndX4() {
    _currentInstruction = &Chip::cmp0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpIndY0() {
    _currentInstruction = &Chip::cmpIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpIndY1() {
    _currentInstruction = &Chip::cmpIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpIndY2() {
    _currentInstruction = &Chip::cmpIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::cmpIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::cmp0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cmpIndY4() {
    _currentInstruction = &Chip::cmp0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpx0() {
    cp0(_xIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpxImm0() {
    _currentInstruction = &Chip::cpx0;
    immediate();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpxZp0() {
    _currentInstruction = &Chip::cpxZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpxZp1() {
    _currentInstruction = &Chip::cpx0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpxAbs0() {
    _currentInstruction = &Chip::cpxAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpxAbs1() {
    _currentInstruction = &Chip::cpxAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpxAbs2() {
    _currentInstruction = &Chip::cpx0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpy0() {
    cp0(_yIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpyImm0() {
    _currentInstruction = &Chip::cpy0;
    immediate();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpyZp0() {
    _currentInstruction = &Chip::cpyZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpyZp1() {
    _currentInstruction = &Chip::cpy0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpyAbs0() {
    _currentInstruction = &Chip::cpyAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpyAbs1() {
    _currentInstruction = &Chip::cpyAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::cpyAbs2() {
    _currentInstruction = &Chip::cpy0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dec(uint8_t data) {
    // Removing 1 from data using ALU (Add 0xFF without carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _alu.performSum<BDecimalSupported, false>(0xFF, data, false, false);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::inc(uint8_t data) {
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _alu.performSum<BDecimalSupported, false>(0x0, data, false, true);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incdecMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decMemory0() {
    _currentInstruction = &Chip::incdecMemory1;
    
    dec(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decZp0() {
    _currentInstruction = &Chip::decZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decZp1() {
    _currentInstruction = &Chip::decMemory0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decZpX0() {
    _currentInstruction = &Chip::decZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decZpX1() {
    _currentInstruction = &Chip::decZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decZpX2() {
    _currentInstruction = &Chip::decMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decAbs0() {
    _currentInstruction = &Chip::decAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decAbs1() {
    _currentInstruction = &Chip::decAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decAbs2() {
    _currentInstruction = &Chip::decMemory0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decAbsX0() {
    _currentInstruction = &Chip::decAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decAbsX1() {
    _currentInstruction = &Chip::decAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decAbsX2() {
    _currentInstruction = &Chip::decAbsX3;
    absoluteIndexedLoad0(&Chip::decAbsX3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decAbsX3() {
    _currentInstruction = &Chip::decMemory0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incMemory0() {
    _currentInstruction = &Chip::incdecMemory1;
    
    inc(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incZp0() {
    _currentInstruction = &Chip::incZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incZp1() {
    _currentInstruction = &Chip::incMemory0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incZpX0() {
    _currentInstruction = &Chip::incZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incZpX1() {
    _currentInstruction = &Chip::incZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incZpX2() {
    _currentInstruction = &Chip::incMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incAbs0() {
    _currentInstruction = &Chip::incAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incAbs1() {
    _currentInstruction = &Chip::incAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incAbs2() {
    _currentInstruction = &Chip::incMemory0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incAbsX0() {
    _currentInstruction = &Chip::incAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incAbsX1() {
    _currentInstruction = &Chip::incAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incAbsX2() {
    _currentInstruction = &Chip::incAbsX3;
    absoluteIndexedLoad0(&Chip::incAbsX3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incAbsX3() {
    _currentInstruction = &Chip::incMemory0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incdecRegister2(uint8_t &data) {
    // Write result back
    data = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::decRegister1(OpcodeInstruction nextInstruction, uint8_t data) {
    dec(data);
    
    // Fetch opcode during performing ALU
    fetchOpcode(nextInstruction);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dex0() {
    _currentInstruction = &Chip::dex1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dex1() {
    decRegister1(&Chip::dex2, _xIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dex2() {
    incdecRegister2(_xIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dey0() {
    _currentInstruction = &Chip::dey1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dey1() {
    decRegister1(&Chip::dey2, _yIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dey2() {
    incdecRegister2(_yIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::incRegister1(OpcodeInstruction nextInstruction, uint8_t data) {
    inc(data);
    
    // Fetch opcode during performing ALU
    fetchOpcode(nextInstruction);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::inx0() {
    _currentInstruction = &Chip::inx1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::inx1() {
    incRegister1(&Chip::inx2, _xIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::inx2() {
    incdecRegister2(_xIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::iny0() {
    _currentInstruction = &Chip::iny1;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::iny1() {
    incRegister1(&Chip::iny2, _yIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::iny2() {
    incdecRegister2(_yIndex);
}

#endif /* Cpu6502_Internal_Arithmetic_s_hpp */
