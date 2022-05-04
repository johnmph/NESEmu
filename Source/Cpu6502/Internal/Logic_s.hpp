//
//  Logic_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Logic_s_hpp
#define Cpu6502_Internal_Logic_s_hpp


template <class TConfiguration>
void Chip<TConfiguration>::and0() {
    // Perform AND
    _alu.performAnd(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::andImm0() {
    _currentInstruction = &Chip::and0;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::andZp0() {
    _currentInstruction = &Chip::andZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::andZp1() {
    _currentInstruction = &Chip::and0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::andZpX0() {
    _currentInstruction = &Chip::andZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::andZpX1() {
    _currentInstruction = &Chip::andZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::andZpX2() {
    _currentInstruction = &Chip::and0;
    zeroPageIndexedRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::andAbs0() {
    _currentInstruction = &Chip::andAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::andAbs1() {
    _currentInstruction = &Chip::andAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::andAbs2() {
    _currentInstruction = &Chip::and0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::andAbsX0() {
    _currentInstruction = &Chip::andAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::andAbsX1() {
    _currentInstruction = &Chip::andAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::andAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::andAbsX3;
    absoluteIndexedRead0(&Chip::and0);
}

template <class TConfiguration>
void Chip<TConfiguration>::andAbsX3() {
    _currentInstruction = &Chip::and0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::andAbsY0() {
    _currentInstruction = &Chip::andAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::andAbsY1() {
    _currentInstruction = &Chip::andAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::andAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::andAbsY3;
    absoluteIndexedRead0(&Chip::and0);
}

template <class TConfiguration>
void Chip<TConfiguration>::andAbsY3() {
    _currentInstruction = &Chip::and0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::andIndX0() {
    _currentInstruction = &Chip::andIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::andIndX1() {
    _currentInstruction = &Chip::andIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::andIndX2() {
    _currentInstruction = &Chip::andIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::andIndX3() {
    _currentInstruction = &Chip::andIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::andIndX4() {
    _currentInstruction = &Chip::and0;
    zeroPagePreIndexedIndirectRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::andIndY0() {
    _currentInstruction = &Chip::andIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::andIndY1() {
    _currentInstruction = &Chip::andIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::andIndY2() {
    _currentInstruction = &Chip::andIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::andIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedRead0 because it can modify it
    _currentInstruction = &Chip::andIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::and0);
}

template <class TConfiguration>
void Chip<TConfiguration>::andIndY4() {
    _currentInstruction = &Chip::and0;
    zeroPageIndirectPostIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ora0() {
    _alu.performOr(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::oraImm0() {
    _currentInstruction = &Chip::ora0;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraZp0() {
    _currentInstruction = &Chip::oraZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraZp1() {
    _currentInstruction = &Chip::ora0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraZpX0() {
    _currentInstruction = &Chip::oraZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraZpX1() {
    _currentInstruction = &Chip::oraZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraZpX2() {
    _currentInstruction = &Chip::ora0;
    zeroPageIndexedRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraAbs0() {
    _currentInstruction = &Chip::oraAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraAbs1() {
    _currentInstruction = &Chip::oraAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraAbs2() {
    _currentInstruction = &Chip::ora0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraAbsX0() {
    _currentInstruction = &Chip::oraAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraAbsX1() {
    _currentInstruction = &Chip::oraAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::oraAbsX3;
    absoluteIndexedRead0(&Chip::ora0);
}

template <class TConfiguration>
void Chip<TConfiguration>::oraAbsX3() {
    _currentInstruction = &Chip::ora0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraAbsY0() {
    _currentInstruction = &Chip::oraAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraAbsY1() {
    _currentInstruction = &Chip::oraAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::oraAbsY3;
    absoluteIndexedRead0(&Chip::ora0);
}

template <class TConfiguration>
void Chip<TConfiguration>::oraAbsY3() {
    _currentInstruction = &Chip::ora0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraIndX0() {
    _currentInstruction = &Chip::oraIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraIndX1() {
    _currentInstruction = &Chip::oraIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraIndX2() {
    _currentInstruction = &Chip::oraIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraIndX3() {
    _currentInstruction = &Chip::oraIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraIndX4() {
    _currentInstruction = &Chip::ora0;
    zeroPagePreIndexedIndirectRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraIndY0() {
    _currentInstruction = &Chip::oraIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraIndY1() {
    _currentInstruction = &Chip::oraIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraIndY2() {
    _currentInstruction = &Chip::oraIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::oraIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedRead0 because it can modify it
    _currentInstruction = &Chip::oraIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::ora0);
}

template <class TConfiguration>
void Chip<TConfiguration>::oraIndY4() {
    _currentInstruction = &Chip::ora0;
    zeroPageIndirectPostIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::eor0() {
    _alu.performEor(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::eorImm0() {
    _currentInstruction = &Chip::eor0;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorZp0() {
    _currentInstruction = &Chip::eorZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorZp1() {
    _currentInstruction = &Chip::eor0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorZpX0() {
    _currentInstruction = &Chip::eorZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorZpX1() {
    _currentInstruction = &Chip::eorZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorZpX2() {
    _currentInstruction = &Chip::eor0;
    zeroPageIndexedRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorAbs0() {
    _currentInstruction = &Chip::eorAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorAbs1() {
    _currentInstruction = &Chip::eorAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorAbs2() {
    _currentInstruction = &Chip::eor0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorAbsX0() {
    _currentInstruction = &Chip::eorAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorAbsX1() {
    _currentInstruction = &Chip::eorAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::eorAbsX3;
    absoluteIndexedRead0(&Chip::eor0);
}

template <class TConfiguration>
void Chip<TConfiguration>::eorAbsX3() {
    _currentInstruction = &Chip::eor0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorAbsY0() {
    _currentInstruction = &Chip::eorAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorAbsY1() {
    _currentInstruction = &Chip::eorAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::eorAbsY3;
    absoluteIndexedRead0(&Chip::eor0);
}

template <class TConfiguration>
void Chip<TConfiguration>::eorAbsY3() {
    _currentInstruction = &Chip::eor0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorIndX0() {
    _currentInstruction = &Chip::eorIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorIndX1() {
    _currentInstruction = &Chip::eorIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorIndX2() {
    _currentInstruction = &Chip::eorIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorIndX3() {
    _currentInstruction = &Chip::eorIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorIndX4() {
    _currentInstruction = &Chip::eor0;
    zeroPagePreIndexedIndirectRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorIndY0() {
    _currentInstruction = &Chip::eorIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorIndY1() {
    _currentInstruction = &Chip::eorIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorIndY2() {
    _currentInstruction = &Chip::eorIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::eorIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedRead0 because it can modify it
    _currentInstruction = &Chip::eorIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::eor0);
}

template <class TConfiguration>
void Chip<TConfiguration>::eorIndY4() {
    _currentInstruction = &Chip::eor0;
    zeroPageIndirectPostIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::bit0() {
    _alu.performAnd(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::bit1);
}

template <class TConfiguration>
void Chip<TConfiguration>::bit1() {
    // Don't save result, it's just to set the flags
    
    // Update status (Zero from result, Overflow and Negative from bInput)
    _flagsHelper.clear<Flag::Zero, Flag::Overflow, Flag::Negative>();
    _flagsHelper.set<Flag::Zero>(_alu.getAdderHold() == 0);
    _flagsHelper.set<Flag::Overflow>(_alu.getBInput() & 0x40);
    _flagsHelper.set<Flag::Negative>(_alu.getBInput() & 0x80);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TConfiguration>
void Chip<TConfiguration>::bitZp0() {
    _currentInstruction = &Chip::bitZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::bitZp1() {
    _currentInstruction = &Chip::bit0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::bitAbs0() {
    _currentInstruction = &Chip::bitAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::bitAbs1() {
    _currentInstruction = &Chip::bitAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::bitAbs2() {
    _currentInstruction = &Chip::bit0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::asl(uint8_t data) {
    // ASL by adding same number to itself with no carry
    _alu.performSum<DecimalSupported, false>(data, data, false, false);
}

template <class TConfiguration>
void Chip<TConfiguration>::aslMemory0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::finishAluForMemory<Flag::Carry, Flag::Zero, Flag::Negative>, _inputDataLatch);
    
    // Arithmetic shift left
    asl(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::aslImm0() {
    _currentInstruction = &Chip::aslImm1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslImm1() {
    asl(_accumulator);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Carry, Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::aslZp0() {
    _currentInstruction = &Chip::aslZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslZp1() {
    _currentInstruction = &Chip::aslMemory0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslZpX0() {
    _currentInstruction = &Chip::aslZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslZpX1() {
    _currentInstruction = &Chip::aslZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslZpX2() {
    _currentInstruction = &Chip::aslMemory0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslAbs0() {
    _currentInstruction = &Chip::aslAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslAbs1() {
    _currentInstruction = &Chip::aslAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslAbs2() {
    _currentInstruction = &Chip::aslMemory0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslAbsX0() {
    _currentInstruction = &Chip::aslAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslAbsX1() {
    _currentInstruction = &Chip::aslAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslAbsX2() {
    _currentInstruction = &Chip::aslAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::aslAbsX3() {
    _currentInstruction = &Chip::aslMemory0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsr(uint8_t data) {
    // LSR by shift right with no carry
    _alu.performShiftRight(data, false);
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrMemory0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::finishAluForMemory<Flag::Carry, Flag::Zero, Flag::Negative>, _inputDataLatch);
    
    // Logical shift right
    lsr(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrImm0() {
    _currentInstruction = &Chip::lsrImm1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrImm1() {
    lsr(_accumulator);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Carry, Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrZp0() {
    _currentInstruction = &Chip::lsrZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrZp1() {
    _currentInstruction = &Chip::lsrMemory0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrZpX0() {
    _currentInstruction = &Chip::lsrZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrZpX1() {
    _currentInstruction = &Chip::lsrZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrZpX2() {
    _currentInstruction = &Chip::lsrMemory0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrAbs0() {
    _currentInstruction = &Chip::lsrAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrAbs1() {
    _currentInstruction = &Chip::lsrAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrAbs2() {
    _currentInstruction = &Chip::lsrMemory0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrAbsX0() {
    _currentInstruction = &Chip::lsrAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrAbsX1() {
    _currentInstruction = &Chip::lsrAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrAbsX2() {
    _currentInstruction = &Chip::lsrAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lsrAbsX3() {
    _currentInstruction = &Chip::lsrMemory0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rol(uint8_t data) {
    // ROL by adding same number to itself with carry
    _alu.performSum<DecimalSupported, false>(data, data, false, _flagsHelper.get<Flag::Carry>());
}

template <class TConfiguration>
void Chip<TConfiguration>::rolMemory0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::finishAluForMemory<Flag::Carry, Flag::Zero, Flag::Negative>, _inputDataLatch);
    
    // Rotate left
    rol(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::rolImm0() {
    _currentInstruction = &Chip::rolImm1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolImm1() {
    rol(_accumulator);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Carry, Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::rolZp0() {
    _currentInstruction = &Chip::rolZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolZp1() {
    _currentInstruction = &Chip::rolMemory0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolZpX0() {
    _currentInstruction = &Chip::rolZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolZpX1() {
    _currentInstruction = &Chip::rolZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolZpX2() {
    _currentInstruction = &Chip::rolMemory0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolAbs0() {
    _currentInstruction = &Chip::rolAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolAbs1() {
    _currentInstruction = &Chip::rolAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolAbs2() {
    _currentInstruction = &Chip::rolMemory0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolAbsX0() {
    _currentInstruction = &Chip::rolAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolAbsX1() {
    _currentInstruction = &Chip::rolAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolAbsX2() {
    _currentInstruction = &Chip::rolAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rolAbsX3() {
    _currentInstruction = &Chip::rolMemory0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ror(uint8_t data) {
    // ROR by shift right with current carry
    _alu.performShiftRight(data, _flagsHelper.get<Flag::Carry>());
}

template <class TConfiguration>
void Chip<TConfiguration>::rorMemory0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::finishAluForMemory<Flag::Carry, Flag::Zero, Flag::Negative>, _inputDataLatch);
    
    // Rotate right
    ror(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::rorImm0() {
    _currentInstruction = &Chip::rorImm1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorImm1() {
    ror(_accumulator);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Carry, Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::rorZp0() {
    _currentInstruction = &Chip::rorZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorZp1() {
    _currentInstruction = &Chip::rorMemory0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorZpX0() {
    _currentInstruction = &Chip::rorZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorZpX1() {
    _currentInstruction = &Chip::rorZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorZpX2() {
    _currentInstruction = &Chip::rorMemory0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorAbs0() {
    _currentInstruction = &Chip::rorAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorAbs1() {
    _currentInstruction = &Chip::rorAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorAbs2() {
    _currentInstruction = &Chip::rorMemory0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorAbsX0() {
    _currentInstruction = &Chip::rorAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorAbsX1() {
    _currentInstruction = &Chip::rorAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorAbsX2() {
    _currentInstruction = &Chip::rorAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rorAbsX3() {
    _currentInstruction = &Chip::rorMemory0;
    absoluteIndexedRMWRead1();
}

#endif /* Cpu6502_Internal_Logic_s_hpp */
