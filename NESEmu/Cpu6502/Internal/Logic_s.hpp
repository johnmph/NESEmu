//
//  Logic_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Logic_s_hpp
#define Cpu6502_Internal_Logic_s_hpp


template <class TBus>
void Chip<TBus>::logic1() {
    // Store ALU result in accumulator
    _accumulator = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_adderHold);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus>
void Chip<TBus>::and0() {
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformAnd();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus>
void Chip<TBus>::andImm0() {
    _currentInstruction = &Chip::and0;
    immediate();
}

template <class TBus>
void Chip<TBus>::andZp0() {
    _currentInstruction = &Chip::andZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::andZp1() {
    _currentInstruction = &Chip::and0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::andZpX0() {
    _currentInstruction = &Chip::andZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::andZpX1() {
    _currentInstruction = &Chip::andZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::andZpX2() {
    _currentInstruction = &Chip::and0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::andAbs0() {
    _currentInstruction = &Chip::andAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::andAbs1() {
    _currentInstruction = &Chip::andAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::andAbs2() {
    _currentInstruction = &Chip::and0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::andAbsX0() {
    _currentInstruction = &Chip::andAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::andAbsX1() {
    _currentInstruction = &Chip::andAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::andAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::andAbsX3;
    absoluteIndexedLoad0(&Chip::and0);
}

template <class TBus>
void Chip<TBus>::andAbsX3() {
    _currentInstruction = &Chip::and0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::andAbsY0() {
    _currentInstruction = &Chip::andAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::andAbsY1() {
    _currentInstruction = &Chip::andAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::andAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::andAbsY3;
    absoluteIndexedLoad0(&Chip::and0);
}

template <class TBus>
void Chip<TBus>::andAbsY3() {
    _currentInstruction = &Chip::and0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::andIndX0() {
    _currentInstruction = &Chip::andIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::andIndX1() {
    _currentInstruction = &Chip::andIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::andIndX2() {
    _currentInstruction = &Chip::andIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::andIndX3() {
    _currentInstruction = &Chip::andIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::andIndX4() {
    _currentInstruction = &Chip::and0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::andIndY0() {
    _currentInstruction = &Chip::andIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::andIndY1() {
    _currentInstruction = &Chip::andIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::andIndY2() {
    _currentInstruction = &Chip::andIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::andIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::andIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::and0);
}

template <class TBus>
void Chip<TBus>::andIndY4() {
    _currentInstruction = &Chip::and0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::ora0() {
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformOr();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus>
void Chip<TBus>::oraImm0() {
    _currentInstruction = &Chip::ora0;
    immediate();
}

template <class TBus>
void Chip<TBus>::oraZp0() {
    _currentInstruction = &Chip::oraZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::oraZp1() {
    _currentInstruction = &Chip::ora0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::oraZpX0() {
    _currentInstruction = &Chip::oraZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::oraZpX1() {
    _currentInstruction = &Chip::oraZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::oraZpX2() {
    _currentInstruction = &Chip::ora0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::oraAbs0() {
    _currentInstruction = &Chip::oraAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::oraAbs1() {
    _currentInstruction = &Chip::oraAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::oraAbs2() {
    _currentInstruction = &Chip::ora0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::oraAbsX0() {
    _currentInstruction = &Chip::oraAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::oraAbsX1() {
    _currentInstruction = &Chip::oraAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::oraAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::oraAbsX3;
    absoluteIndexedLoad0(&Chip::ora0);
}

template <class TBus>
void Chip<TBus>::oraAbsX3() {
    _currentInstruction = &Chip::ora0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::oraAbsY0() {
    _currentInstruction = &Chip::oraAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::oraAbsY1() {
    _currentInstruction = &Chip::oraAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::oraAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::oraAbsY3;
    absoluteIndexedLoad0(&Chip::ora0);
}

template <class TBus>
void Chip<TBus>::oraAbsY3() {
    _currentInstruction = &Chip::ora0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::oraIndX0() {
    _currentInstruction = &Chip::oraIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::oraIndX1() {
    _currentInstruction = &Chip::oraIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::oraIndX2() {
    _currentInstruction = &Chip::oraIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::oraIndX3() {
    _currentInstruction = &Chip::oraIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::oraIndX4() {
    _currentInstruction = &Chip::ora0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::oraIndY0() {
    _currentInstruction = &Chip::oraIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::oraIndY1() {
    _currentInstruction = &Chip::oraIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::oraIndY2() {
    _currentInstruction = &Chip::oraIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::oraIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::oraIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::ora0);
}

template <class TBus>
void Chip<TBus>::oraIndY4() {
    _currentInstruction = &Chip::ora0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::eor0() {
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformEor();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus>
void Chip<TBus>::eorImm0() {
    _currentInstruction = &Chip::eor0;
    immediate();
}

template <class TBus>
void Chip<TBus>::eorZp0() {
    _currentInstruction = &Chip::eorZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::eorZp1() {
    _currentInstruction = &Chip::eor0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::eorZpX0() {
    _currentInstruction = &Chip::eorZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::eorZpX1() {
    _currentInstruction = &Chip::eorZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::eorZpX2() {
    _currentInstruction = &Chip::eor0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::eorAbs0() {
    _currentInstruction = &Chip::eorAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::eorAbs1() {
    _currentInstruction = &Chip::eorAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::eorAbs2() {
    _currentInstruction = &Chip::eor0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::eorAbsX0() {
    _currentInstruction = &Chip::eorAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::eorAbsX1() {
    _currentInstruction = &Chip::eorAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::eorAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::eorAbsX3;
    absoluteIndexedLoad0(&Chip::eor0);
}

template <class TBus>
void Chip<TBus>::eorAbsX3() {
    _currentInstruction = &Chip::eor0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::eorAbsY0() {
    _currentInstruction = &Chip::eorAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::eorAbsY1() {
    _currentInstruction = &Chip::eorAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::eorAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::eorAbsY3;
    absoluteIndexedLoad0(&Chip::eor0);
}

template <class TBus>
void Chip<TBus>::eorAbsY3() {
    _currentInstruction = &Chip::eor0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::eorIndX0() {
    _currentInstruction = &Chip::eorIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::eorIndX1() {
    _currentInstruction = &Chip::eorIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::eorIndX2() {
    _currentInstruction = &Chip::eorIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::eorIndX3() {
    _currentInstruction = &Chip::eorIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::eorIndX4() {
    _currentInstruction = &Chip::eor0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::eorIndY0() {
    _currentInstruction = &Chip::eorIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::eorIndY1() {
    _currentInstruction = &Chip::eorIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::eorIndY2() {
    _currentInstruction = &Chip::eorIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::eorIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::eorIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::eor0);
}

template <class TBus>
void Chip<TBus>::eorIndY4() {
    _currentInstruction = &Chip::eor0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::bit0() {
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformAnd();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::bit1);
}

template <class TBus>
void Chip<TBus>::bit1() {
    // Don't save result, it's just to set the flags
    
    // Update status (Zero from result, Overflow and Negative from bInput)
    _flagsHelper.clear<Flag::Zero, Flag::Overflow, Flag::Negative>();
    _flagsHelper.set<Flag::Zero>(_adderHold == 0);
    _flagsHelper.set<Flag::Overflow>(_bInput & 0x40);
    _flagsHelper.set<Flag::Negative>(_bInput & 0x80);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus>
void Chip<TBus>::bitZp0() {
    _currentInstruction = &Chip::bitZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::bitZp1() {
    _currentInstruction = &Chip::bit0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::bitAbs0() {
    _currentInstruction = &Chip::bitAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::bitAbs1() {
    _currentInstruction = &Chip::bitAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::bitAbs2() {
    _currentInstruction = &Chip::bit0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::asl(uint8_t data) {
    // ASL by adding same number to itself with no carry
    _aInput = data;
    _bInput = data;
    aluPerformSum(false, false);
}

template <class TBus>
void Chip<TBus>::aslMemory0() {
    _currentInstruction = &Chip::aslMemory1;
    asl(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::aslMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Chip<TBus>::aslImm0() {
    _currentInstruction = &Chip::aslImm1;
    asl(_accumulator);
    implied();
}

template <class TBus>
void Chip<TBus>::aslImm1() {
    // Write result back
    _accumulator = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus>
void Chip<TBus>::aslZp0() {
    _currentInstruction = &Chip::aslZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::aslZp1() {
    _currentInstruction = &Chip::aslMemory0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::aslZpX0() {
    _currentInstruction = &Chip::aslZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::aslZpX1() {
    _currentInstruction = &Chip::aslZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::aslZpX2() {
    _currentInstruction = &Chip::aslMemory0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::aslAbs0() {
    _currentInstruction = &Chip::aslAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::aslAbs1() {
    _currentInstruction = &Chip::aslAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::aslAbs2() {
    _currentInstruction = &Chip::aslMemory0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::aslAbsX0() {
    _currentInstruction = &Chip::aslAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::aslAbsX1() {
    _currentInstruction = &Chip::aslAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::aslAbsX2() {
    _currentInstruction = &Chip::aslAbsX3;
    absoluteIndexedLoad0(&Chip::aslAbsX3);
}

template <class TBus>
void Chip<TBus>::aslAbsX3() {
    _currentInstruction = &Chip::aslMemory0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::lsr(uint8_t data) {
    // LSR by shift right with no carry
    _aInput = data;
    aluPerformShiftRight(false);
}

template <class TBus>
void Chip<TBus>::lsrMemory0() {
    _currentInstruction = &Chip::lsrMemory1;
    lsr(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::lsrMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Chip<TBus>::lsrImm0() {
    _currentInstruction = &Chip::lsrImm1;
    lsr(_accumulator);
    implied();
}

template <class TBus>
void Chip<TBus>::lsrImm1() {
    // Write result back
    _accumulator = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus>
void Chip<TBus>::lsrZp0() {
    _currentInstruction = &Chip::lsrZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::lsrZp1() {
    _currentInstruction = &Chip::lsrMemory0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::lsrZpX0() {
    _currentInstruction = &Chip::lsrZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::lsrZpX1() {
    _currentInstruction = &Chip::lsrZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::lsrZpX2() {
    _currentInstruction = &Chip::lsrMemory0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::lsrAbs0() {
    _currentInstruction = &Chip::lsrAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::lsrAbs1() {
    _currentInstruction = &Chip::lsrAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::lsrAbs2() {
    _currentInstruction = &Chip::lsrMemory0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::lsrAbsX0() {
    _currentInstruction = &Chip::lsrAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::lsrAbsX1() {
    _currentInstruction = &Chip::lsrAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::lsrAbsX2() {
    _currentInstruction = &Chip::lsrAbsX3;
    absoluteIndexedLoad0(&Chip::lsrAbsX3);
}

template <class TBus>
void Chip<TBus>::lsrAbsX3() {
    _currentInstruction = &Chip::lsrMemory0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::rol(uint8_t data) {
    // ROL by adding same number to itself with carry
    _aInput = data;
    _bInput = data;
    aluPerformSum(false, _flagsHelper.get<Flag::Carry>());
}

template <class TBus>
void Chip<TBus>::rolMemory0() {
    _currentInstruction = &Chip::rolMemory1;
    rol(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::rolMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Chip<TBus>::rolImm0() {
    _currentInstruction = &Chip::rolImm1;
    rol(_accumulator);
    implied();
}

template <class TBus>
void Chip<TBus>::rolImm1() {
    // Write result back
    _accumulator = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
    
    fetchOpcode();
}

template <class TBus>
void Chip<TBus>::rolZp0() {
    _currentInstruction = &Chip::rolZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::rolZp1() {
    _currentInstruction = &Chip::rolMemory0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::rolZpX0() {
    _currentInstruction = &Chip::rolZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::rolZpX1() {
    _currentInstruction = &Chip::rolZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::rolZpX2() {
    _currentInstruction = &Chip::rolMemory0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::rolAbs0() {
    _currentInstruction = &Chip::rolAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::rolAbs1() {
    _currentInstruction = &Chip::rolAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::rolAbs2() {
    _currentInstruction = &Chip::rolMemory0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::rolAbsX0() {
    _currentInstruction = &Chip::rolAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::rolAbsX1() {
    _currentInstruction = &Chip::rolAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::rolAbsX2() {
    _currentInstruction = &Chip::rolAbsX3;
    absoluteIndexedLoad0(&Chip::rolAbsX3);
}

template <class TBus>
void Chip<TBus>::rolAbsX3() {
    _currentInstruction = &Chip::rolMemory0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::ror(uint8_t data) {
    // ROR by shift right with current carry
    _aInput = data;
    aluPerformShiftRight(_flagsHelper.get<Flag::Carry>());
}

template <class TBus>
void Chip<TBus>::rorMemory0() {
    _currentInstruction = &Chip::rorMemory1;
    ror(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::rorMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Chip<TBus>::rorImm0() {
    _currentInstruction = &Chip::rorImm1;
    ror(_accumulator);
    implied();
}

template <class TBus>
void Chip<TBus>::rorImm1() {
    // Write result back
    _accumulator = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus>
void Chip<TBus>::rorZp0() {
    _currentInstruction = &Chip::rorZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::rorZp1() {
    _currentInstruction = &Chip::rorMemory0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::rorZpX0() {
    _currentInstruction = &Chip::rorZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::rorZpX1() {
    _currentInstruction = &Chip::rorZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::rorZpX2() {
    _currentInstruction = &Chip::rorMemory0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::rorAbs0() {
    _currentInstruction = &Chip::rorAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::rorAbs1() {
    _currentInstruction = &Chip::rorAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::rorAbs2() {
    _currentInstruction = &Chip::rorMemory0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::rorAbsX0() {
    _currentInstruction = &Chip::rorAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::rorAbsX1() {
    _currentInstruction = &Chip::rorAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::rorAbsX2() {
    _currentInstruction = &Chip::rorAbsX3;
    absoluteIndexedLoad0(&Chip::rorAbsX3);
}

template <class TBus>
void Chip<TBus>::rorAbsX3() {
    _currentInstruction = &Chip::rorMemory0;
    absoluteIndexedLoad1();
}

#endif /* Cpu6502_Internal_Logic_s_hpp */
