//
//  Cpu6502Logic_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Logic_s_hpp
#define Cpu6502Logic_s_hpp
// TODO: voir pour les flags

template <class TBus>
void Cpu6502<TBus>::logic1() {
    // Store ALU result in accumulator
    _accumulator = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Zero, Flags::Negative });     // TODO: par apres si beaucoup d'instructions utilisent ca, avoir une methode setZeroNegative(data)
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
    
    // Execute instruction
    decodeOpcode();
}

template <class TBus>
void Cpu6502<TBus>::and0() {
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformAnd();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::logic1);
}

template <class TBus>
void Cpu6502<TBus>::andImm0() {
    _currentInstruction = &Cpu6502::and0;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::andZp0() {
    _currentInstruction = &Cpu6502::andZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::andZp1() {
    _currentInstruction = &Cpu6502::and0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::andZpX0() {
    _currentInstruction = &Cpu6502::andZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::andZpX1() {
    _currentInstruction = &Cpu6502::andZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::andZpX2() {
    _currentInstruction = &Cpu6502::and0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::andAbs0() {
    _currentInstruction = &Cpu6502::andAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::andAbs1() {
    _currentInstruction = &Cpu6502::andAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::andAbs2() {
    _currentInstruction = &Cpu6502::and0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::andAbsX0() {
    _currentInstruction = &Cpu6502::andAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::andAbsX1() {
    _currentInstruction = &Cpu6502::andAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::andAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::andAbsX3;
    absoluteIndexedLoad0(&Cpu6502::and0);
}

template <class TBus>
void Cpu6502<TBus>::andAbsX3() {
    _currentInstruction = &Cpu6502::and0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::andAbsY0() {
    _currentInstruction = &Cpu6502::andAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::andAbsY1() {
    _currentInstruction = &Cpu6502::andAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::andAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::andAbsY3;
    absoluteIndexedLoad0(&Cpu6502::and0);
}

template <class TBus>
void Cpu6502<TBus>::andAbsY3() {
    _currentInstruction = &Cpu6502::and0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::andIndX0() {
    _currentInstruction = &Cpu6502::andIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::andIndX1() {
    _currentInstruction = &Cpu6502::andIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::andIndX2() {
    _currentInstruction = &Cpu6502::andIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::andIndX3() {
    _currentInstruction = &Cpu6502::andIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::andIndX4() {
    _currentInstruction = &Cpu6502::and0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::andIndY0() {
    _currentInstruction = &Cpu6502::andIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::andIndY1() {
    _currentInstruction = &Cpu6502::andIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::andIndY2() {
    _currentInstruction = &Cpu6502::andIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::andIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Cpu6502::andIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::and0);
}

template <class TBus>
void Cpu6502<TBus>::andIndY4() {
    _currentInstruction = &Cpu6502::and0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::ora0() {
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformOr();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::logic1);
}

template <class TBus>
void Cpu6502<TBus>::oraImm0() {
    _currentInstruction = &Cpu6502::ora0;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::oraZp0() {
    _currentInstruction = &Cpu6502::oraZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::oraZp1() {
    _currentInstruction = &Cpu6502::ora0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::oraZpX0() {
    _currentInstruction = &Cpu6502::oraZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::oraZpX1() {
    _currentInstruction = &Cpu6502::oraZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::oraZpX2() {
    _currentInstruction = &Cpu6502::ora0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::oraAbs0() {
    _currentInstruction = &Cpu6502::oraAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::oraAbs1() {
    _currentInstruction = &Cpu6502::oraAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::oraAbs2() {
    _currentInstruction = &Cpu6502::ora0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::oraAbsX0() {
    _currentInstruction = &Cpu6502::oraAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::oraAbsX1() {
    _currentInstruction = &Cpu6502::oraAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::oraAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::oraAbsX3;
    absoluteIndexedLoad0(&Cpu6502::ora0);
}

template <class TBus>
void Cpu6502<TBus>::oraAbsX3() {
    _currentInstruction = &Cpu6502::ora0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::oraAbsY0() {
    _currentInstruction = &Cpu6502::oraAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::oraAbsY1() {
    _currentInstruction = &Cpu6502::oraAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::oraAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::oraAbsY3;
    absoluteIndexedLoad0(&Cpu6502::ora0);
}

template <class TBus>
void Cpu6502<TBus>::oraAbsY3() {
    _currentInstruction = &Cpu6502::ora0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::oraIndX0() {
    _currentInstruction = &Cpu6502::oraIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::oraIndX1() {
    _currentInstruction = &Cpu6502::oraIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::oraIndX2() {
    _currentInstruction = &Cpu6502::oraIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::oraIndX3() {
    _currentInstruction = &Cpu6502::oraIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::oraIndX4() {
    _currentInstruction = &Cpu6502::ora0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::oraIndY0() {
    _currentInstruction = &Cpu6502::oraIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::oraIndY1() {
    _currentInstruction = &Cpu6502::oraIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::oraIndY2() {
    _currentInstruction = &Cpu6502::oraIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::oraIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Cpu6502::oraIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::ora0);
}

template <class TBus>
void Cpu6502<TBus>::oraIndY4() {
    _currentInstruction = &Cpu6502::ora0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::eor0() {
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformEor();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::logic1);
}

template <class TBus>
void Cpu6502<TBus>::eorImm0() {
    _currentInstruction = &Cpu6502::eor0;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::eorZp0() {
    _currentInstruction = &Cpu6502::eorZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::eorZp1() {
    _currentInstruction = &Cpu6502::eor0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::eorZpX0() {
    _currentInstruction = &Cpu6502::eorZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::eorZpX1() {
    _currentInstruction = &Cpu6502::eorZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::eorZpX2() {
    _currentInstruction = &Cpu6502::eor0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::eorAbs0() {
    _currentInstruction = &Cpu6502::eorAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::eorAbs1() {
    _currentInstruction = &Cpu6502::eorAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::eorAbs2() {
    _currentInstruction = &Cpu6502::eor0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::eorAbsX0() {
    _currentInstruction = &Cpu6502::eorAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::eorAbsX1() {
    _currentInstruction = &Cpu6502::eorAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::eorAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::eorAbsX3;
    absoluteIndexedLoad0(&Cpu6502::eor0);
}

template <class TBus>
void Cpu6502<TBus>::eorAbsX3() {
    _currentInstruction = &Cpu6502::eor0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::eorAbsY0() {
    _currentInstruction = &Cpu6502::eorAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::eorAbsY1() {
    _currentInstruction = &Cpu6502::eorAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::eorAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Cpu6502::eorAbsY3;
    absoluteIndexedLoad0(&Cpu6502::eor0);
}

template <class TBus>
void Cpu6502<TBus>::eorAbsY3() {
    _currentInstruction = &Cpu6502::eor0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::eorIndX0() {
    _currentInstruction = &Cpu6502::eorIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::eorIndX1() {
    _currentInstruction = &Cpu6502::eorIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::eorIndX2() {
    _currentInstruction = &Cpu6502::eorIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::eorIndX3() {
    _currentInstruction = &Cpu6502::eorIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::eorIndX4() {
    _currentInstruction = &Cpu6502::eor0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::eorIndY0() {
    _currentInstruction = &Cpu6502::eorIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::eorIndY1() {
    _currentInstruction = &Cpu6502::eorIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::eorIndY2() {
    _currentInstruction = &Cpu6502::eorIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::eorIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Cpu6502::eorIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::eor0);
}

template <class TBus>
void Cpu6502<TBus>::eorIndY4() {
    _currentInstruction = &Cpu6502::eor0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::bit0() {
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformAnd();
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Cpu6502::bit1);
}

template <class TBus>
void Cpu6502<TBus>::bit1() {
    // Don't save result, it's just to set the flags
    
    // Update status (Zero from result, Overflow and Negative from bInput)
    clearStatusFlags({ Flags::Zero, Flags::Overflow, Flags::Negative });     // TODO: par apres si beaucoup d'instructions utilisent ca, avoir une methode setZeroNegative(data)
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Overflow, (_bInput & 0x40));
    setStatusFlag(Flags::Negative, (_bInput & 0x80));
    
    // Execute instruction
    decodeOpcode();
}

template <class TBus>
void Cpu6502<TBus>::bitZp0() {
    _currentInstruction = &Cpu6502::bitZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::bitZp1() {
    _currentInstruction = &Cpu6502::bit0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::bitAbs0() {
    _currentInstruction = &Cpu6502::bitAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::bitAbs1() {
    _currentInstruction = &Cpu6502::bitAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::bitAbs2() {
    _currentInstruction = &Cpu6502::bit0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::asl(uint8_t data) {
    // ASL by adding same number to itself with no carry
    _aInput = data;
    _bInput = data;
    aluPerformSum(false, false);
}

template <class TBus>
void Cpu6502<TBus>::aslMemory0() {
    _currentInstruction = &Cpu6502::aslMemory1;
    asl(_inputDataLatch);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::aslMemory1() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
}

template <class TBus>
void Cpu6502<TBus>::aslImm0() {
    _currentInstruction = &Cpu6502::aslImm1;
    asl(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::aslImm1() {
    // Write result back
    _accumulator = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::aslZp0() {
    _currentInstruction = &Cpu6502::aslZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::aslZp1() {
    _currentInstruction = &Cpu6502::aslMemory0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::aslZpX0() {
    _currentInstruction = &Cpu6502::aslZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::aslZpX1() {
    _currentInstruction = &Cpu6502::aslZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::aslZpX2() {
    _currentInstruction = &Cpu6502::aslMemory0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::aslAbs0() {
    _currentInstruction = &Cpu6502::aslAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::aslAbs1() {
    _currentInstruction = &Cpu6502::aslAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::aslAbs2() {
    _currentInstruction = &Cpu6502::aslMemory0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::aslAbsX0() {
    _currentInstruction = &Cpu6502::aslAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::aslAbsX1() {
    _currentInstruction = &Cpu6502::aslAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::aslAbsX2() {
    _currentInstruction = &Cpu6502::aslAbsX3;
    absoluteIndexedLoad0(&Cpu6502::aslAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::aslAbsX3() {
    _currentInstruction = &Cpu6502::aslMemory0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::lsr(uint8_t data) {
    // LSR by shift right with no carry
    _aInput = data;
    aluPerformShiftRight(false);
}

template <class TBus>
void Cpu6502<TBus>::lsrMemory0() {
    _currentInstruction = &Cpu6502::lsrMemory1;
    lsr(_inputDataLatch);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::lsrMemory1() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    //setStatusFlag(Flags::Negative, (_adderHold & 0x80));  // TODO: pas besoin car le bit ajouté a gauche est tjs 0
}

template <class TBus>
void Cpu6502<TBus>::lsrImm0() {
    _currentInstruction = &Cpu6502::lsrImm1;
    lsr(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::lsrImm1() {
    // Write result back
    _accumulator = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    //setStatusFlag(Flags::Negative, (_adderHold & 0x80));  // TODO: pas besoin car le bit ajouté a gauche est tjs 0
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::lsrZp0() {
    _currentInstruction = &Cpu6502::lsrZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::lsrZp1() {
    _currentInstruction = &Cpu6502::lsrMemory0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::lsrZpX0() {
    _currentInstruction = &Cpu6502::lsrZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::lsrZpX1() {
    _currentInstruction = &Cpu6502::lsrZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::lsrZpX2() {
    _currentInstruction = &Cpu6502::lsrMemory0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::lsrAbs0() {
    _currentInstruction = &Cpu6502::lsrAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::lsrAbs1() {
    _currentInstruction = &Cpu6502::lsrAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::lsrAbs2() {
    _currentInstruction = &Cpu6502::lsrMemory0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::lsrAbsX0() {
    _currentInstruction = &Cpu6502::lsrAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::lsrAbsX1() {
    _currentInstruction = &Cpu6502::lsrAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::lsrAbsX2() {
    _currentInstruction = &Cpu6502::lsrAbsX3;
    absoluteIndexedLoad0(&Cpu6502::lsrAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::lsrAbsX3() {
    _currentInstruction = &Cpu6502::lsrMemory0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::rol(uint8_t data) {
    // ROL by adding same number to itself with carry
    _aInput = data;
    _bInput = data;
    aluPerformSum(false, getStatusFlag(Flags::Carry));
}

template <class TBus>
void Cpu6502<TBus>::rolMemory0() {
    _currentInstruction = &Cpu6502::rolMemory1;
    rol(_inputDataLatch);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::rolMemory1() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
}

template <class TBus>
void Cpu6502<TBus>::rolImm0() {
    _currentInstruction = &Cpu6502::rolImm1;
    rol(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::rolImm1() {
    // Write result back
    _accumulator = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::rolZp0() {
    _currentInstruction = &Cpu6502::rolZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::rolZp1() {
    _currentInstruction = &Cpu6502::rolMemory0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::rolZpX0() {
    _currentInstruction = &Cpu6502::rolZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rolZpX1() {
    _currentInstruction = &Cpu6502::rolZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::rolZpX2() {
    _currentInstruction = &Cpu6502::rolMemory0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::rolAbs0() {
    _currentInstruction = &Cpu6502::rolAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::rolAbs1() {
    _currentInstruction = &Cpu6502::rolAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::rolAbs2() {
    _currentInstruction = &Cpu6502::rolMemory0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::rolAbsX0() {
    _currentInstruction = &Cpu6502::rolAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rolAbsX1() {
    _currentInstruction = &Cpu6502::rolAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::rolAbsX2() {
    _currentInstruction = &Cpu6502::rolAbsX3;
    absoluteIndexedLoad0(&Cpu6502::rolAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::rolAbsX3() {
    _currentInstruction = &Cpu6502::rolMemory0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::ror(uint8_t data) {
    // ROR by shift right with current carry
    _aInput = data;
    aluPerformShiftRight(getStatusFlag({ Flags::Carry }));
}

template <class TBus>
void Cpu6502<TBus>::rorMemory0() {
    _currentInstruction = &Cpu6502::rorMemory1;
    ror(_inputDataLatch);
    
    // Write result back
    //writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);//TODO: voir si mettre ca
}

template <class TBus>
void Cpu6502<TBus>::rorMemory1() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
}

template <class TBus>
void Cpu6502<TBus>::rorImm0() {
    _currentInstruction = &Cpu6502::rorImm1;
    ror(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::rorImm1() {
    // Write result back
    _accumulator = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _aluCarry);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::rorZp0() {
    _currentInstruction = &Cpu6502::rorZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::rorZp1() {
    _currentInstruction = &Cpu6502::rorMemory0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::rorZpX0() {
    _currentInstruction = &Cpu6502::rorZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rorZpX1() {
    _currentInstruction = &Cpu6502::rorZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::rorZpX2() {
    _currentInstruction = &Cpu6502::rorMemory0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::rorAbs0() {
    _currentInstruction = &Cpu6502::rorAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::rorAbs1() {
    _currentInstruction = &Cpu6502::rorAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::rorAbs2() {
    _currentInstruction = &Cpu6502::rorMemory0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::rorAbsX0() {
    _currentInstruction = &Cpu6502::rorAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::rorAbsX1() {
    _currentInstruction = &Cpu6502::rorAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::rorAbsX2() {
    _currentInstruction = &Cpu6502::rorAbsX3;
    absoluteIndexedLoad0(&Cpu6502::rorAbsX3);
}

template <class TBus>
void Cpu6502<TBus>::rorAbsX3() {
    _currentInstruction = &Cpu6502::rorMemory0;
    absoluteIndexedLoad1();
}

#endif /* Cpu6502Logic_s_hpp */
