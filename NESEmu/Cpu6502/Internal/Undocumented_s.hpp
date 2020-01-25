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

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::aso0() {
    _currentInstruction = &Chip::aso1;
    asl(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::aso1() {
    _currentInstruction = &Chip::aso2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::aso2() {
    _alu.performOr(_accumulator, _alu.getAdderHold());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoZp0() {
    _currentInstruction = &Chip::asoZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoZp1() {
    _currentInstruction = &Chip::aso0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoZpX0() {
    _currentInstruction = &Chip::asoZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoZpX1() {
    _currentInstruction = &Chip::asoZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoZpX2() {
    _currentInstruction = &Chip::aso0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoAbs0() {
    _currentInstruction = &Chip::asoAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoAbs1() {
    _currentInstruction = &Chip::asoAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoAbs2() {
    _currentInstruction = &Chip::aso0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoAbsX0() {
    _currentInstruction = &Chip::asoAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoAbsX1() {
    _currentInstruction = &Chip::asoAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoAbsX2() {
    _currentInstruction = &Chip::asoAbsX3;
    absoluteIndexedLoad0(&Chip::asoAbsX3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoAbsX3() {
    _currentInstruction = &Chip::aso0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoAbsY0() {
    _currentInstruction = &Chip::asoAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoAbsY1() {
    _currentInstruction = &Chip::asoAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoAbsY2() {
    _currentInstruction = &Chip::asoAbsY3;
    absoluteIndexedLoad0(&Chip::asoAbsY3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoAbsY3() {
    _currentInstruction = &Chip::aso0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoIndX0() {
    _currentInstruction = &Chip::asoIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoIndX1() {
    _currentInstruction = &Chip::asoIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoIndX2() {
    _currentInstruction = &Chip::asoIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoIndX3() {
    _currentInstruction = &Chip::asoIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoIndX4() {
    _currentInstruction = &Chip::aso0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoIndY0() {
    _currentInstruction = &Chip::asoIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoIndY1() {
    _currentInstruction = &Chip::asoIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoIndY2() {
    _currentInstruction = &Chip::asoIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoIndY3() {
    _currentInstruction = &Chip::asoIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::asoIndY4);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::asoIndY4() {
    _currentInstruction = &Chip::aso0;
    zeroPageIndirectPostIndexedLoad1();
}

// RLA

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rla0() {
    _currentInstruction = &Chip::rla1;
    rol(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rla1() {
    _currentInstruction = &Chip::rla2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rla2() {
    _alu.performAnd(_accumulator, _alu.getAdderHold());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaZp0() {
    _currentInstruction = &Chip::rlaZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaZp1() {
    _currentInstruction = &Chip::rla0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaZpX0() {
    _currentInstruction = &Chip::rlaZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaZpX1() {
    _currentInstruction = &Chip::rlaZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaZpX2() {
    _currentInstruction = &Chip::rla0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaAbs0() {
    _currentInstruction = &Chip::rlaAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaAbs1() {
    _currentInstruction = &Chip::rlaAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaAbs2() {
    _currentInstruction = &Chip::rla0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaAbsX0() {
    _currentInstruction = &Chip::rlaAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaAbsX1() {
    _currentInstruction = &Chip::rlaAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaAbsX2() {
    _currentInstruction = &Chip::rlaAbsX3;
    absoluteIndexedLoad0(&Chip::rlaAbsX3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaAbsX3() {
    _currentInstruction = &Chip::rla0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaAbsY0() {
    _currentInstruction = &Chip::rlaAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaAbsY1() {
    _currentInstruction = &Chip::rlaAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaAbsY2() {
    _currentInstruction = &Chip::rlaAbsY3;
    absoluteIndexedLoad0(&Chip::rlaAbsY3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaAbsY3() {
    _currentInstruction = &Chip::rla0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaIndX0() {
    _currentInstruction = &Chip::rlaIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaIndX1() {
    _currentInstruction = &Chip::rlaIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaIndX2() {
    _currentInstruction = &Chip::rlaIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaIndX3() {
    _currentInstruction = &Chip::rlaIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaIndX4() {
    _currentInstruction = &Chip::rla0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaIndY0() {
    _currentInstruction = &Chip::rlaIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaIndY1() {
    _currentInstruction = &Chip::rlaIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaIndY2() {
    _currentInstruction = &Chip::rlaIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaIndY3() {
    _currentInstruction = &Chip::rlaIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::rlaIndY4);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rlaIndY4() {
    _currentInstruction = &Chip::rla0;
    zeroPageIndirectPostIndexedLoad1();
}

// LSE

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lse0() {
    _currentInstruction = &Chip::lse1;
    lsr(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lse1() {
    _currentInstruction = &Chip::lse2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lse2() {
    _alu.performEor(_accumulator, _alu.getAdderHold());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseZp0() {
    _currentInstruction = &Chip::lseZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseZp1() {
    _currentInstruction = &Chip::lse0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseZpX0() {
    _currentInstruction = &Chip::lseZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseZpX1() {
    _currentInstruction = &Chip::lseZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseZpX2() {
    _currentInstruction = &Chip::lse0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseAbs0() {
    _currentInstruction = &Chip::lseAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseAbs1() {
    _currentInstruction = &Chip::lseAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseAbs2() {
    _currentInstruction = &Chip::lse0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseAbsX0() {
    _currentInstruction = &Chip::lseAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseAbsX1() {
    _currentInstruction = &Chip::lseAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseAbsX2() {
    _currentInstruction = &Chip::lseAbsX3;
    absoluteIndexedLoad0(&Chip::lseAbsX3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseAbsX3() {
    _currentInstruction = &Chip::lse0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseAbsY0() {
    _currentInstruction = &Chip::lseAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseAbsY1() {
    _currentInstruction = &Chip::lseAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseAbsY2() {
    _currentInstruction = &Chip::lseAbsY3;
    absoluteIndexedLoad0(&Chip::lseAbsY3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseAbsY3() {
    _currentInstruction = &Chip::lse0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseIndX0() {
    _currentInstruction = &Chip::lseIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseIndX1() {
    _currentInstruction = &Chip::lseIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseIndX2() {
    _currentInstruction = &Chip::lseIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseIndX3() {
    _currentInstruction = &Chip::lseIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseIndX4() {
    _currentInstruction = &Chip::lse0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseIndY0() {
    _currentInstruction = &Chip::lseIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseIndY1() {
    _currentInstruction = &Chip::lseIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseIndY2() {
    _currentInstruction = &Chip::lseIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseIndY3() {
    _currentInstruction = &Chip::lseIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::lseIndY4);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lseIndY4() {
    _currentInstruction = &Chip::lse0;
    zeroPageIndirectPostIndexedLoad1();
}

// RRA

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rra0() {
    _currentInstruction = &Chip::rra1;
    ror(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rra1() {
    _currentInstruction = &Chip::rra2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rra2() {
    // Adding data to accumulator with possible carry
    _alu.performSum<BDecimalSupported, false>(_accumulator, _alu.getAdderHold(), _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::arithmetic1);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraZp0() {
    _currentInstruction = &Chip::rraZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraZp1() {
    _currentInstruction = &Chip::rra0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraZpX0() {
    _currentInstruction = &Chip::rraZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraZpX1() {
    _currentInstruction = &Chip::rraZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraZpX2() {
    _currentInstruction = &Chip::rra0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraAbs0() {
    _currentInstruction = &Chip::rraAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraAbs1() {
    _currentInstruction = &Chip::rraAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraAbs2() {
    _currentInstruction = &Chip::rra0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraAbsX0() {
    _currentInstruction = &Chip::rraAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraAbsX1() {
    _currentInstruction = &Chip::rraAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraAbsX2() {
    _currentInstruction = &Chip::rraAbsX3;
    absoluteIndexedLoad0(&Chip::rraAbsX3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraAbsX3() {
    _currentInstruction = &Chip::rra0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraAbsY0() {
    _currentInstruction = &Chip::rraAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraAbsY1() {
    _currentInstruction = &Chip::rraAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraAbsY2() {
    _currentInstruction = &Chip::rraAbsY3;
    absoluteIndexedLoad0(&Chip::rraAbsY3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraAbsY3() {
    _currentInstruction = &Chip::rra0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraIndX0() {
    _currentInstruction = &Chip::rraIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraIndX1() {
    _currentInstruction = &Chip::rraIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraIndX2() {
    _currentInstruction = &Chip::rraIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraIndX3() {
    _currentInstruction = &Chip::rraIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraIndX4() {
    _currentInstruction = &Chip::rra0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraIndY0() {
    _currentInstruction = &Chip::rraIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraIndY1() {
    _currentInstruction = &Chip::rraIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraIndY2() {
    _currentInstruction = &Chip::rraIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraIndY3() {
    _currentInstruction = &Chip::rraIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::rraIndY4);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rraIndY4() {
    _currentInstruction = &Chip::rra0;
    zeroPageIndirectPostIndexedLoad1();
}

// AXS

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsZp0() {
    _currentInstruction = &Chip::axsZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageStore(_accumulator & _xIndex);
    //staZp1();     // TODO: par apres voir pour ca (avec le dataOutput &=)
    //stxZp1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsZpY0() {
    _currentInstruction = &Chip::axsZpY1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsZpY1() {
    _currentInstruction = &Chip::axsZpY2;
    zeroPageIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsZpY2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedStore(_accumulator & _xIndex);
    // Need to be first because there is no zero page Y addressing mode for STA, so STX will correct the address
    //staZpX2();       // TODO: par apres voir pour ca (avec le dataOutput &=)
    //stxZpY2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsAbs0() {
    _currentInstruction = &Chip::axsAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsAbs1() {
    _currentInstruction = &Chip::axsAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteStore(_accumulator & _xIndex);
    //staAbs2();         // TODO: par apres voir pour ca (avec le dataOutput &=)
    //stxAbs2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsIndX0() {
    _currentInstruction = &Chip::axsIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsIndX1() {
    _currentInstruction = &Chip::axsIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsIndX2() {
    _currentInstruction = &Chip::axsIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsIndX3() {
    _currentInstruction = &Chip::axsIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::axsIndX4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPagePreIndexedIndirectStore(_accumulator & _xIndex);
    // Need to be first because there is no indirect X addressing mode for STX, so STA will correct the address
    //stxAbs2();             // TODO: par apres voir pour ca (avec le dataOutput &=)
    //staIndX4();
}

// LAX

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lax0() {
    lda0();
    
    _xIndex = _accumulator;
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxZp0() {
    _currentInstruction = &Chip::laxZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxZp1() {
    _currentInstruction = &Chip::lax0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxZpY0() {
    _currentInstruction = &Chip::laxZpY1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxZpY1() {
    _currentInstruction = &Chip::laxZpY2;
    zeroPageIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxZpY2() {
    _currentInstruction = &Chip::lax0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxAbs0() {
    _currentInstruction = &Chip::laxAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxAbs1() {
    _currentInstruction = &Chip::laxAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxAbs2() {
    _currentInstruction = &Chip::lax0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxAbsY0() {
    _currentInstruction = &Chip::laxAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxAbsY1() {
    _currentInstruction = &Chip::laxAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxAbsY2() {
    _currentInstruction = &Chip::laxAbsY3;
    absoluteIndexedLoad0(&Chip::lax0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxAbsY3() {
    _currentInstruction = &Chip::lax0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxIndX0() {
    _currentInstruction = &Chip::laxIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxIndX1() {
    _currentInstruction = &Chip::laxIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxIndX2() {
    _currentInstruction = &Chip::laxIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxIndX3() {
    _currentInstruction = &Chip::laxIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxIndX4() {
    _currentInstruction = &Chip::lax0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxIndY0() {
    _currentInstruction = &Chip::laxIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxIndY1() {
    _currentInstruction = &Chip::laxIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxIndY2() {
    _currentInstruction = &Chip::laxIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxIndY3() {
    _currentInstruction = &Chip::laxIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::lax0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::laxIndY4() {
    _currentInstruction = &Chip::lax0;
    zeroPageIndirectPostIndexedLoad1();
}

// DCM

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcm0() {
    _currentInstruction = &Chip::dcm1;
    
    dec(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcm1() {
    _currentInstruction = &Chip::dcm2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcm2() {
    // Substracting data fetched to accumulator
    _alu.performSum<BDecimalSupported, true>(_accumulator, _alu.getAdderHold(), false, true);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::dcm3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcm3() {
    // Don't save result, it's just to set the flags
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmZp0() {
    _currentInstruction = &Chip::dcmZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmZp1() {
    _currentInstruction = &Chip::dcm0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmZpX0() {
    _currentInstruction = &Chip::dcmZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmZpX1() {
    _currentInstruction = &Chip::dcmZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmZpX2() {
    _currentInstruction = &Chip::dcm0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmAbs0() {
    _currentInstruction = &Chip::dcmAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmAbs1() {
    _currentInstruction = &Chip::dcmAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmAbs2() {
    _currentInstruction = &Chip::dcm0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmAbsX0() {
    _currentInstruction = &Chip::dcmAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmAbsX1() {
    _currentInstruction = &Chip::dcmAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmAbsX2() {
    _currentInstruction = &Chip::dcmAbsX3;
    absoluteIndexedLoad0(&Chip::dcmAbsX3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmAbsX3() {
    _currentInstruction = &Chip::dcm0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmAbsY0() {
    _currentInstruction = &Chip::dcmAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmAbsY1() {
    _currentInstruction = &Chip::dcmAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmAbsY2() {
    _currentInstruction = &Chip::dcmAbsY3;
    absoluteIndexedLoad0(&Chip::dcmAbsY3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmAbsY3() {
    _currentInstruction = &Chip::dcm0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmIndX0() {
    _currentInstruction = &Chip::dcmIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmIndX1() {
    _currentInstruction = &Chip::dcmIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmIndX2() {
    _currentInstruction = &Chip::dcmIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmIndX3() {
    _currentInstruction = &Chip::dcmIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmIndX4() {
    _currentInstruction = &Chip::dcm0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmIndY0() {
    _currentInstruction = &Chip::dcmIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmIndY1() {
    _currentInstruction = &Chip::dcmIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmIndY2() {
    _currentInstruction = &Chip::dcmIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmIndY3() {
    _currentInstruction = &Chip::dcmIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::dcmIndY4);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::dcmIndY4() {
    _currentInstruction = &Chip::dcm0;
    zeroPageIndirectPostIndexedLoad1();
}

// INS

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ins0() {
    _currentInstruction = &Chip::ins1;
    
    inc(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ins1() {
    _currentInstruction = &Chip::ins2;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ins2() {
    // Substracting data to accumulator with possible carry by inverting bInput
    _alu.performSum<BDecimalSupported, true>(_accumulator, _alu.getAdderHold(), _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::ins3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ins3() {
    // Store ALU result in accumulator
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Overflow, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insZp0() {
    _currentInstruction = &Chip::insZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insZp1() {
    _currentInstruction = &Chip::ins0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insZpX0() {
    _currentInstruction = &Chip::insZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insZpX1() {
    _currentInstruction = &Chip::insZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insZpX2() {
    _currentInstruction = &Chip::ins0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insAbs0() {
    _currentInstruction = &Chip::insAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insAbs1() {
    _currentInstruction = &Chip::insAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insAbs2() {
    _currentInstruction = &Chip::ins0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insAbsX0() {
    _currentInstruction = &Chip::insAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insAbsX1() {
    _currentInstruction = &Chip::insAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insAbsX2() {
    _currentInstruction = &Chip::insAbsX3;
    absoluteIndexedLoad0(&Chip::insAbsX3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insAbsX3() {
    _currentInstruction = &Chip::ins0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insAbsY0() {
    _currentInstruction = &Chip::insAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insAbsY1() {
    _currentInstruction = &Chip::insAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insAbsY2() {
    _currentInstruction = &Chip::insAbsY3;
    absoluteIndexedLoad0(&Chip::insAbsY3);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insAbsY3() {
    _currentInstruction = &Chip::ins0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insIndX0() {
    _currentInstruction = &Chip::insIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insIndX1() {
    _currentInstruction = &Chip::insIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insIndX2() {
    _currentInstruction = &Chip::insIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insIndX3() {
    _currentInstruction = &Chip::insIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insIndX4() {
    _currentInstruction = &Chip::ins0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insIndY0() {
    _currentInstruction = &Chip::insIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insIndY1() {
    _currentInstruction = &Chip::insIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insIndY2() {
    _currentInstruction = &Chip::insIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insIndY3() {
    _currentInstruction = &Chip::insIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::insIndY4);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::insIndY4() {
    _currentInstruction = &Chip::ins0;
    zeroPageIndirectPostIndexedLoad1();
}

// ALR

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::alrImm0() {                 // TODO: voir si ok cette instruction complete
    _currentInstruction = &Chip::alrImm1;
    immediate();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::alrImm1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // The AND is not performed by the ALU but by bus conflict which cause a low level to win (it is like an AND operation)
    // See : https://wiki.nesdev.com/w/index.php/Bus_conflict
    // And : https://wiki.nesdev.com/w/index.php/Programming_with_unofficial_opcodes
    lsr(_accumulator & _inputDataLatch);
    
    // Write result back
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Fetch opcode
    fetchOpcode();
}

// NOP

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopImm0() {
    _currentInstruction = &Chip::fetchOpcode;
    immediate();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopZp0() {
    _currentInstruction = &Chip::nopZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopZpX0() {
    _currentInstruction = &Chip::nopZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopZpX1() {
    _currentInstruction = &Chip::nopZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopAbs0() {
    _currentInstruction = &Chip::nopAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopAbs1() {
    _currentInstruction = &Chip::nopAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopAbsX0() {
    _currentInstruction = &Chip::nopAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopAbsX1() {
    _currentInstruction = &Chip::nopAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopAbsX2() {
    _currentInstruction = &Chip::nopAbsX3;
    absoluteIndexedLoad0(&Chip::fetchOpcode);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nopAbsX3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::kil() {
    // Does nothing and doesn't set the next instruction to cause an infinite loop on this
}

#endif /* Cpu6502_Internal_Undocumented_s_hpp */
