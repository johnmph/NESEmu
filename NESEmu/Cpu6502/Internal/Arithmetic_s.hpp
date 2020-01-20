//
//  Arithmetic_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Arithmetic_s_hpp
#define Cpu6502_Internal_Arithmetic_s_hpp


template <class TBus>
void Chip<TBus>::arithmetic1() {
    // Store ALU result in accumulator
    _accumulator = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Overflow, Flag::Negative>(_adderHold);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus>
void Chip<TBus>::adc0() {
    // Adding data to accumulator with possible carry
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformSum(_flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::arithmetic1);
}

template <class TBus>
void Chip<TBus>::adcImm0() {
    _currentInstruction = &Chip::adc0;
    immediate();
}

template <class TBus>
void Chip<TBus>::adcZp0() {
    _currentInstruction = &Chip::adcZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::adcZp1() {
    _currentInstruction = &Chip::adc0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::adcZpX0() {
    _currentInstruction = &Chip::adcZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::adcZpX1() {
    _currentInstruction = &Chip::adcZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::adcZpX2() {
    _currentInstruction = &Chip::adc0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::adcAbs0() {
    _currentInstruction = &Chip::adcAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::adcAbs1() {
    _currentInstruction = &Chip::adcAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::adcAbs2() {
    _currentInstruction = &Chip::adc0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::adcAbsX0() {
    _currentInstruction = &Chip::adcAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::adcAbsX1() {
    _currentInstruction = &Chip::adcAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::adcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::adcAbsX3;
    absoluteIndexedLoad0(&Chip::adc0);
}

template <class TBus>
void Chip<TBus>::adcAbsX3() {
    _currentInstruction = &Chip::adc0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::adcAbsY0() {
    _currentInstruction = &Chip::adcAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::adcAbsY1() {
    _currentInstruction = &Chip::adcAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::adcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::adcAbsY3;
    absoluteIndexedLoad0(&Chip::adc0);
}

template <class TBus>
void Chip<TBus>::adcAbsY3() {
    _currentInstruction = &Chip::adc0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::adcIndX0() {
    _currentInstruction = &Chip::adcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::adcIndX1() {
    _currentInstruction = &Chip::adcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::adcIndX2() {
    _currentInstruction = &Chip::adcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::adcIndX3() {
    _currentInstruction = &Chip::adcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::adcIndX4() {
    _currentInstruction = &Chip::adc0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::adcIndY0() {
    _currentInstruction = &Chip::adcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::adcIndY1() {
    _currentInstruction = &Chip::adcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::adcIndY2() {
    _currentInstruction = &Chip::adcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::adcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::adcIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::adc0);
}

template <class TBus>
void Chip<TBus>::adcIndY4() {
    _currentInstruction = &Chip::adc0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::sbc0() {
    // Substracting data to accumulator with possible carry by inverting bInput
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluInvertBInput();
    aluPerformSum(_flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::arithmetic1);
}

template <class TBus>
void Chip<TBus>::sbcImm0() {
    _currentInstruction = &Chip::sbc0;
    immediate();
}

template <class TBus>
void Chip<TBus>::sbcZp0() {
    _currentInstruction = &Chip::sbcZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::sbcZp1() {
    _currentInstruction = &Chip::sbc0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::sbcZpX0() {
    _currentInstruction = &Chip::sbcZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::sbcZpX1() {
    _currentInstruction = &Chip::sbcZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::sbcZpX2() {
    _currentInstruction = &Chip::sbc0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::sbcAbs0() {
    _currentInstruction = &Chip::sbcAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::sbcAbs1() {
    _currentInstruction = &Chip::sbcAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::sbcAbs2() {
    _currentInstruction = &Chip::sbc0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::sbcAbsX0() {
    _currentInstruction = &Chip::sbcAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::sbcAbsX1() {
    _currentInstruction = &Chip::sbcAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::sbcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::sbcAbsX3;
    absoluteIndexedLoad0(&Chip::sbc0);
}

template <class TBus>
void Chip<TBus>::sbcAbsX3() {
    _currentInstruction = &Chip::sbc0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::sbcAbsY0() {
    _currentInstruction = &Chip::sbcAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::sbcAbsY1() {
    _currentInstruction = &Chip::sbcAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::sbcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::sbcAbsY3;
    absoluteIndexedLoad0(&Chip::sbc0);
}

template <class TBus>
void Chip<TBus>::sbcAbsY3() {
    _currentInstruction = &Chip::sbc0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::sbcIndX0() {
    _currentInstruction = &Chip::sbcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::sbcIndX1() {
    _currentInstruction = &Chip::sbcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::sbcIndX2() {
    _currentInstruction = &Chip::sbcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::sbcIndX3() {
    _currentInstruction = &Chip::sbcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::sbcIndX4() {
    _currentInstruction = &Chip::sbc0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::sbcIndY0() {
    _currentInstruction = &Chip::sbcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::sbcIndY1() {
    _currentInstruction = &Chip::sbcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::sbcIndY2() {
    _currentInstruction = &Chip::sbcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::sbcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::sbcIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::sbc0);
}

template <class TBus>
void Chip<TBus>::sbcIndY4() {
    _currentInstruction = &Chip::sbc0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::cp0(uint8_t data) {
    // Substracting data fetched to data to compare them
    _aInput = data;
    _bInput = _inputDataLatch;
    aluInvertBInput();
    aluPerformSum(false, true);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::cp1);
}

template <class TBus>
void Chip<TBus>::cp1() {
    // Don't save result, it's just to set the flags
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus>
void Chip<TBus>::cmp0() {
    cp0(_accumulator);
}

template <class TBus>
void Chip<TBus>::cmpImm0() {
    _currentInstruction = &Chip::cmp0;
    immediate();
}

template <class TBus>
void Chip<TBus>::cmpZp0() {
    _currentInstruction = &Chip::cmpZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::cmpZp1() {
    _currentInstruction = &Chip::cmp0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::cmpZpX0() {
    _currentInstruction = &Chip::cmpZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::cmpZpX1() {
    _currentInstruction = &Chip::cmpZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::cmpZpX2() {
    _currentInstruction = &Chip::cmp0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::cmpAbs0() {
    _currentInstruction = &Chip::cmpAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::cmpAbs1() {
    _currentInstruction = &Chip::cmpAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::cmpAbs2() {
    _currentInstruction = &Chip::cmp0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::cmpAbsX0() {
    _currentInstruction = &Chip::cmpAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::cmpAbsX1() {
    _currentInstruction = &Chip::cmpAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::cmpAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::cmpAbsX3;
    absoluteIndexedLoad0(&Chip::cmp0);
}

template <class TBus>
void Chip<TBus>::cmpAbsX3() {
    _currentInstruction = &Chip::cmp0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::cmpAbsY0() {
    _currentInstruction = &Chip::cmpAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::cmpAbsY1() {
    _currentInstruction = &Chip::cmpAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::cmpAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::cmpAbsY3;
    absoluteIndexedLoad0(&Chip::cmp0);
}

template <class TBus>
void Chip<TBus>::cmpAbsY3() {
    _currentInstruction = &Chip::cmp0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::cmpIndX0() {
    _currentInstruction = &Chip::cmpIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::cmpIndX1() {
    _currentInstruction = &Chip::cmpIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::cmpIndX2() {
    _currentInstruction = &Chip::cmpIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::cmpIndX3() {
    _currentInstruction = &Chip::cmpIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::cmpIndX4() {
    _currentInstruction = &Chip::cmp0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::cmpIndY0() {
    _currentInstruction = &Chip::cmpIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::cmpIndY1() {
    _currentInstruction = &Chip::cmpIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::cmpIndY2() {
    _currentInstruction = &Chip::cmpIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::cmpIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::cmpIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::cmp0);
}

template <class TBus>
void Chip<TBus>::cmpIndY4() {
    _currentInstruction = &Chip::cmp0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::cpx0() {
    cp0(_xIndex);
}

template <class TBus>
void Chip<TBus>::cpxImm0() {
    _currentInstruction = &Chip::cpx0;
    immediate();
}

template <class TBus>
void Chip<TBus>::cpxZp0() {
    _currentInstruction = &Chip::cpxZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::cpxZp1() {
    _currentInstruction = &Chip::cpx0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::cpxAbs0() {
    _currentInstruction = &Chip::cpxAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::cpxAbs1() {
    _currentInstruction = &Chip::cpxAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::cpxAbs2() {
    _currentInstruction = &Chip::cpx0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::cpy0() {
    cp0(_yIndex);
}

template <class TBus>
void Chip<TBus>::cpyImm0() {
    _currentInstruction = &Chip::cpy0;
    immediate();
}

template <class TBus>
void Chip<TBus>::cpyZp0() {
    _currentInstruction = &Chip::cpyZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::cpyZp1() {
    _currentInstruction = &Chip::cpy0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::cpyAbs0() {
    _currentInstruction = &Chip::cpyAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::cpyAbs1() {
    _currentInstruction = &Chip::cpyAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::cpyAbs2() {
    _currentInstruction = &Chip::cpy0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::dec(uint8_t data) {
    // Removing 1 from data using ALU (Add 0xFF without carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _aInput = 0xFF;
    _bInput = data;
    aluPerformSum(false, false);
}

template <class TBus>
void Chip<TBus>::inc(uint8_t data) {
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _aInput = 0x0;
    _bInput = data;
    aluPerformSum(false, true);
}

template <class TBus>
void Chip<TBus>::incdecMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Chip<TBus>::decMemory0() {
    _currentInstruction = &Chip::incdecMemory1;
    
    dec(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::decZp0() {
    _currentInstruction = &Chip::decZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::decZp1() {
    _currentInstruction = &Chip::decMemory0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::decZpX0() {
    _currentInstruction = &Chip::decZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::decZpX1() {
    _currentInstruction = &Chip::decZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::decZpX2() {
    _currentInstruction = &Chip::decMemory0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::decAbs0() {
    _currentInstruction = &Chip::decAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::decAbs1() {
    _currentInstruction = &Chip::decAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::decAbs2() {
    _currentInstruction = &Chip::decMemory0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::decAbsX0() {
    _currentInstruction = &Chip::decAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::decAbsX1() {
    _currentInstruction = &Chip::decAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::decAbsX2() {
    _currentInstruction = &Chip::decAbsX3;
    absoluteIndexedLoad0(&Chip::decAbsX3);
}

template <class TBus>
void Chip<TBus>::decAbsX3() {
    _currentInstruction = &Chip::decMemory0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::incMemory0() {
    _currentInstruction = &Chip::incdecMemory1;
    
    inc(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::incZp0() {
    _currentInstruction = &Chip::incZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::incZp1() {
    _currentInstruction = &Chip::incMemory0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::incZpX0() {
    _currentInstruction = &Chip::incZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::incZpX1() {
    _currentInstruction = &Chip::incZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::incZpX2() {
    _currentInstruction = &Chip::incMemory0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::incAbs0() {
    _currentInstruction = &Chip::incAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::incAbs1() {
    _currentInstruction = &Chip::incAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::incAbs2() {
    _currentInstruction = &Chip::incMemory0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::incAbsX0() {
    _currentInstruction = &Chip::incAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::incAbsX1() {
    _currentInstruction = &Chip::incAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::incAbsX2() {
    _currentInstruction = &Chip::incAbsX3;
    absoluteIndexedLoad0(&Chip::incAbsX3);
}

template <class TBus>
void Chip<TBus>::incAbsX3() {
    _currentInstruction = &Chip::incMemory0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::incdecRegister2(uint8_t &data) {
    // Write result back
    data = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_adderHold);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus>
void Chip<TBus>::decRegister1(OpcodeInstruction nextInstruction, uint8_t data) {
    dec(data);
    
    // Fetch opcode during performing ALU
    fetchOpcode(nextInstruction);
}

template <class TBus>
void Chip<TBus>::dex0() {
    _currentInstruction = &Chip::dex1;
    implied();
}

template <class TBus>
void Chip<TBus>::dex1() {
    decRegister1(&Chip::dex2, _xIndex);
}

template <class TBus>
void Chip<TBus>::dex2() {
    incdecRegister2(_xIndex);
}

template <class TBus>
void Chip<TBus>::dey0() {
    _currentInstruction = &Chip::dey1;
    implied();
}

template <class TBus>
void Chip<TBus>::dey1() {
    decRegister1(&Chip::dey2, _yIndex);
}

template <class TBus>
void Chip<TBus>::dey2() {
    incdecRegister2(_yIndex);
}

template <class TBus>
void Chip<TBus>::incRegister1(OpcodeInstruction nextInstruction, uint8_t data) {
    inc(data);
    
    // Fetch opcode during performing ALU
    fetchOpcode(nextInstruction);
}

template <class TBus>
void Chip<TBus>::inx0() {
    _currentInstruction = &Chip::inx1;
    implied();
}

template <class TBus>
void Chip<TBus>::inx1() {
    incRegister1(&Chip::inx2, _xIndex);
}

template <class TBus>
void Chip<TBus>::inx2() {
    incdecRegister2(_xIndex);
}

template <class TBus>
void Chip<TBus>::iny0() {
    _currentInstruction = &Chip::iny1;
    implied();
}

template <class TBus>
void Chip<TBus>::iny1() {
    incRegister1(&Chip::iny2, _yIndex);
}

template <class TBus>
void Chip<TBus>::iny2() {
    incdecRegister2(_yIndex);
}

#endif /* Cpu6502_Internal_Arithmetic_s_hpp */
