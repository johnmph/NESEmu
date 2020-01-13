//
//  Cpu6502Undocumented_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Undocumented_s_hpp
#define Cpu6502Undocumented_s_hpp


// ASO

template <class TBus>
void Cpu6502<TBus>::aso0() {
    _currentInstruction = &Cpu6502::aso1;
    asl(_inputDataLatch);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::aso1() {
    _currentInstruction = &Cpu6502::aso2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
}

template <class TBus>
void Cpu6502<TBus>::aso2() {
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluPerformOr();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::logic1);
}

template <class TBus>
void Cpu6502<TBus>::asoZp0() {
    _currentInstruction = &Cpu6502::asoZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::asoZp1() {
    _currentInstruction = &Cpu6502::aso0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::asoZpX0() {
    _currentInstruction = &Cpu6502::asoZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::asoZpX1() {
    _currentInstruction = &Cpu6502::asoZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::asoZpX2() {
    _currentInstruction = &Cpu6502::aso0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::asoAbs0() {
    _currentInstruction = &Cpu6502::asoAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::asoAbs1() {
    _currentInstruction = &Cpu6502::asoAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::asoAbs2() {
    _currentInstruction = &Cpu6502::aso0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::asoAbsX0() {
    _currentInstruction = &Cpu6502::asoAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::asoAbsX1() {
    _currentInstruction = &Cpu6502::asoAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::asoAbsX2() {
    _currentInstruction = &Cpu6502::asoAbsX3;
    absoluteIndexedLoad0(&Cpu6502::asoAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::asoAbsX3() {
    _currentInstruction = &Cpu6502::aso0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::asoAbsY0() {
    _currentInstruction = &Cpu6502::asoAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::asoAbsY1() {
    _currentInstruction = &Cpu6502::asoAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::asoAbsY2() {
    _currentInstruction = &Cpu6502::asoAbsY3;
    absoluteIndexedLoad0(&Cpu6502::asoAbsY3);
}

template <class TBus>
void Cpu6502<TBus>::asoAbsY3() {
    _currentInstruction = &Cpu6502::aso0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::asoIndX0() {
    _currentInstruction = &Cpu6502::asoIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::asoIndX1() {
    _currentInstruction = &Cpu6502::asoIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::asoIndX2() {
    _currentInstruction = &Cpu6502::asoIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::asoIndX3() {
    _currentInstruction = &Cpu6502::asoIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::asoIndX4() {
    _currentInstruction = &Cpu6502::aso0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::asoIndY0() {
    _currentInstruction = &Cpu6502::asoIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::asoIndY1() {
    _currentInstruction = &Cpu6502::asoIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::asoIndY2() {
    _currentInstruction = &Cpu6502::asoIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::asoIndY3() {
    _currentInstruction = &Cpu6502::asoIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::asoIndY4);
}

template <class TBus>
void Cpu6502<TBus>::asoIndY4() {
    _currentInstruction = &Cpu6502::aso0;
    zeroPageIndirectPostIndexedLoad1();
}

// RLO

template <class TBus>
void Cpu6502<TBus>::rla0() {
    _currentInstruction = &Cpu6502::rla1;
    rol(_inputDataLatch);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::rla1() {
    _currentInstruction = &Cpu6502::rla2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
}

template <class TBus>
void Cpu6502<TBus>::rla2() {
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluPerformAnd();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::logic1);
}

template <class TBus>
void Cpu6502<TBus>::rlaZp0() {
    _currentInstruction = &Cpu6502::rlaZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::rlaZp1() {
    _currentInstruction = &Cpu6502::rla0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::rlaZpX0() {
    _currentInstruction = &Cpu6502::rlaZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rlaZpX1() {
    _currentInstruction = &Cpu6502::rlaZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::rlaZpX2() {
    _currentInstruction = &Cpu6502::rla0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::rlaAbs0() {
    _currentInstruction = &Cpu6502::rlaAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::rlaAbs1() {
    _currentInstruction = &Cpu6502::rlaAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::rlaAbs2() {
    _currentInstruction = &Cpu6502::rla0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::rlaAbsX0() {
    _currentInstruction = &Cpu6502::rlaAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rlaAbsX1() {
    _currentInstruction = &Cpu6502::rlaAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::rlaAbsX2() {
    _currentInstruction = &Cpu6502::rlaAbsX3;
    absoluteIndexedLoad0(&Cpu6502::rlaAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::rlaAbsX3() {
    _currentInstruction = &Cpu6502::rla0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::rlaAbsY0() {
    _currentInstruction = &Cpu6502::rlaAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rlaAbsY1() {
    _currentInstruction = &Cpu6502::rlaAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::rlaAbsY2() {
    _currentInstruction = &Cpu6502::rlaAbsY3;
    absoluteIndexedLoad0(&Cpu6502::rlaAbsY3);
}

template <class TBus>
void Cpu6502<TBus>::rlaAbsY3() {
    _currentInstruction = &Cpu6502::rla0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::rlaIndX0() {
    _currentInstruction = &Cpu6502::rlaIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::rlaIndX1() {
    _currentInstruction = &Cpu6502::rlaIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::rlaIndX2() {
    _currentInstruction = &Cpu6502::rlaIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::rlaIndX3() {
    _currentInstruction = &Cpu6502::rlaIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::rlaIndX4() {
    _currentInstruction = &Cpu6502::rla0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::rlaIndY0() {
    _currentInstruction = &Cpu6502::rlaIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rlaIndY1() {
    _currentInstruction = &Cpu6502::rlaIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::rlaIndY2() {
    _currentInstruction = &Cpu6502::rlaIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::rlaIndY3() {
    _currentInstruction = &Cpu6502::rlaIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::rlaIndY4);
}

template <class TBus>
void Cpu6502<TBus>::rlaIndY4() {
    _currentInstruction = &Cpu6502::rla0;
    zeroPageIndirectPostIndexedLoad1();
}

// LSE

template <class TBus>
void Cpu6502<TBus>::lse0() {
    _currentInstruction = &Cpu6502::lse1;
    lsr(_inputDataLatch);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::lse1() {
    _currentInstruction = &Cpu6502::lse2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
}

template <class TBus>
void Cpu6502<TBus>::lse2() {
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluPerformEor();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::logic1);
}

template <class TBus>
void Cpu6502<TBus>::lseZp0() {
    _currentInstruction = &Cpu6502::lseZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::lseZp1() {
    _currentInstruction = &Cpu6502::lse0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::lseZpX0() {
    _currentInstruction = &Cpu6502::lseZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::lseZpX1() {
    _currentInstruction = &Cpu6502::lseZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::lseZpX2() {
    _currentInstruction = &Cpu6502::lse0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::lseAbs0() {
    _currentInstruction = &Cpu6502::lseAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::lseAbs1() {
    _currentInstruction = &Cpu6502::lseAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::lseAbs2() {
    _currentInstruction = &Cpu6502::lse0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::lseAbsX0() {
    _currentInstruction = &Cpu6502::lseAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::lseAbsX1() {
    _currentInstruction = &Cpu6502::lseAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::lseAbsX2() {
    _currentInstruction = &Cpu6502::lseAbsX3;
    absoluteIndexedLoad0(&Cpu6502::lseAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::lseAbsX3() {
    _currentInstruction = &Cpu6502::lse0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::lseAbsY0() {
    _currentInstruction = &Cpu6502::lseAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::lseAbsY1() {
    _currentInstruction = &Cpu6502::lseAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::lseAbsY2() {
    _currentInstruction = &Cpu6502::lseAbsY3;
    absoluteIndexedLoad0(&Cpu6502::lseAbsY3);
}

template <class TBus>
void Cpu6502<TBus>::lseAbsY3() {
    _currentInstruction = &Cpu6502::lse0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::lseIndX0() {
    _currentInstruction = &Cpu6502::lseIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::lseIndX1() {
    _currentInstruction = &Cpu6502::lseIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::lseIndX2() {
    _currentInstruction = &Cpu6502::lseIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::lseIndX3() {
    _currentInstruction = &Cpu6502::lseIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::lseIndX4() {
    _currentInstruction = &Cpu6502::lse0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::lseIndY0() {
    _currentInstruction = &Cpu6502::lseIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::lseIndY1() {
    _currentInstruction = &Cpu6502::lseIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::lseIndY2() {
    _currentInstruction = &Cpu6502::lseIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::lseIndY3() {
    _currentInstruction = &Cpu6502::lseIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::lseIndY4);
}

template <class TBus>
void Cpu6502<TBus>::lseIndY4() {
    _currentInstruction = &Cpu6502::lse0;
    zeroPageIndirectPostIndexedLoad1();
}

// RRA

template <class TBus>
void Cpu6502<TBus>::rra0() {
    _currentInstruction = &Cpu6502::rra1;
    ror(_inputDataLatch);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::rra1() {
    _currentInstruction = &Cpu6502::rra2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
}

template <class TBus>
void Cpu6502<TBus>::rra2() {
    // Adding data to accumulator with possible carry
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluPerformSum(getStatusFlag(Flags::DecimalMode), getStatusFlag(Flags::Carry));
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::adc1);
}

template <class TBus>
void Cpu6502<TBus>::rraZp0() {
    _currentInstruction = &Cpu6502::rraZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::rraZp1() {
    _currentInstruction = &Cpu6502::rra0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::rraZpX0() {
    _currentInstruction = &Cpu6502::rraZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rraZpX1() {
    _currentInstruction = &Cpu6502::rraZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::rraZpX2() {
    _currentInstruction = &Cpu6502::rra0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::rraAbs0() {
    _currentInstruction = &Cpu6502::rraAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::rraAbs1() {
    _currentInstruction = &Cpu6502::rraAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::rraAbs2() {
    _currentInstruction = &Cpu6502::rra0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::rraAbsX0() {
    _currentInstruction = &Cpu6502::rraAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rraAbsX1() {
    _currentInstruction = &Cpu6502::rraAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::rraAbsX2() {
    _currentInstruction = &Cpu6502::rraAbsX3;
    absoluteIndexedLoad0(&Cpu6502::rraAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::rraAbsX3() {
    _currentInstruction = &Cpu6502::rra0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::rraAbsY0() {
    _currentInstruction = &Cpu6502::rraAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rraAbsY1() {
    _currentInstruction = &Cpu6502::rraAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::rraAbsY2() {
    _currentInstruction = &Cpu6502::rraAbsY3;
    absoluteIndexedLoad0(&Cpu6502::rraAbsY3);
}

template <class TBus>
void Cpu6502<TBus>::rraAbsY3() {
    _currentInstruction = &Cpu6502::rra0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::rraIndX0() {
    _currentInstruction = &Cpu6502::rraIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::rraIndX1() {
    _currentInstruction = &Cpu6502::rraIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::rraIndX2() {
    _currentInstruction = &Cpu6502::rraIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::rraIndX3() {
    _currentInstruction = &Cpu6502::rraIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::rraIndX4() {
    _currentInstruction = &Cpu6502::rra0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::rraIndY0() {
    _currentInstruction = &Cpu6502::rraIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rraIndY1() {
    _currentInstruction = &Cpu6502::rraIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::rraIndY2() {
    _currentInstruction = &Cpu6502::rraIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::rraIndY3() {
    _currentInstruction = &Cpu6502::rraIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::rraIndY4);
}

template <class TBus>
void Cpu6502<TBus>::rraIndY4() {
    _currentInstruction = &Cpu6502::rra0;
    zeroPageIndirectPostIndexedLoad1();
}


// AXS

template <class TBus>
void Cpu6502<TBus>::axsZp0() {
    _currentInstruction = &Cpu6502::axsZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::axsZp1() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPageStore(_accumulator & _xIndex);
}

template <class TBus>
void Cpu6502<TBus>::axsZpY0() {
    _currentInstruction = &Cpu6502::axsZpY1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::axsZpY1() {
    _currentInstruction = &Cpu6502::axsZpY2;
    zeroPageIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::axsZpY2() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPageIndexedStore(_accumulator & _xIndex);
}

template <class TBus>
void Cpu6502<TBus>::axsAbs0() {
    _currentInstruction = &Cpu6502::axsAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::axsAbs1() {
    _currentInstruction = &Cpu6502::axsAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::axsAbs2() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    absoluteStore(_accumulator & _xIndex);
}

template <class TBus>
void Cpu6502<TBus>::axsIndX0() {
    _currentInstruction = &Cpu6502::axsIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::axsIndX1() {
    _currentInstruction = &Cpu6502::axsIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::axsIndX2() {
    _currentInstruction = &Cpu6502::axsIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::axsIndX3() {
    _currentInstruction = &Cpu6502::axsIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::axsIndX4() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPagePreIndexedIndirectStore(_accumulator & _xIndex);
}

// LAX

template <class TBus>
void Cpu6502<TBus>::lax0() {
    lda0();
    
    _xIndex = _accumulator;
}

template <class TBus>
void Cpu6502<TBus>::laxZp0() {
    _currentInstruction = &Cpu6502::laxZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::laxZp1() {
    _currentInstruction = &Cpu6502::lax0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::laxZpY0() {
    _currentInstruction = &Cpu6502::laxZpY1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::laxZpY1() {
    _currentInstruction = &Cpu6502::laxZpY2;
    zeroPageIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::laxZpY2() {
    _currentInstruction = &Cpu6502::lax0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::laxAbs0() {
    _currentInstruction = &Cpu6502::laxAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::laxAbs1() {
    _currentInstruction = &Cpu6502::laxAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::laxAbs2() {
    _currentInstruction = &Cpu6502::lax0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::laxAbsY0() {
    _currentInstruction = &Cpu6502::laxAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::laxAbsY1() {
    _currentInstruction = &Cpu6502::laxAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::laxAbsY2() {
    _currentInstruction = &Cpu6502::laxAbsY3;
    absoluteIndexedLoad0(&Cpu6502::lax0);
}

template <class TBus>
void Cpu6502<TBus>::laxAbsY3() {
    _currentInstruction = &Cpu6502::lax0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::laxIndX0() {
    _currentInstruction = &Cpu6502::laxIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::laxIndX1() {
    _currentInstruction = &Cpu6502::laxIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::laxIndX2() {
    _currentInstruction = &Cpu6502::laxIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::laxIndX3() {
    _currentInstruction = &Cpu6502::laxIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::laxIndX4() {
    _currentInstruction = &Cpu6502::lax0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::laxIndY0() {
    _currentInstruction = &Cpu6502::laxIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::laxIndY1() {
    _currentInstruction = &Cpu6502::laxIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::laxIndY2() {
    _currentInstruction = &Cpu6502::laxIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::laxIndY3() {
    _currentInstruction = &Cpu6502::laxIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::lax0);
}

template <class TBus>
void Cpu6502<TBus>::laxIndY4() {
    _currentInstruction = &Cpu6502::lax0;
    zeroPageIndirectPostIndexedLoad1();
}

// DCP

template <class TBus>
void Cpu6502<TBus>::dcp0() {
    _currentInstruction = &Cpu6502::dcp1;
    
    // Removing 1 from inputDataLatch using ALU (Add 0xFF without carry set like true 6502)
    _aInput = 0xFF;
    _bInput = _inputDataLatch;
    aluPerformSum(false, false);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::dcp1() {
    _currentInstruction = &Cpu6502::dcp2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
}

template <class TBus>
void Cpu6502<TBus>::dcp2() {
    // Substracting data fetched to accumulator
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluInvertBInput();
    aluPerformSum(false, true);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::dcp3);
}

template <class TBus>
void Cpu6502<TBus>::dcp3() {
    // Don't save result, it's just to set the flags
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
    
    // Execute instruction
    decodeOpcode();
}

template <class TBus>
void Cpu6502<TBus>::dcpZp0() {
    _currentInstruction = &Cpu6502::dcpZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::dcpZp1() {
    _currentInstruction = &Cpu6502::dcp0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::dcpZpX0() {
    _currentInstruction = &Cpu6502::dcpZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::dcpZpX1() {
    _currentInstruction = &Cpu6502::dcpZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::dcpZpX2() {
    _currentInstruction = &Cpu6502::dcp0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::dcpAbs0() {
    _currentInstruction = &Cpu6502::dcpAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::dcpAbs1() {
    _currentInstruction = &Cpu6502::dcpAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::dcpAbs2() {
    _currentInstruction = &Cpu6502::dcp0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::dcpAbsX0() {
    _currentInstruction = &Cpu6502::dcpAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::dcpAbsX1() {
    _currentInstruction = &Cpu6502::dcpAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::dcpAbsX2() {
    _currentInstruction = &Cpu6502::dcpAbsX3;
    absoluteIndexedLoad0(&Cpu6502::dcpAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::dcpAbsX3() {
    _currentInstruction = &Cpu6502::dcp0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::dcpAbsY0() {
    _currentInstruction = &Cpu6502::dcpAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::dcpAbsY1() {
    _currentInstruction = &Cpu6502::dcpAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::dcpAbsY2() {
    _currentInstruction = &Cpu6502::dcpAbsY3;
    absoluteIndexedLoad0(&Cpu6502::dcpAbsY3);
}

template <class TBus>
void Cpu6502<TBus>::dcpAbsY3() {
    _currentInstruction = &Cpu6502::dcp0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::dcpIndX0() {
    _currentInstruction = &Cpu6502::dcpIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::dcpIndX1() {
    _currentInstruction = &Cpu6502::dcpIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::dcpIndX2() {
    _currentInstruction = &Cpu6502::dcpIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::dcpIndX3() {
    _currentInstruction = &Cpu6502::dcpIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::dcpIndX4() {
    _currentInstruction = &Cpu6502::dcp0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::dcpIndY0() {
    _currentInstruction = &Cpu6502::dcpIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::dcpIndY1() {
    _currentInstruction = &Cpu6502::dcpIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::dcpIndY2() {
    _currentInstruction = &Cpu6502::dcpIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::dcpIndY3() {
    _currentInstruction = &Cpu6502::dcpIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::dcpIndY4);
}

template <class TBus>
void Cpu6502<TBus>::dcpIndY4() {
    _currentInstruction = &Cpu6502::dcp0;
    zeroPageIndirectPostIndexedLoad1();
}

// INS

template <class TBus>
void Cpu6502<TBus>::ins0() {
    _currentInstruction = &Cpu6502::ins1;
    
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502)
    _aInput = 0;
    _bInput = _inputDataLatch;
    aluPerformSum(false, true);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::ins1() {
    _currentInstruction = &Cpu6502::ins2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    clearStatusFlags({ Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
}

template <class TBus>
void Cpu6502<TBus>::ins2() {    // TODO: c'est le meme que adc avec juste l'inversion de bInput, il faut essayer de combiner les 2 pour eviter la duplication de code
    // Substracting data to accumulator with possible carry by inverting bInput
    _aInput = _accumulator;
    _bInput = _adderHold;
    aluInvertBInput();
    aluPerformSum(getStatusFlag(Flags::DecimalMode), getStatusFlag(Flags::Carry));
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::ins3);
}

template <class TBus>
void Cpu6502<TBus>::ins3() {
    // Store ALU result in accumulator
    _accumulator = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Overflow, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Overflow, _aluOverflow);
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
    
    // Execute instruction
    decodeOpcode();
}

template <class TBus>
void Cpu6502<TBus>::insZp0() {
    _currentInstruction = &Cpu6502::insZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::insZp1() {
    _currentInstruction = &Cpu6502::ins0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::insZpX0() {
    _currentInstruction = &Cpu6502::insZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::insZpX1() {
    _currentInstruction = &Cpu6502::insZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::insZpX2() {
    _currentInstruction = &Cpu6502::ins0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::insAbs0() {
    _currentInstruction = &Cpu6502::insAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::insAbs1() {
    _currentInstruction = &Cpu6502::insAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::insAbs2() {
    _currentInstruction = &Cpu6502::ins0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::insAbsX0() {
    _currentInstruction = &Cpu6502::insAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::insAbsX1() {
    _currentInstruction = &Cpu6502::insAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::insAbsX2() {
    _currentInstruction = &Cpu6502::insAbsX3;
    absoluteIndexedLoad0(&Cpu6502::insAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::insAbsX3() {
    _currentInstruction = &Cpu6502::ins0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::insAbsY0() {
    _currentInstruction = &Cpu6502::insAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::insAbsY1() {
    _currentInstruction = &Cpu6502::insAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::insAbsY2() {
    _currentInstruction = &Cpu6502::insAbsY3;
    absoluteIndexedLoad0(&Cpu6502::insAbsY3);
}

template <class TBus>
void Cpu6502<TBus>::insAbsY3() {
    _currentInstruction = &Cpu6502::ins0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::insIndX0() {
    _currentInstruction = &Cpu6502::insIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::insIndX1() {
    _currentInstruction = &Cpu6502::insIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::insIndX2() {
    _currentInstruction = &Cpu6502::insIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::insIndX3() {
    _currentInstruction = &Cpu6502::insIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::insIndX4() {
    _currentInstruction = &Cpu6502::ins0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::insIndY0() {
    _currentInstruction = &Cpu6502::insIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::insIndY1() {
    _currentInstruction = &Cpu6502::insIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::insIndY2() {
    _currentInstruction = &Cpu6502::insIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::insIndY3() {
    _currentInstruction = &Cpu6502::insIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::insIndY4);
}

template <class TBus>
void Cpu6502<TBus>::insIndY4() {
    _currentInstruction = &Cpu6502::ins0;
    zeroPageIndirectPostIndexedLoad1();
}

//

template <class TBus>
void Cpu6502<TBus>::nopImm0() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::nopZp0() {
    _currentInstruction = &Cpu6502::nopZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::nopZp1() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::nopZpX0() {
    _currentInstruction = &Cpu6502::nopZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::nopZpX1() {
    _currentInstruction = &Cpu6502::nopZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::nopZpX2() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::nopAbs0() {
    _currentInstruction = &Cpu6502::nopAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::nopAbs1() {
    _currentInstruction = &Cpu6502::nopAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::nopAbs2() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::nopAbsX0() {
    _currentInstruction = &Cpu6502::nopAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::nopAbsX1() {
    _currentInstruction = &Cpu6502::nopAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::nopAbsX2() {
    _currentInstruction = &Cpu6502::nopAbsX3;
    absoluteIndexedLoad0(&Cpu6502::fetchOpcode);
}

template <class TBus>
void Cpu6502<TBus>::nopAbsX3() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::kil() {
    // Does nothing and doesn't set the next instruction to cause an infinite loop on this
}

#endif /* Cpu6502Undocumented_s_hpp */
