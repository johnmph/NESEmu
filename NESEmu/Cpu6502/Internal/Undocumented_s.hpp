//
//  Undocumented_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Undocumented_s_hpp
#define Cpu6502_Internal_Undocumented_s_hpp


// ASO

template <class TConfiguration>
void Chip<TConfiguration>::aso0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::aso1, _inputDataLatch);
    
    // Shift left
    asl(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::aso1() {
    finishAluForMemory<Flag::Carry, Flag::Zero, Flag::Negative>(&Chip::aso2);
}

template <class TConfiguration>
void Chip<TConfiguration>::aso2() {
    _alu.performOr(_accumulator, _alu.getAdderHold());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::asoZp0() {
    _currentInstruction = &Chip::asoZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoZp1() {
    _currentInstruction = &Chip::aso0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoZpX0() {
    _currentInstruction = &Chip::asoZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoZpX1() {
    _currentInstruction = &Chip::asoZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoZpX2() {
    _currentInstruction = &Chip::aso0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoAbs0() {
    _currentInstruction = &Chip::asoAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoAbs1() {
    _currentInstruction = &Chip::asoAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoAbs2() {
    _currentInstruction = &Chip::aso0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoAbsX0() {
    _currentInstruction = &Chip::asoAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoAbsX1() {
    _currentInstruction = &Chip::asoAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoAbsX2() {
    _currentInstruction = &Chip::asoAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoAbsX3() {
    _currentInstruction = &Chip::aso0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoAbsY0() {
    _currentInstruction = &Chip::asoAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoAbsY1() {
    _currentInstruction = &Chip::asoAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoAbsY2() {
    _currentInstruction = &Chip::asoAbsY3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoAbsY3() {
    _currentInstruction = &Chip::aso0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoIndX0() {
    _currentInstruction = &Chip::asoIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoIndX1() {
    _currentInstruction = &Chip::asoIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoIndX2() {
    _currentInstruction = &Chip::asoIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoIndX3() {
    _currentInstruction = &Chip::asoIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoIndX4() {
    _currentInstruction = &Chip::aso0;
    zeroPagePreIndexedIndirectRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoIndY0() {
    _currentInstruction = &Chip::asoIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoIndY1() {
    _currentInstruction = &Chip::asoIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoIndY2() {
    _currentInstruction = &Chip::asoIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoIndY3() {
    _currentInstruction = &Chip::asoIndY4;
    zeroPageIndirectPostIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::asoIndY4() {
    _currentInstruction = &Chip::aso0;
    zeroPageIndirectPostIndexedRMWRead1();
}

// RLA

template <class TConfiguration>
void Chip<TConfiguration>::rla0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::rla1, _inputDataLatch);
    
    // Rotate left
    rol(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::rla1() {
    finishAluForMemory<Flag::Carry, Flag::Zero, Flag::Negative>(&Chip::rla2);
}

template <class TConfiguration>
void Chip<TConfiguration>::rla2() {
    _alu.performAnd(_accumulator, _alu.getAdderHold());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaZp0() {
    _currentInstruction = &Chip::rlaZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaZp1() {
    _currentInstruction = &Chip::rla0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaZpX0() {
    _currentInstruction = &Chip::rlaZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaZpX1() {
    _currentInstruction = &Chip::rlaZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaZpX2() {
    _currentInstruction = &Chip::rla0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaAbs0() {
    _currentInstruction = &Chip::rlaAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaAbs1() {
    _currentInstruction = &Chip::rlaAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaAbs2() {
    _currentInstruction = &Chip::rla0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaAbsX0() {
    _currentInstruction = &Chip::rlaAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaAbsX1() {
    _currentInstruction = &Chip::rlaAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaAbsX2() {
    _currentInstruction = &Chip::rlaAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaAbsX3() {
    _currentInstruction = &Chip::rla0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaAbsY0() {
    _currentInstruction = &Chip::rlaAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaAbsY1() {
    _currentInstruction = &Chip::rlaAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaAbsY2() {
    _currentInstruction = &Chip::rlaAbsY3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaAbsY3() {
    _currentInstruction = &Chip::rla0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaIndX0() {
    _currentInstruction = &Chip::rlaIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaIndX1() {
    _currentInstruction = &Chip::rlaIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaIndX2() {
    _currentInstruction = &Chip::rlaIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaIndX3() {
    _currentInstruction = &Chip::rlaIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaIndX4() {
    _currentInstruction = &Chip::rla0;
    zeroPagePreIndexedIndirectRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaIndY0() {
    _currentInstruction = &Chip::rlaIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaIndY1() {
    _currentInstruction = &Chip::rlaIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaIndY2() {
    _currentInstruction = &Chip::rlaIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaIndY3() {
    _currentInstruction = &Chip::rlaIndY4;
    zeroPageIndirectPostIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rlaIndY4() {
    _currentInstruction = &Chip::rla0;
    zeroPageIndirectPostIndexedRMWRead1();
}

// LSE

template <class TConfiguration>
void Chip<TConfiguration>::lse0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::lse1, _inputDataLatch);
    
    // Shift right
    lsr(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::lse1() {
    finishAluForMemory<Flag::Carry, Flag::Zero, Flag::Negative>(&Chip::lse2);
}

template <class TConfiguration>
void Chip<TConfiguration>::lse2() {
    _alu.performEor(_accumulator, _alu.getAdderHold());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::lseZp0() {
    _currentInstruction = &Chip::lseZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseZp1() {
    _currentInstruction = &Chip::lse0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseZpX0() {
    _currentInstruction = &Chip::lseZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseZpX1() {
    _currentInstruction = &Chip::lseZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseZpX2() {
    _currentInstruction = &Chip::lse0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseAbs0() {
    _currentInstruction = &Chip::lseAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseAbs1() {
    _currentInstruction = &Chip::lseAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseAbs2() {
    _currentInstruction = &Chip::lse0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseAbsX0() {
    _currentInstruction = &Chip::lseAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseAbsX1() {
    _currentInstruction = &Chip::lseAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseAbsX2() {
    _currentInstruction = &Chip::lseAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseAbsX3() {
    _currentInstruction = &Chip::lse0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseAbsY0() {
    _currentInstruction = &Chip::lseAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseAbsY1() {
    _currentInstruction = &Chip::lseAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseAbsY2() {
    _currentInstruction = &Chip::lseAbsY3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseAbsY3() {
    _currentInstruction = &Chip::lse0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseIndX0() {
    _currentInstruction = &Chip::lseIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseIndX1() {
    _currentInstruction = &Chip::lseIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseIndX2() {
    _currentInstruction = &Chip::lseIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseIndX3() {
    _currentInstruction = &Chip::lseIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseIndX4() {
    _currentInstruction = &Chip::lse0;
    zeroPagePreIndexedIndirectRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseIndY0() {
    _currentInstruction = &Chip::lseIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseIndY1() {
    _currentInstruction = &Chip::lseIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseIndY2() {
    _currentInstruction = &Chip::lseIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseIndY3() {
    _currentInstruction = &Chip::lseIndY4;
    zeroPageIndirectPostIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::lseIndY4() {
    _currentInstruction = &Chip::lse0;
    zeroPageIndirectPostIndexedRMWRead1();
}

// RRA

template <class TConfiguration>
void Chip<TConfiguration>::rra0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::rra1, _inputDataLatch);
    
    // Rotate right
    ror(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::rra1() {
    finishAluForMemory<Flag::Carry, Flag::Zero, Flag::Negative>(&Chip::rra2);
}

template <class TConfiguration>
void Chip<TConfiguration>::rra2() {
    // Adding data to accumulator with possible carry
    _alu.performSum<DecimalSupported, false>(_accumulator, _alu.getAdderHold(), _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Carry, Flag::Zero, Flag::Overflow, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::rraZp0() {
    _currentInstruction = &Chip::rraZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraZp1() {
    _currentInstruction = &Chip::rra0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraZpX0() {
    _currentInstruction = &Chip::rraZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraZpX1() {
    _currentInstruction = &Chip::rraZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraZpX2() {
    _currentInstruction = &Chip::rra0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraAbs0() {
    _currentInstruction = &Chip::rraAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraAbs1() {
    _currentInstruction = &Chip::rraAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraAbs2() {
    _currentInstruction = &Chip::rra0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraAbsX0() {
    _currentInstruction = &Chip::rraAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraAbsX1() {
    _currentInstruction = &Chip::rraAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraAbsX2() {
    _currentInstruction = &Chip::rraAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraAbsX3() {
    _currentInstruction = &Chip::rra0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraAbsY0() {
    _currentInstruction = &Chip::rraAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraAbsY1() {
    _currentInstruction = &Chip::rraAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraAbsY2() {
    _currentInstruction = &Chip::rraAbsY3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraAbsY3() {
    _currentInstruction = &Chip::rra0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraIndX0() {
    _currentInstruction = &Chip::rraIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraIndX1() {
    _currentInstruction = &Chip::rraIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraIndX2() {
    _currentInstruction = &Chip::rraIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraIndX3() {
    _currentInstruction = &Chip::rraIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraIndX4() {
    _currentInstruction = &Chip::rra0;
    zeroPagePreIndexedIndirectRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraIndY0() {
    _currentInstruction = &Chip::rraIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraIndY1() {
    _currentInstruction = &Chip::rraIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraIndY2() {
    _currentInstruction = &Chip::rraIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraIndY3() {
    _currentInstruction = &Chip::rraIndY4;
    zeroPageIndirectPostIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rraIndY4() {
    _currentInstruction = &Chip::rra0;
    zeroPageIndirectPostIndexedRMWRead1();
}

// AXS

template <class TConfiguration>
void Chip<TConfiguration>::axsZp0() {
    _currentInstruction = &Chip::axsZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageWrite(_accumulator & _xIndex);
    //staZp1();     // TODO: par apres voir pour ca (avec le dataOutput &=)
    //stxZp1();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsZpY0() {
    _currentInstruction = &Chip::axsZpY1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsZpY1() {
    _currentInstruction = &Chip::axsZpY2;
    zeroPageIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsZpY2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedWrite(_accumulator & _xIndex);
    // Need to be first because there is no zero page Y addressing mode for STA, so STX will correct the address
    //staZpX2();       // TODO: par apres voir pour ca (avec le dataOutput &=)
    //stxZpY2();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsAbs0() {
    _currentInstruction = &Chip::axsAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsAbs1() {
    _currentInstruction = &Chip::axsAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteWrite(_accumulator & _xIndex);
    //staAbs2();         // TODO: par apres voir pour ca (avec le dataOutput &=)
    //stxAbs2();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsIndX0() {
    _currentInstruction = &Chip::axsIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsIndX1() {
    _currentInstruction = &Chip::axsIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsIndX2() {
    _currentInstruction = &Chip::axsIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsIndX3() {
    _currentInstruction = &Chip::axsIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::axsIndX4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPagePreIndexedIndirectWrite(_accumulator & _xIndex);
    // Need to be first because there is no indirect X addressing mode for STX, so STA will correct the address
    //stxAbs2();             // TODO: par apres voir pour ca (avec le dataOutput &=)
    //staIndX4();
}

// LAX

template <class TConfiguration>
void Chip<TConfiguration>::lax0() {
    lda0();
    
    _xIndex = _accumulator;
}

template <class TConfiguration>
void Chip<TConfiguration>::laxZp0() {
    _currentInstruction = &Chip::laxZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxZp1() {
    _currentInstruction = &Chip::lax0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxZpY0() {
    _currentInstruction = &Chip::laxZpY1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxZpY1() {
    _currentInstruction = &Chip::laxZpY2;
    zeroPageIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxZpY2() {
    _currentInstruction = &Chip::lax0;
    zeroPageIndexedRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxAbs0() {
    _currentInstruction = &Chip::laxAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxAbs1() {
    _currentInstruction = &Chip::laxAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxAbs2() {
    _currentInstruction = &Chip::lax0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxAbsY0() {
    _currentInstruction = &Chip::laxAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxAbsY1() {
    _currentInstruction = &Chip::laxAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxAbsY2() {
    _currentInstruction = &Chip::laxAbsY3;
    absoluteIndexedRead0(&Chip::lax0);
}

template <class TConfiguration>
void Chip<TConfiguration>::laxAbsY3() {
    _currentInstruction = &Chip::lax0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxIndX0() {
    _currentInstruction = &Chip::laxIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxIndX1() {
    _currentInstruction = &Chip::laxIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxIndX2() {
    _currentInstruction = &Chip::laxIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxIndX3() {
    _currentInstruction = &Chip::laxIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxIndX4() {
    _currentInstruction = &Chip::lax0;
    zeroPagePreIndexedIndirectRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxIndY0() {
    _currentInstruction = &Chip::laxIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxIndY1() {
    _currentInstruction = &Chip::laxIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxIndY2() {
    _currentInstruction = &Chip::laxIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::laxIndY3() {
    _currentInstruction = &Chip::laxIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::lax0);
}

template <class TConfiguration>
void Chip<TConfiguration>::laxIndY4() {
    _currentInstruction = &Chip::lax0;
    zeroPageIndirectPostIndexedRead1();
}

// DCM

template <class TConfiguration>
void Chip<TConfiguration>::dcm0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::dcm1, _inputDataLatch);
    
    // Decrement
    dec(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::dcm1() {
    finishAluForMemory<Flag::Zero, Flag::Negative>(&Chip::dcm2);
}

template <class TConfiguration>
void Chip<TConfiguration>::dcm2() {
    // Substracting data fetched to accumulator
    _alu.performSum<DecimalSupported, true>(_accumulator, _alu.getAdderHold(), false, true);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAlu<Flag::Carry, Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmZp0() {
    _currentInstruction = &Chip::dcmZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmZp1() {
    _currentInstruction = &Chip::dcm0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmZpX0() {
    _currentInstruction = &Chip::dcmZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmZpX1() {
    _currentInstruction = &Chip::dcmZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmZpX2() {
    _currentInstruction = &Chip::dcm0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmAbs0() {
    _currentInstruction = &Chip::dcmAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmAbs1() {
    _currentInstruction = &Chip::dcmAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmAbs2() {
    _currentInstruction = &Chip::dcm0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmAbsX0() {
    _currentInstruction = &Chip::dcmAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmAbsX1() {
    _currentInstruction = &Chip::dcmAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmAbsX2() {
    _currentInstruction = &Chip::dcmAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmAbsX3() {
    _currentInstruction = &Chip::dcm0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmAbsY0() {
    _currentInstruction = &Chip::dcmAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmAbsY1() {
    _currentInstruction = &Chip::dcmAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmAbsY2() {
    _currentInstruction = &Chip::dcmAbsY3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmAbsY3() {
    _currentInstruction = &Chip::dcm0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmIndX0() {
    _currentInstruction = &Chip::dcmIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmIndX1() {
    _currentInstruction = &Chip::dcmIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmIndX2() {
    _currentInstruction = &Chip::dcmIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmIndX3() {
    _currentInstruction = &Chip::dcmIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmIndX4() {
    _currentInstruction = &Chip::dcm0;
    zeroPagePreIndexedIndirectRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmIndY0() {
    _currentInstruction = &Chip::dcmIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmIndY1() {
    _currentInstruction = &Chip::dcmIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmIndY2() {
    _currentInstruction = &Chip::dcmIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmIndY3() {
    _currentInstruction = &Chip::dcmIndY4;
    zeroPageIndirectPostIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::dcmIndY4() {
    _currentInstruction = &Chip::dcm0;
    zeroPageIndirectPostIndexedRMWRead1();
}

// INS

template <class TConfiguration>
void Chip<TConfiguration>::ins0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::ins1, _inputDataLatch);
    
    // Increment
    inc(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::ins1() {
    finishAluForMemory<Flag::Zero, Flag::Negative>(&Chip::ins2);
}

template <class TConfiguration>
void Chip<TConfiguration>::ins2() {
    // Substracting data to accumulator with possible carry by inverting bInput
    _alu.performSum<DecimalSupported, true>(_accumulator, _alu.getAdderHold(), _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Carry, Flag::Zero, Flag::Overflow, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::insZp0() {
    _currentInstruction = &Chip::insZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::insZp1() {
    _currentInstruction = &Chip::ins0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::insZpX0() {
    _currentInstruction = &Chip::insZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::insZpX1() {
    _currentInstruction = &Chip::insZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::insZpX2() {
    _currentInstruction = &Chip::ins0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::insAbs0() {
    _currentInstruction = &Chip::insAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::insAbs1() {
    _currentInstruction = &Chip::insAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::insAbs2() {
    _currentInstruction = &Chip::ins0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::insAbsX0() {
    _currentInstruction = &Chip::insAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::insAbsX1() {
    _currentInstruction = &Chip::insAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::insAbsX2() {
    _currentInstruction = &Chip::insAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::insAbsX3() {
    _currentInstruction = &Chip::ins0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::insAbsY0() {
    _currentInstruction = &Chip::insAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::insAbsY1() {
    _currentInstruction = &Chip::insAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::insAbsY2() {
    _currentInstruction = &Chip::insAbsY3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::insAbsY3() {
    _currentInstruction = &Chip::ins0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::insIndX0() {
    _currentInstruction = &Chip::insIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::insIndX1() {
    _currentInstruction = &Chip::insIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::insIndX2() {
    _currentInstruction = &Chip::insIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::insIndX3() {
    _currentInstruction = &Chip::insIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::insIndX4() {
    _currentInstruction = &Chip::ins0;
    zeroPagePreIndexedIndirectRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::insIndY0() {
    _currentInstruction = &Chip::insIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::insIndY1() {
    _currentInstruction = &Chip::insIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::insIndY2() {
    _currentInstruction = &Chip::insIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::insIndY3() {
    _currentInstruction = &Chip::insIndY4;
    zeroPageIndirectPostIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::insIndY4() {
    _currentInstruction = &Chip::ins0;
    zeroPageIndirectPostIndexedRMWRead1();
}

// ALR

template <class TConfiguration>
void Chip<TConfiguration>::alrImm0() {
    _currentInstruction = &Chip::alrImm1;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::alrImm1() {
    // The AND is not performed by the ALU but by bus conflict which cause a low level to win (it is like an AND operation)
    // See : https://wiki.nesdev.com/w/index.php/Bus_conflict
    // And : https://wiki.nesdev.com/w/index.php/Programming_with_unofficial_opcodes
    lsr(_accumulator & _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Carry, Flag::Zero, Flag::Negative>);
}

// NOP

template <class TConfiguration>
void Chip<TConfiguration>::nopImm0() {
    _currentInstruction = &Chip::fetchOpcode;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::nopZp0() {
    _currentInstruction = &Chip::nopZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::nopZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::nopZpX0() {
    _currentInstruction = &Chip::nopZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::nopZpX1() {
    _currentInstruction = &Chip::nopZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::nopZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::nopAbs0() {
    _currentInstruction = &Chip::nopAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::nopAbs1() {
    _currentInstruction = &Chip::nopAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::nopAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::nopAbsX0() {
    _currentInstruction = &Chip::nopAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::nopAbsX1() {
    _currentInstruction = &Chip::nopAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::nopAbsX2() {
    _currentInstruction = &Chip::nopAbsX3;
    absoluteIndexedRead0(&Chip::fetchOpcode);
}

template <class TConfiguration>
void Chip<TConfiguration>::nopAbsX3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::kil() {
    // Does nothing and doesn't set the next instruction to cause an infinite loop on this
}

#endif /* Cpu6502_Internal_Undocumented_s_hpp */
