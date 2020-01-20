//
//  Logic_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Logic_s_hpp
#define Cpu6502_Internal_Logic_s_hpp


template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::logic1() {
    // Store ALU result in accumulator
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::and0() {
    _alu.performAnd(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andImm0() {
    _currentInstruction = &Chip::and0;
    immediate();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andZp0() {
    _currentInstruction = &Chip::andZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andZp1() {
    _currentInstruction = &Chip::and0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andZpX0() {
    _currentInstruction = &Chip::andZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andZpX1() {
    _currentInstruction = &Chip::andZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andZpX2() {
    _currentInstruction = &Chip::and0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andAbs0() {
    _currentInstruction = &Chip::andAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andAbs1() {
    _currentInstruction = &Chip::andAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andAbs2() {
    _currentInstruction = &Chip::and0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andAbsX0() {
    _currentInstruction = &Chip::andAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andAbsX1() {
    _currentInstruction = &Chip::andAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::andAbsX3;
    absoluteIndexedLoad0(&Chip::and0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andAbsX3() {
    _currentInstruction = &Chip::and0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andAbsY0() {
    _currentInstruction = &Chip::andAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andAbsY1() {
    _currentInstruction = &Chip::andAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::andAbsY3;
    absoluteIndexedLoad0(&Chip::and0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andAbsY3() {
    _currentInstruction = &Chip::and0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andIndX0() {
    _currentInstruction = &Chip::andIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andIndX1() {
    _currentInstruction = &Chip::andIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andIndX2() {
    _currentInstruction = &Chip::andIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andIndX3() {
    _currentInstruction = &Chip::andIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andIndX4() {
    _currentInstruction = &Chip::and0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andIndY0() {
    _currentInstruction = &Chip::andIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andIndY1() {
    _currentInstruction = &Chip::andIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andIndY2() {
    _currentInstruction = &Chip::andIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::andIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::and0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::andIndY4() {
    _currentInstruction = &Chip::and0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ora0() {
    _alu.performOr(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraImm0() {
    _currentInstruction = &Chip::ora0;
    immediate();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraZp0() {
    _currentInstruction = &Chip::oraZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraZp1() {
    _currentInstruction = &Chip::ora0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraZpX0() {
    _currentInstruction = &Chip::oraZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraZpX1() {
    _currentInstruction = &Chip::oraZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraZpX2() {
    _currentInstruction = &Chip::ora0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraAbs0() {
    _currentInstruction = &Chip::oraAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraAbs1() {
    _currentInstruction = &Chip::oraAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraAbs2() {
    _currentInstruction = &Chip::ora0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraAbsX0() {
    _currentInstruction = &Chip::oraAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraAbsX1() {
    _currentInstruction = &Chip::oraAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::oraAbsX3;
    absoluteIndexedLoad0(&Chip::ora0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraAbsX3() {
    _currentInstruction = &Chip::ora0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraAbsY0() {
    _currentInstruction = &Chip::oraAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraAbsY1() {
    _currentInstruction = &Chip::oraAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::oraAbsY3;
    absoluteIndexedLoad0(&Chip::ora0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraAbsY3() {
    _currentInstruction = &Chip::ora0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraIndX0() {
    _currentInstruction = &Chip::oraIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraIndX1() {
    _currentInstruction = &Chip::oraIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraIndX2() {
    _currentInstruction = &Chip::oraIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraIndX3() {
    _currentInstruction = &Chip::oraIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraIndX4() {
    _currentInstruction = &Chip::ora0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraIndY0() {
    _currentInstruction = &Chip::oraIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraIndY1() {
    _currentInstruction = &Chip::oraIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraIndY2() {
    _currentInstruction = &Chip::oraIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::oraIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::ora0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::oraIndY4() {
    _currentInstruction = &Chip::ora0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eor0() {
    _alu.performEor(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorImm0() {
    _currentInstruction = &Chip::eor0;
    immediate();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorZp0() {
    _currentInstruction = &Chip::eorZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorZp1() {
    _currentInstruction = &Chip::eor0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorZpX0() {
    _currentInstruction = &Chip::eorZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorZpX1() {
    _currentInstruction = &Chip::eorZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorZpX2() {
    _currentInstruction = &Chip::eor0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorAbs0() {
    _currentInstruction = &Chip::eorAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorAbs1() {
    _currentInstruction = &Chip::eorAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorAbs2() {
    _currentInstruction = &Chip::eor0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorAbsX0() {
    _currentInstruction = &Chip::eorAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorAbsX1() {
    _currentInstruction = &Chip::eorAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::eorAbsX3;
    absoluteIndexedLoad0(&Chip::eor0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorAbsX3() {
    _currentInstruction = &Chip::eor0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorAbsY0() {
    _currentInstruction = &Chip::eorAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorAbsY1() {
    _currentInstruction = &Chip::eorAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::eorAbsY3;
    absoluteIndexedLoad0(&Chip::eor0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorAbsY3() {
    _currentInstruction = &Chip::eor0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorIndX0() {
    _currentInstruction = &Chip::eorIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorIndX1() {
    _currentInstruction = &Chip::eorIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorIndX2() {
    _currentInstruction = &Chip::eorIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorIndX3() {
    _currentInstruction = &Chip::eorIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorIndX4() {
    _currentInstruction = &Chip::eor0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorIndY0() {
    _currentInstruction = &Chip::eorIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorIndY1() {
    _currentInstruction = &Chip::eorIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorIndY2() {
    _currentInstruction = &Chip::eorIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::eorIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::eor0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::eorIndY4() {
    _currentInstruction = &Chip::eor0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bit0() {
    _alu.performAnd(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::bit1);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bit1() {
    // Don't save result, it's just to set the flags
    
    // Update status (Zero from result, Overflow and Negative from bInput)
    _flagsHelper.clear<Flag::Zero, Flag::Overflow, Flag::Negative>();
    _flagsHelper.set<Flag::Zero>(_alu.getAdderHold() == 0);
    _flagsHelper.set<Flag::Overflow>(_alu.getBInput() & 0x40);
    _flagsHelper.set<Flag::Negative>(_alu.getBInput() & 0x80);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bitZp0() {
    _currentInstruction = &Chip::bitZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bitZp1() {
    _currentInstruction = &Chip::bit0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bitAbs0() {
    _currentInstruction = &Chip::bitAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bitAbs1() {
    _currentInstruction = &Chip::bitAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::bitAbs2() {
    _currentInstruction = &Chip::bit0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::asl(uint8_t data) {
    // ASL by adding same number to itself with no carry
    _alu.performSum<DecimalSupported, false>(data, data, false, false);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslMemory0() {
    _currentInstruction = &Chip::aslMemory1;
    asl(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslImm0() {
    _currentInstruction = &Chip::aslImm1;
    asl(_accumulator);
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslImm1() {
    // Write result back
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslZp0() {
    _currentInstruction = &Chip::aslZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslZp1() {
    _currentInstruction = &Chip::aslMemory0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslZpX0() {
    _currentInstruction = &Chip::aslZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslZpX1() {
    _currentInstruction = &Chip::aslZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslZpX2() {
    _currentInstruction = &Chip::aslMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslAbs0() {
    _currentInstruction = &Chip::aslAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslAbs1() {
    _currentInstruction = &Chip::aslAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslAbs2() {
    _currentInstruction = &Chip::aslMemory0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslAbsX0() {
    _currentInstruction = &Chip::aslAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslAbsX1() {
    _currentInstruction = &Chip::aslAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslAbsX2() {
    _currentInstruction = &Chip::aslAbsX3;
    absoluteIndexedLoad0(&Chip::aslAbsX3);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::aslAbsX3() {
    _currentInstruction = &Chip::aslMemory0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsr(uint8_t data) {
    // LSR by shift right with no carry
    _alu.performShiftRight(data, false);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrMemory0() {
    _currentInstruction = &Chip::lsrMemory1;
    lsr(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrImm0() {
    _currentInstruction = &Chip::lsrImm1;
    lsr(_accumulator);
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrImm1() {
    // Write result back
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrZp0() {
    _currentInstruction = &Chip::lsrZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrZp1() {
    _currentInstruction = &Chip::lsrMemory0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrZpX0() {
    _currentInstruction = &Chip::lsrZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrZpX1() {
    _currentInstruction = &Chip::lsrZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrZpX2() {
    _currentInstruction = &Chip::lsrMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrAbs0() {
    _currentInstruction = &Chip::lsrAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrAbs1() {
    _currentInstruction = &Chip::lsrAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrAbs2() {
    _currentInstruction = &Chip::lsrMemory0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrAbsX0() {
    _currentInstruction = &Chip::lsrAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrAbsX1() {
    _currentInstruction = &Chip::lsrAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrAbsX2() {
    _currentInstruction = &Chip::lsrAbsX3;
    absoluteIndexedLoad0(&Chip::lsrAbsX3);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lsrAbsX3() {
    _currentInstruction = &Chip::lsrMemory0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rol(uint8_t data) {
    // ROL by adding same number to itself with carry
    _alu.performSum<DecimalSupported, false>(data, data, false, _flagsHelper.get<Flag::Carry>());
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolMemory0() {
    _currentInstruction = &Chip::rolMemory1;
    rol(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolImm0() {
    _currentInstruction = &Chip::rolImm1;
    rol(_accumulator);
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolImm1() {
    // Write result back
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    fetchOpcode();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolZp0() {
    _currentInstruction = &Chip::rolZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolZp1() {
    _currentInstruction = &Chip::rolMemory0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolZpX0() {
    _currentInstruction = &Chip::rolZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolZpX1() {
    _currentInstruction = &Chip::rolZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolZpX2() {
    _currentInstruction = &Chip::rolMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolAbs0() {
    _currentInstruction = &Chip::rolAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolAbs1() {
    _currentInstruction = &Chip::rolAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolAbs2() {
    _currentInstruction = &Chip::rolMemory0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolAbsX0() {
    _currentInstruction = &Chip::rolAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolAbsX1() {
    _currentInstruction = &Chip::rolAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolAbsX2() {
    _currentInstruction = &Chip::rolAbsX3;
    absoluteIndexedLoad0(&Chip::rolAbsX3);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rolAbsX3() {
    _currentInstruction = &Chip::rolMemory0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ror(uint8_t data) {
    // ROR by shift right with current carry
    _alu.performShiftRight(data, _flagsHelper.get<Flag::Carry>());
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorMemory0() {
    _currentInstruction = &Chip::rorMemory1;
    ror(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorImm0() {
    _currentInstruction = &Chip::rorImm1;
    ror(_accumulator);
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorImm1() {
    // Write result back
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorZp0() {
    _currentInstruction = &Chip::rorZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorZp1() {
    _currentInstruction = &Chip::rorMemory0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorZpX0() {
    _currentInstruction = &Chip::rorZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorZpX1() {
    _currentInstruction = &Chip::rorZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorZpX2() {
    _currentInstruction = &Chip::rorMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorAbs0() {
    _currentInstruction = &Chip::rorAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorAbs1() {
    _currentInstruction = &Chip::rorAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorAbs2() {
    _currentInstruction = &Chip::rorMemory0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorAbsX0() {
    _currentInstruction = &Chip::rorAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorAbsX1() {
    _currentInstruction = &Chip::rorAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorAbsX2() {
    _currentInstruction = &Chip::rorAbsX3;
    absoluteIndexedLoad0(&Chip::rorAbsX3);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::rorAbsX3() {
    _currentInstruction = &Chip::rorMemory0;
    absoluteIndexedLoad1();
}

#endif /* Cpu6502_Internal_Logic_s_hpp */
