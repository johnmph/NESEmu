//
//  Cpu6502Arithmetic_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Arithmetic_s_hpp
#define Cpu6502Arithmetic_s_hpp


template <class TBus>
void Cpu6502<TBus>::arithmetic1() {
    // Store ALU result in accumulator
    _accumulator = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Overflow, Flag::Negative>(_adderHold);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus>
void Cpu6502<TBus>::adc0() {
    // Adding data to accumulator with possible carry
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformSum(_flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::arithmetic1);
}

template <class TBus>
void Cpu6502<TBus>::adcImm0() {
    _currentInstruction = &Cpu6502::adc0;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::adcZp0() {
    _currentInstruction = &Cpu6502::adcZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::adcZp1() {
    _currentInstruction = &Cpu6502::adc0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::adcZpX0() {
    _currentInstruction = &Cpu6502::adcZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::adcZpX1() {
    _currentInstruction = &Cpu6502::adcZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::adcZpX2() {
    _currentInstruction = &Cpu6502::adc0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::adcAbs0() {
    _currentInstruction = &Cpu6502::adcAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::adcAbs1() {
    _currentInstruction = &Cpu6502::adcAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::adcAbs2() {
    _currentInstruction = &Cpu6502::adc0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::adcAbsX0() {
    _currentInstruction = &Cpu6502::adcAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::adcAbsX1() {
    _currentInstruction = &Cpu6502::adcAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::adcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::adcAbsX3;
    absoluteIndexedLoad0(&Cpu6502::adc0);
}

template <class TBus>
void Cpu6502<TBus>::adcAbsX3() {
    _currentInstruction = &Cpu6502::adc0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::adcAbsY0() {
    _currentInstruction = &Cpu6502::adcAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::adcAbsY1() {
    _currentInstruction = &Cpu6502::adcAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::adcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::adcAbsY3;
    absoluteIndexedLoad0(&Cpu6502::adc0);
}

template <class TBus>
void Cpu6502<TBus>::adcAbsY3() {
    _currentInstruction = &Cpu6502::adc0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::adcIndX0() {
    _currentInstruction = &Cpu6502::adcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::adcIndX1() {
    _currentInstruction = &Cpu6502::adcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::adcIndX2() {
    _currentInstruction = &Cpu6502::adcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::adcIndX3() {
    _currentInstruction = &Cpu6502::adcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::adcIndX4() {
    _currentInstruction = &Cpu6502::adc0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::adcIndY0() {
    _currentInstruction = &Cpu6502::adcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::adcIndY1() {
    _currentInstruction = &Cpu6502::adcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::adcIndY2() {
    _currentInstruction = &Cpu6502::adcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::adcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Cpu6502::adcIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::adc0);
}

template <class TBus>
void Cpu6502<TBus>::adcIndY4() {
    _currentInstruction = &Cpu6502::adc0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::sbc0() {
    // Substracting data to accumulator with possible carry by inverting bInput
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluInvertBInput();
    aluPerformSum(_flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::arithmetic1);
}

template <class TBus>
void Cpu6502<TBus>::sbcImm0() {
    _currentInstruction = &Cpu6502::sbc0;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::sbcZp0() {
    _currentInstruction = &Cpu6502::sbcZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::sbcZp1() {
    _currentInstruction = &Cpu6502::sbc0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::sbcZpX0() {
    _currentInstruction = &Cpu6502::sbcZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::sbcZpX1() {
    _currentInstruction = &Cpu6502::sbcZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::sbcZpX2() {
    _currentInstruction = &Cpu6502::sbc0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::sbcAbs0() {
    _currentInstruction = &Cpu6502::sbcAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::sbcAbs1() {
    _currentInstruction = &Cpu6502::sbcAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::sbcAbs2() {
    _currentInstruction = &Cpu6502::sbc0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::sbcAbsX0() {
    _currentInstruction = &Cpu6502::sbcAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::sbcAbsX1() {
    _currentInstruction = &Cpu6502::sbcAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::sbcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::sbcAbsX3;
    absoluteIndexedLoad0(&Cpu6502::sbc0);
}

template <class TBus>
void Cpu6502<TBus>::sbcAbsX3() {
    _currentInstruction = &Cpu6502::sbc0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::sbcAbsY0() {
    _currentInstruction = &Cpu6502::sbcAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::sbcAbsY1() {
    _currentInstruction = &Cpu6502::sbcAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::sbcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::sbcAbsY3;
    absoluteIndexedLoad0(&Cpu6502::sbc0);
}

template <class TBus>
void Cpu6502<TBus>::sbcAbsY3() {
    _currentInstruction = &Cpu6502::sbc0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::sbcIndX0() {
    _currentInstruction = &Cpu6502::sbcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::sbcIndX1() {
    _currentInstruction = &Cpu6502::sbcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::sbcIndX2() {
    _currentInstruction = &Cpu6502::sbcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::sbcIndX3() {
    _currentInstruction = &Cpu6502::sbcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::sbcIndX4() {
    _currentInstruction = &Cpu6502::sbc0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::sbcIndY0() {
    _currentInstruction = &Cpu6502::sbcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::sbcIndY1() {
    _currentInstruction = &Cpu6502::sbcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::sbcIndY2() {
    _currentInstruction = &Cpu6502::sbcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::sbcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Cpu6502::sbcIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::sbc0);
}

template <class TBus>
void Cpu6502<TBus>::sbcIndY4() {
    _currentInstruction = &Cpu6502::sbc0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::cp0(uint8_t data) {
    // Substracting data fetched to data to compare them
    _aInput = data;
    _bInput = _inputDataLatch;
    aluInvertBInput();
    aluPerformSum(false, true);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::cp1);
}

template <class TBus>
void Cpu6502<TBus>::cp1() {
    // Don't save result, it's just to set the flags
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus>
void Cpu6502<TBus>::cmp0() {
    cp0(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::cmpImm0() {
    _currentInstruction = &Cpu6502::cmp0;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::cmpZp0() {
    _currentInstruction = &Cpu6502::cmpZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::cmpZp1() {
    _currentInstruction = &Cpu6502::cmp0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::cmpZpX0() {
    _currentInstruction = &Cpu6502::cmpZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::cmpZpX1() {
    _currentInstruction = &Cpu6502::cmpZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::cmpZpX2() {
    _currentInstruction = &Cpu6502::cmp0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::cmpAbs0() {
    _currentInstruction = &Cpu6502::cmpAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::cmpAbs1() {
    _currentInstruction = &Cpu6502::cmpAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::cmpAbs2() {
    _currentInstruction = &Cpu6502::cmp0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::cmpAbsX0() {
    _currentInstruction = &Cpu6502::cmpAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::cmpAbsX1() {
    _currentInstruction = &Cpu6502::cmpAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::cmpAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::cmpAbsX3;
    absoluteIndexedLoad0(&Cpu6502::cmp0);
}

template <class TBus>
void Cpu6502<TBus>::cmpAbsX3() {
    _currentInstruction = &Cpu6502::cmp0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::cmpAbsY0() {
    _currentInstruction = &Cpu6502::cmpAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::cmpAbsY1() {
    _currentInstruction = &Cpu6502::cmpAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::cmpAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::cmpAbsY3;
    absoluteIndexedLoad0(&Cpu6502::cmp0);
}

template <class TBus>
void Cpu6502<TBus>::cmpAbsY3() {
    _currentInstruction = &Cpu6502::cmp0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::cmpIndX0() {
    _currentInstruction = &Cpu6502::cmpIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::cmpIndX1() {
    _currentInstruction = &Cpu6502::cmpIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::cmpIndX2() {
    _currentInstruction = &Cpu6502::cmpIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::cmpIndX3() {
    _currentInstruction = &Cpu6502::cmpIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::cmpIndX4() {
    _currentInstruction = &Cpu6502::cmp0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::cmpIndY0() {
    _currentInstruction = &Cpu6502::cmpIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::cmpIndY1() {
    _currentInstruction = &Cpu6502::cmpIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::cmpIndY2() {
    _currentInstruction = &Cpu6502::cmpIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::cmpIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Cpu6502::cmpIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::cmp0);
}

template <class TBus>
void Cpu6502<TBus>::cmpIndY4() {
    _currentInstruction = &Cpu6502::cmp0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::cpx0() {
    cp0(_xIndex);
}

template <class TBus>
void Cpu6502<TBus>::cpxImm0() {
    _currentInstruction = &Cpu6502::cpx0;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::cpxZp0() {
    _currentInstruction = &Cpu6502::cpxZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::cpxZp1() {
    _currentInstruction = &Cpu6502::cpx0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::cpxAbs0() {
    _currentInstruction = &Cpu6502::cpxAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::cpxAbs1() {
    _currentInstruction = &Cpu6502::cpxAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::cpxAbs2() {
    _currentInstruction = &Cpu6502::cpx0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::cpy0() {
    cp0(_yIndex);
}

template <class TBus>
void Cpu6502<TBus>::cpyImm0() {
    _currentInstruction = &Cpu6502::cpy0;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::cpyZp0() {
    _currentInstruction = &Cpu6502::cpyZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::cpyZp1() {
    _currentInstruction = &Cpu6502::cpy0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::cpyAbs0() {
    _currentInstruction = &Cpu6502::cpyAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::cpyAbs1() {
    _currentInstruction = &Cpu6502::cpyAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::cpyAbs2() {
    _currentInstruction = &Cpu6502::cpy0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::dec0() {
    _currentInstruction = &Cpu6502::dec1;
    
    // Removing 1 from data using ALU (Add 0xFF without carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _aInput = 0xFF;
    _bInput = _inputDataLatch;
    aluPerformSum(false, false);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::dec1() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Cpu6502<TBus>::decZp0() {
    _currentInstruction = &Cpu6502::decZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::decZp1() {
    _currentInstruction = &Cpu6502::dec0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::decZpX0() {
    _currentInstruction = &Cpu6502::decZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::decZpX1() {
    _currentInstruction = &Cpu6502::decZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::decZpX2() {
    _currentInstruction = &Cpu6502::dec0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::decAbs0() {
    _currentInstruction = &Cpu6502::decAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::decAbs1() {
    _currentInstruction = &Cpu6502::decAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::decAbs2() {
    _currentInstruction = &Cpu6502::dec0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::decAbsX0() {
    _currentInstruction = &Cpu6502::decAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::decAbsX1() {
    _currentInstruction = &Cpu6502::decAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::decAbsX2() {
    _currentInstruction = &Cpu6502::decAbsX3;
    absoluteIndexedLoad0(&Cpu6502::decAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::decAbsX3() {
    _currentInstruction = &Cpu6502::dec0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::inc0() {
    _currentInstruction = &Cpu6502::inc1;
    
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _aInput = 0x0;
    _bInput = _inputDataLatch;
    aluPerformSum(false, true);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::inc1() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Cpu6502<TBus>::incZp0() {
    _currentInstruction = &Cpu6502::incZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::incZp1() {
    _currentInstruction = &Cpu6502::inc0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::incZpX0() {
    _currentInstruction = &Cpu6502::incZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::incZpX1() {
    _currentInstruction = &Cpu6502::incZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::incZpX2() {
    _currentInstruction = &Cpu6502::inc0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::incAbs0() {
    _currentInstruction = &Cpu6502::incAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::incAbs1() {
    _currentInstruction = &Cpu6502::incAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::incAbs2() {
    _currentInstruction = &Cpu6502::inc0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::incAbsX0() {
    _currentInstruction = &Cpu6502::incAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::incAbsX1() {
    _currentInstruction = &Cpu6502::incAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::incAbsX2() {
    _currentInstruction = &Cpu6502::incAbsX3;
    absoluteIndexedLoad0(&Cpu6502::incAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::incAbsX3() {
    _currentInstruction = &Cpu6502::inc0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::decrement0(OpcodeInstruction nextInstruction, uint8_t data) {
    _currentInstruction = nextInstruction;
    
    // Removing 1 from data using ALU (Add 0xFF without carry set like true 6502, data is loaded on A register because it comes from SB signal)
    _aInput = data;
    _bInput = 0xFF;
    aluPerformSum(false, false);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, data);//TODO: voir si mettre ca
    
    implied();  // TODO: si on met la ligne du dessus, il faut retirer celle ci
}

template <class TBus>
void Cpu6502<TBus>::decrement1(uint8_t &data) {
    // Write result back
    data = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_adderHold);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::dex0() {
    decrement0(&Cpu6502::dex1, _xIndex);
}

template <class TBus>
void Cpu6502<TBus>::dex1() {
    decrement1(_xIndex);
}

template <class TBus>
void Cpu6502<TBus>::dey0() {
    decrement0(&Cpu6502::dey1, _yIndex);
}

template <class TBus>
void Cpu6502<TBus>::dey1() {
    decrement1(_yIndex);
}

template <class TBus>
void Cpu6502<TBus>::increment0(OpcodeInstruction nextInstruction, uint8_t data) {
    _currentInstruction = nextInstruction;
    
    // Adding 1 with data using ALU (Add 0 with carry set like true 6502, data is loaded on B register, it comes from SB/DB signal because only A register can be set to 0)
    _aInput = 0x0;
    _bInput = data;
    aluPerformSum(false, true);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, data);//TODO: voir si mettre ca
    
    implied();  // TODO: si on met la ligne du dessus, il faut retirer celle ci
}

template <class TBus>
void Cpu6502<TBus>::increment1(uint8_t &data) {
    // Write result back
    data = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_adderHold);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::inx0() {
    increment0(&Cpu6502::inx1, _xIndex);
}

template <class TBus>
void Cpu6502<TBus>::inx1() {
    increment1(_xIndex);
}

template <class TBus>
void Cpu6502<TBus>::iny0() {
    increment0(&Cpu6502::iny1, _yIndex);
}

template <class TBus>
void Cpu6502<TBus>::iny1() {
    increment1(_yIndex);
}


#endif /* Cpu6502Arithmetic_s_hpp */
