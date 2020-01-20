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

template <class TBus>
void Chip<TBus>::aso0() {
    _currentInstruction = &Chip::aso1;
    asl(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::aso1() {
    _currentInstruction = &Chip::aso2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Chip<TBus>::aso2() {
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluPerformOr();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus>
void Chip<TBus>::asoZp0() {
    _currentInstruction = &Chip::asoZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::asoZp1() {
    _currentInstruction = &Chip::aso0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::asoZpX0() {
    _currentInstruction = &Chip::asoZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::asoZpX1() {
    _currentInstruction = &Chip::asoZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::asoZpX2() {
    _currentInstruction = &Chip::aso0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::asoAbs0() {
    _currentInstruction = &Chip::asoAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::asoAbs1() {
    _currentInstruction = &Chip::asoAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::asoAbs2() {
    _currentInstruction = &Chip::aso0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::asoAbsX0() {
    _currentInstruction = &Chip::asoAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::asoAbsX1() {
    _currentInstruction = &Chip::asoAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::asoAbsX2() {
    _currentInstruction = &Chip::asoAbsX3;
    absoluteIndexedLoad0(&Chip::asoAbsX3);
}

template <class TBus>
void Chip<TBus>::asoAbsX3() {
    _currentInstruction = &Chip::aso0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::asoAbsY0() {
    _currentInstruction = &Chip::asoAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::asoAbsY1() {
    _currentInstruction = &Chip::asoAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::asoAbsY2() {
    _currentInstruction = &Chip::asoAbsY3;
    absoluteIndexedLoad0(&Chip::asoAbsY3);
}

template <class TBus>
void Chip<TBus>::asoAbsY3() {
    _currentInstruction = &Chip::aso0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::asoIndX0() {
    _currentInstruction = &Chip::asoIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::asoIndX1() {
    _currentInstruction = &Chip::asoIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::asoIndX2() {
    _currentInstruction = &Chip::asoIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::asoIndX3() {
    _currentInstruction = &Chip::asoIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::asoIndX4() {
    _currentInstruction = &Chip::aso0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::asoIndY0() {
    _currentInstruction = &Chip::asoIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::asoIndY1() {
    _currentInstruction = &Chip::asoIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::asoIndY2() {
    _currentInstruction = &Chip::asoIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::asoIndY3() {
    _currentInstruction = &Chip::asoIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::asoIndY4);
}

template <class TBus>
void Chip<TBus>::asoIndY4() {
    _currentInstruction = &Chip::aso0;
    zeroPageIndirectPostIndexedLoad1();
}

// RLA

template <class TBus>
void Chip<TBus>::rla0() {
    _currentInstruction = &Chip::rla1;
    rol(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::rla1() {
    _currentInstruction = &Chip::rla2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Chip<TBus>::rla2() {
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluPerformAnd();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus>
void Chip<TBus>::rlaZp0() {
    _currentInstruction = &Chip::rlaZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::rlaZp1() {
    _currentInstruction = &Chip::rla0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::rlaZpX0() {
    _currentInstruction = &Chip::rlaZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::rlaZpX1() {
    _currentInstruction = &Chip::rlaZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::rlaZpX2() {
    _currentInstruction = &Chip::rla0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::rlaAbs0() {
    _currentInstruction = &Chip::rlaAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::rlaAbs1() {
    _currentInstruction = &Chip::rlaAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::rlaAbs2() {
    _currentInstruction = &Chip::rla0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::rlaAbsX0() {
    _currentInstruction = &Chip::rlaAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::rlaAbsX1() {
    _currentInstruction = &Chip::rlaAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::rlaAbsX2() {
    _currentInstruction = &Chip::rlaAbsX3;
    absoluteIndexedLoad0(&Chip::rlaAbsX3);
}

template <class TBus>
void Chip<TBus>::rlaAbsX3() {
    _currentInstruction = &Chip::rla0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::rlaAbsY0() {
    _currentInstruction = &Chip::rlaAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::rlaAbsY1() {
    _currentInstruction = &Chip::rlaAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::rlaAbsY2() {
    _currentInstruction = &Chip::rlaAbsY3;
    absoluteIndexedLoad0(&Chip::rlaAbsY3);
}

template <class TBus>
void Chip<TBus>::rlaAbsY3() {
    _currentInstruction = &Chip::rla0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::rlaIndX0() {
    _currentInstruction = &Chip::rlaIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::rlaIndX1() {
    _currentInstruction = &Chip::rlaIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::rlaIndX2() {
    _currentInstruction = &Chip::rlaIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::rlaIndX3() {
    _currentInstruction = &Chip::rlaIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::rlaIndX4() {
    _currentInstruction = &Chip::rla0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::rlaIndY0() {
    _currentInstruction = &Chip::rlaIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::rlaIndY1() {
    _currentInstruction = &Chip::rlaIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::rlaIndY2() {
    _currentInstruction = &Chip::rlaIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::rlaIndY3() {
    _currentInstruction = &Chip::rlaIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::rlaIndY4);
}

template <class TBus>
void Chip<TBus>::rlaIndY4() {
    _currentInstruction = &Chip::rla0;
    zeroPageIndirectPostIndexedLoad1();
}

// LSE

template <class TBus>
void Chip<TBus>::lse0() {
    _currentInstruction = &Chip::lse1;
    lsr(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::lse1() {
    _currentInstruction = &Chip::lse2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Chip<TBus>::lse2() {
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluPerformEor();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus>
void Chip<TBus>::lseZp0() {
    _currentInstruction = &Chip::lseZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::lseZp1() {
    _currentInstruction = &Chip::lse0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::lseZpX0() {
    _currentInstruction = &Chip::lseZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::lseZpX1() {
    _currentInstruction = &Chip::lseZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::lseZpX2() {
    _currentInstruction = &Chip::lse0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::lseAbs0() {
    _currentInstruction = &Chip::lseAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::lseAbs1() {
    _currentInstruction = &Chip::lseAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::lseAbs2() {
    _currentInstruction = &Chip::lse0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::lseAbsX0() {
    _currentInstruction = &Chip::lseAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::lseAbsX1() {
    _currentInstruction = &Chip::lseAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::lseAbsX2() {
    _currentInstruction = &Chip::lseAbsX3;
    absoluteIndexedLoad0(&Chip::lseAbsX3);
}

template <class TBus>
void Chip<TBus>::lseAbsX3() {
    _currentInstruction = &Chip::lse0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::lseAbsY0() {
    _currentInstruction = &Chip::lseAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::lseAbsY1() {
    _currentInstruction = &Chip::lseAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::lseAbsY2() {
    _currentInstruction = &Chip::lseAbsY3;
    absoluteIndexedLoad0(&Chip::lseAbsY3);
}

template <class TBus>
void Chip<TBus>::lseAbsY3() {
    _currentInstruction = &Chip::lse0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::lseIndX0() {
    _currentInstruction = &Chip::lseIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::lseIndX1() {
    _currentInstruction = &Chip::lseIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::lseIndX2() {
    _currentInstruction = &Chip::lseIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::lseIndX3() {
    _currentInstruction = &Chip::lseIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::lseIndX4() {
    _currentInstruction = &Chip::lse0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::lseIndY0() {
    _currentInstruction = &Chip::lseIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::lseIndY1() {
    _currentInstruction = &Chip::lseIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::lseIndY2() {
    _currentInstruction = &Chip::lseIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::lseIndY3() {
    _currentInstruction = &Chip::lseIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::lseIndY4);
}

template <class TBus>
void Chip<TBus>::lseIndY4() {
    _currentInstruction = &Chip::lse0;
    zeroPageIndirectPostIndexedLoad1();
}

// RRA

template <class TBus>
void Chip<TBus>::rra0() {
    _currentInstruction = &Chip::rra1;
    ror(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::rra1() {
    _currentInstruction = &Chip::rra2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Chip<TBus>::rra2() {
    // Adding data to accumulator with possible carry
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluPerformSum(_flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::arithmetic1);
}

template <class TBus>
void Chip<TBus>::rraZp0() {
    _currentInstruction = &Chip::rraZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::rraZp1() {
    _currentInstruction = &Chip::rra0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::rraZpX0() {
    _currentInstruction = &Chip::rraZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::rraZpX1() {
    _currentInstruction = &Chip::rraZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::rraZpX2() {
    _currentInstruction = &Chip::rra0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::rraAbs0() {
    _currentInstruction = &Chip::rraAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::rraAbs1() {
    _currentInstruction = &Chip::rraAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::rraAbs2() {
    _currentInstruction = &Chip::rra0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::rraAbsX0() {
    _currentInstruction = &Chip::rraAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::rraAbsX1() {
    _currentInstruction = &Chip::rraAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::rraAbsX2() {
    _currentInstruction = &Chip::rraAbsX3;
    absoluteIndexedLoad0(&Chip::rraAbsX3);
}

template <class TBus>
void Chip<TBus>::rraAbsX3() {
    _currentInstruction = &Chip::rra0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::rraAbsY0() {
    _currentInstruction = &Chip::rraAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::rraAbsY1() {
    _currentInstruction = &Chip::rraAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::rraAbsY2() {
    _currentInstruction = &Chip::rraAbsY3;
    absoluteIndexedLoad0(&Chip::rraAbsY3);
}

template <class TBus>
void Chip<TBus>::rraAbsY3() {
    _currentInstruction = &Chip::rra0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::rraIndX0() {
    _currentInstruction = &Chip::rraIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::rraIndX1() {
    _currentInstruction = &Chip::rraIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::rraIndX2() {
    _currentInstruction = &Chip::rraIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::rraIndX3() {
    _currentInstruction = &Chip::rraIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::rraIndX4() {
    _currentInstruction = &Chip::rra0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::rraIndY0() {
    _currentInstruction = &Chip::rraIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::rraIndY1() {
    _currentInstruction = &Chip::rraIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::rraIndY2() {
    _currentInstruction = &Chip::rraIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::rraIndY3() {
    _currentInstruction = &Chip::rraIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::rraIndY4);
}

template <class TBus>
void Chip<TBus>::rraIndY4() {
    _currentInstruction = &Chip::rra0;
    zeroPageIndirectPostIndexedLoad1();
}

// AXS

template <class TBus>
void Chip<TBus>::axsZp0() {
    _currentInstruction = &Chip::axsZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::axsZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageStore(_accumulator & _xIndex);
    //staZp1();     // TODO: par apres voir pour ca (avec le dataOutput &=)
    //stxZp1();
}

template <class TBus>
void Chip<TBus>::axsZpY0() {
    _currentInstruction = &Chip::axsZpY1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::axsZpY1() {
    _currentInstruction = &Chip::axsZpY2;
    zeroPageIndexedY1();
}

template <class TBus>
void Chip<TBus>::axsZpY2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedStore(_accumulator & _xIndex);
    // Need to be first because there is no zero page Y addressing mode for STA, so STX will correct the address
    //staZpX2();       // TODO: par apres voir pour ca (avec le dataOutput &=)
    //stxZpY2();
}

template <class TBus>
void Chip<TBus>::axsAbs0() {
    _currentInstruction = &Chip::axsAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::axsAbs1() {
    _currentInstruction = &Chip::axsAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::axsAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteStore(_accumulator & _xIndex);
    //staAbs2();         // TODO: par apres voir pour ca (avec le dataOutput &=)
    //stxAbs2();
}

template <class TBus>
void Chip<TBus>::axsIndX0() {
    _currentInstruction = &Chip::axsIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::axsIndX1() {
    _currentInstruction = &Chip::axsIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::axsIndX2() {
    _currentInstruction = &Chip::axsIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::axsIndX3() {
    _currentInstruction = &Chip::axsIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::axsIndX4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPagePreIndexedIndirectStore(_accumulator & _xIndex);
    // Need to be first because there is no indirect X addressing mode for STX, so STA will correct the address
    //stxAbs2();             // TODO: par apres voir pour ca (avec le dataOutput &=)
    //staIndX4();
}

// LAX

template <class TBus>
void Chip<TBus>::lax0() {
    lda0();
    
    _xIndex = _accumulator;
}

template <class TBus>
void Chip<TBus>::laxZp0() {
    _currentInstruction = &Chip::laxZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::laxZp1() {
    _currentInstruction = &Chip::lax0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::laxZpY0() {
    _currentInstruction = &Chip::laxZpY1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::laxZpY1() {
    _currentInstruction = &Chip::laxZpY2;
    zeroPageIndexedY1();
}

template <class TBus>
void Chip<TBus>::laxZpY2() {
    _currentInstruction = &Chip::lax0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::laxAbs0() {
    _currentInstruction = &Chip::laxAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::laxAbs1() {
    _currentInstruction = &Chip::laxAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::laxAbs2() {
    _currentInstruction = &Chip::lax0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::laxAbsY0() {
    _currentInstruction = &Chip::laxAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::laxAbsY1() {
    _currentInstruction = &Chip::laxAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::laxAbsY2() {
    _currentInstruction = &Chip::laxAbsY3;
    absoluteIndexedLoad0(&Chip::lax0);
}

template <class TBus>
void Chip<TBus>::laxAbsY3() {
    _currentInstruction = &Chip::lax0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::laxIndX0() {
    _currentInstruction = &Chip::laxIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::laxIndX1() {
    _currentInstruction = &Chip::laxIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::laxIndX2() {
    _currentInstruction = &Chip::laxIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::laxIndX3() {
    _currentInstruction = &Chip::laxIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::laxIndX4() {
    _currentInstruction = &Chip::lax0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::laxIndY0() {
    _currentInstruction = &Chip::laxIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::laxIndY1() {
    _currentInstruction = &Chip::laxIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::laxIndY2() {
    _currentInstruction = &Chip::laxIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::laxIndY3() {
    _currentInstruction = &Chip::laxIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::lax0);
}

template <class TBus>
void Chip<TBus>::laxIndY4() {
    _currentInstruction = &Chip::lax0;
    zeroPageIndirectPostIndexedLoad1();
}

// DCM

template <class TBus>
void Chip<TBus>::dcm0() {
    _currentInstruction = &Chip::dcm1;
    
    dec(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::dcm1() {
    _currentInstruction = &Chip::dcm2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
}

template <class TBus>
void Chip<TBus>::dcm2() {
    // Substracting data fetched to accumulator
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluInvertBInput();
    aluPerformSum(false, true);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::dcm3);
}

template <class TBus>
void Chip<TBus>::dcm3() {
    // Don't save result, it's just to set the flags
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus>
void Chip<TBus>::dcmZp0() {
    _currentInstruction = &Chip::dcmZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::dcmZp1() {
    _currentInstruction = &Chip::dcm0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::dcmZpX0() {
    _currentInstruction = &Chip::dcmZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::dcmZpX1() {
    _currentInstruction = &Chip::dcmZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::dcmZpX2() {
    _currentInstruction = &Chip::dcm0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::dcmAbs0() {
    _currentInstruction = &Chip::dcmAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::dcmAbs1() {
    _currentInstruction = &Chip::dcmAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::dcmAbs2() {
    _currentInstruction = &Chip::dcm0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::dcmAbsX0() {
    _currentInstruction = &Chip::dcmAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::dcmAbsX1() {
    _currentInstruction = &Chip::dcmAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::dcmAbsX2() {
    _currentInstruction = &Chip::dcmAbsX3;
    absoluteIndexedLoad0(&Chip::dcmAbsX3);
}

template <class TBus>
void Chip<TBus>::dcmAbsX3() {
    _currentInstruction = &Chip::dcm0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::dcmAbsY0() {
    _currentInstruction = &Chip::dcmAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::dcmAbsY1() {
    _currentInstruction = &Chip::dcmAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::dcmAbsY2() {
    _currentInstruction = &Chip::dcmAbsY3;
    absoluteIndexedLoad0(&Chip::dcmAbsY3);
}

template <class TBus>
void Chip<TBus>::dcmAbsY3() {
    _currentInstruction = &Chip::dcm0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::dcmIndX0() {
    _currentInstruction = &Chip::dcmIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::dcmIndX1() {
    _currentInstruction = &Chip::dcmIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::dcmIndX2() {
    _currentInstruction = &Chip::dcmIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::dcmIndX3() {
    _currentInstruction = &Chip::dcmIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::dcmIndX4() {
    _currentInstruction = &Chip::dcm0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::dcmIndY0() {
    _currentInstruction = &Chip::dcmIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::dcmIndY1() {
    _currentInstruction = &Chip::dcmIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::dcmIndY2() {
    _currentInstruction = &Chip::dcmIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::dcmIndY3() {
    _currentInstruction = &Chip::dcmIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::dcmIndY4);
}

template <class TBus>
void Chip<TBus>::dcmIndY4() {
    _currentInstruction = &Chip::dcm0;
    zeroPageIndirectPostIndexedLoad1();
}

// INS

template <class TBus>
void Chip<TBus>::ins0() {
    _currentInstruction = &Chip::ins1;
    
    inc(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus>
void Chip<TBus>::ins1() {
    _currentInstruction = &Chip::ins2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_adderHold);
}

template <class TBus>
void Chip<TBus>::ins2() {    // TODO: c'est le meme que adc avec juste l'inversion de bInput, il faut essayer de combiner les 2 pour eviter la duplication de code
    // Substracting data to accumulator with possible carry by inverting bInput
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluInvertBInput();
    aluPerformSum(_flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::ins3);
}

template <class TBus>
void Chip<TBus>::ins3() {
    // Store ALU result in accumulator
    _accumulator = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Overflow, Flag::Negative>(_adderHold);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus>
void Chip<TBus>::insZp0() {
    _currentInstruction = &Chip::insZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::insZp1() {
    _currentInstruction = &Chip::ins0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::insZpX0() {
    _currentInstruction = &Chip::insZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::insZpX1() {
    _currentInstruction = &Chip::insZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::insZpX2() {
    _currentInstruction = &Chip::ins0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::insAbs0() {
    _currentInstruction = &Chip::insAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::insAbs1() {
    _currentInstruction = &Chip::insAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::insAbs2() {
    _currentInstruction = &Chip::ins0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::insAbsX0() {
    _currentInstruction = &Chip::insAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::insAbsX1() {
    _currentInstruction = &Chip::insAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::insAbsX2() {
    _currentInstruction = &Chip::insAbsX3;
    absoluteIndexedLoad0(&Chip::insAbsX3);
}

template <class TBus>
void Chip<TBus>::insAbsX3() {
    _currentInstruction = &Chip::ins0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::insAbsY0() {
    _currentInstruction = &Chip::insAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::insAbsY1() {
    _currentInstruction = &Chip::insAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::insAbsY2() {
    _currentInstruction = &Chip::insAbsY3;
    absoluteIndexedLoad0(&Chip::insAbsY3);
}

template <class TBus>
void Chip<TBus>::insAbsY3() {
    _currentInstruction = &Chip::ins0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::insIndX0() {
    _currentInstruction = &Chip::insIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::insIndX1() {
    _currentInstruction = &Chip::insIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::insIndX2() {
    _currentInstruction = &Chip::insIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::insIndX3() {
    _currentInstruction = &Chip::insIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::insIndX4() {
    _currentInstruction = &Chip::ins0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::insIndY0() {
    _currentInstruction = &Chip::insIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::insIndY1() {
    _currentInstruction = &Chip::insIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::insIndY2() {
    _currentInstruction = &Chip::insIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::insIndY3() {
    _currentInstruction = &Chip::insIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::insIndY4);
}

template <class TBus>
void Chip<TBus>::insIndY4() {
    _currentInstruction = &Chip::ins0;
    zeroPageIndirectPostIndexedLoad1();
}

// ALR

template <class TBus>
void Chip<TBus>::alrImm0() {                 // TODO: voir si ok cette instruction complete
    _currentInstruction = &Chip::alrImm1;
    immediate();
}

template <class TBus>
void Chip<TBus>::alrImm1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // The AND is not performed by the ALU but by bus conflict which cause a low level to win (it is like an AND operation)
    // See : https://wiki.nesdev.com/w/index.php/Bus_conflict
    // And : https://wiki.nesdev.com/w/index.php/Programming_with_unofficial_opcodes
    lsr(_accumulator & _inputDataLatch);
    
    // Write result back
    _accumulator = _adderHold;
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_adderHold);
    
    // Fetch opcode
    fetchOpcode();
}

// NOP

template <class TBus>
void Chip<TBus>::nopImm0() {
    _currentInstruction = &Chip::fetchOpcode;
    immediate();
}

template <class TBus>
void Chip<TBus>::nopZp0() {
    _currentInstruction = &Chip::nopZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::nopZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::nopZpX0() {
    _currentInstruction = &Chip::nopZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::nopZpX1() {
    _currentInstruction = &Chip::nopZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::nopZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::nopAbs0() {
    _currentInstruction = &Chip::nopAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::nopAbs1() {
    _currentInstruction = &Chip::nopAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::nopAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::nopAbsX0() {
    _currentInstruction = &Chip::nopAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::nopAbsX1() {
    _currentInstruction = &Chip::nopAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::nopAbsX2() {
    _currentInstruction = &Chip::nopAbsX3;
    absoluteIndexedLoad0(&Chip::fetchOpcode);
}

template <class TBus>
void Chip<TBus>::nopAbsX3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::kil() {
    // Does nothing and doesn't set the next instruction to cause an infinite loop on this
}

#endif /* Cpu6502_Internal_Undocumented_s_hpp */
