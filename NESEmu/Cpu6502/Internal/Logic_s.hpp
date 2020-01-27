//
//  Logic_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Logic_s_hpp
#define Cpu6502_Internal_Logic_s_hpp


template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::logic1() {
    // Store ALU result in accumulator
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::and0() {
    _alu.performAnd(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andImm0() {
    _currentInstruction = &Chip::and0;
    immediate();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andZp0() {
    _currentInstruction = &Chip::andZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andZp1() {
    _currentInstruction = &Chip::and0;
    zeroPageLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andZpX0() {
    _currentInstruction = &Chip::andZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andZpX1() {
    _currentInstruction = &Chip::andZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andZpX2() {
    _currentInstruction = &Chip::and0;
    zeroPageIndexedLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andAbs0() {
    _currentInstruction = &Chip::andAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andAbs1() {
    _currentInstruction = &Chip::andAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andAbs2() {
    _currentInstruction = &Chip::and0;
    absoluteLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andAbsX0() {
    _currentInstruction = &Chip::andAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andAbsX1() {
    _currentInstruction = &Chip::andAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::andAbsX3;
    absoluteIndexedLoad0(&Chip::and0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andAbsX3() {
    _currentInstruction = &Chip::and0;
    absoluteIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andAbsY0() {
    _currentInstruction = &Chip::andAbsY1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andAbsY1() {
    _currentInstruction = &Chip::andAbsY2;
    absoluteIndexedY1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::andAbsY3;
    absoluteIndexedLoad0(&Chip::and0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andAbsY3() {
    _currentInstruction = &Chip::and0;
    absoluteIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andIndX0() {
    _currentInstruction = &Chip::andIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andIndX1() {
    _currentInstruction = &Chip::andIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andIndX2() {
    _currentInstruction = &Chip::andIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andIndX3() {
    _currentInstruction = &Chip::andIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andIndX4() {
    _currentInstruction = &Chip::and0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andIndY0() {
    _currentInstruction = &Chip::andIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andIndY1() {
    _currentInstruction = &Chip::andIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andIndY2() {
    _currentInstruction = &Chip::andIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::andIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::and0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::andIndY4() {
    _currentInstruction = &Chip::and0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ora0() {
    _alu.performOr(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraImm0() {
    _currentInstruction = &Chip::ora0;
    immediate();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraZp0() {
    _currentInstruction = &Chip::oraZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraZp1() {
    _currentInstruction = &Chip::ora0;
    zeroPageLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraZpX0() {
    _currentInstruction = &Chip::oraZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraZpX1() {
    _currentInstruction = &Chip::oraZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraZpX2() {
    _currentInstruction = &Chip::ora0;
    zeroPageIndexedLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraAbs0() {
    _currentInstruction = &Chip::oraAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraAbs1() {
    _currentInstruction = &Chip::oraAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraAbs2() {
    _currentInstruction = &Chip::ora0;
    absoluteLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraAbsX0() {
    _currentInstruction = &Chip::oraAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraAbsX1() {
    _currentInstruction = &Chip::oraAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::oraAbsX3;
    absoluteIndexedLoad0(&Chip::ora0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraAbsX3() {
    _currentInstruction = &Chip::ora0;
    absoluteIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraAbsY0() {
    _currentInstruction = &Chip::oraAbsY1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraAbsY1() {
    _currentInstruction = &Chip::oraAbsY2;
    absoluteIndexedY1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::oraAbsY3;
    absoluteIndexedLoad0(&Chip::ora0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraAbsY3() {
    _currentInstruction = &Chip::ora0;
    absoluteIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraIndX0() {
    _currentInstruction = &Chip::oraIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraIndX1() {
    _currentInstruction = &Chip::oraIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraIndX2() {
    _currentInstruction = &Chip::oraIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraIndX3() {
    _currentInstruction = &Chip::oraIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraIndX4() {
    _currentInstruction = &Chip::ora0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraIndY0() {
    _currentInstruction = &Chip::oraIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraIndY1() {
    _currentInstruction = &Chip::oraIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraIndY2() {
    _currentInstruction = &Chip::oraIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::oraIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::ora0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::oraIndY4() {
    _currentInstruction = &Chip::ora0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eor0() {
    _alu.performEor(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::logic1);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorImm0() {
    _currentInstruction = &Chip::eor0;
    immediate();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorZp0() {
    _currentInstruction = &Chip::eorZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorZp1() {
    _currentInstruction = &Chip::eor0;
    zeroPageLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorZpX0() {
    _currentInstruction = &Chip::eorZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorZpX1() {
    _currentInstruction = &Chip::eorZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorZpX2() {
    _currentInstruction = &Chip::eor0;
    zeroPageIndexedLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorAbs0() {
    _currentInstruction = &Chip::eorAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorAbs1() {
    _currentInstruction = &Chip::eorAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorAbs2() {
    _currentInstruction = &Chip::eor0;
    absoluteLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorAbsX0() {
    _currentInstruction = &Chip::eorAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorAbsX1() {
    _currentInstruction = &Chip::eorAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::eorAbsX3;
    absoluteIndexedLoad0(&Chip::eor0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorAbsX3() {
    _currentInstruction = &Chip::eor0;
    absoluteIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorAbsY0() {
    _currentInstruction = &Chip::eorAbsY1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorAbsY1() {
    _currentInstruction = &Chip::eorAbsY2;
    absoluteIndexedY1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexLoad0 because it can modify it
    _currentInstruction = &Chip::eorAbsY3;
    absoluteIndexedLoad0(&Chip::eor0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorAbsY3() {
    _currentInstruction = &Chip::eor0;
    absoluteIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorIndX0() {
    _currentInstruction = &Chip::eorIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorIndX1() {
    _currentInstruction = &Chip::eorIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorIndX2() {
    _currentInstruction = &Chip::eorIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorIndX3() {
    _currentInstruction = &Chip::eorIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorIndX4() {
    _currentInstruction = &Chip::eor0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorIndY0() {
    _currentInstruction = &Chip::eorIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorIndY1() {
    _currentInstruction = &Chip::eorIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorIndY2() {
    _currentInstruction = &Chip::eorIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedLoad0 because it can modify it
    _currentInstruction = &Chip::eorIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::eor0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::eorIndY4() {
    _currentInstruction = &Chip::eor0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bit0() {
    _alu.performAnd(_accumulator, _inputDataLatch);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::bit1);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bit1() {
    // Don't save result, it's just to set the flags
    
    // Update status (Zero from result, Overflow and Negative from bInput)
    _flagsHelper.clear<Flag::Zero, Flag::Overflow, Flag::Negative>();
    _flagsHelper.set<Flag::Zero>(_alu.getAdderHold() == 0);
    _flagsHelper.set<Flag::Overflow>(_alu.getBInput() & 0x40);
    _flagsHelper.set<Flag::Negative>(_alu.getBInput() & 0x80);
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bitZp0() {
    _currentInstruction = &Chip::bitZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bitZp1() {
    _currentInstruction = &Chip::bit0;
    zeroPageLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bitAbs0() {
    _currentInstruction = &Chip::bitAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bitAbs1() {
    _currentInstruction = &Chip::bitAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::bitAbs2() {
    _currentInstruction = &Chip::bit0;
    absoluteLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::asl(uint8_t data) {
    // ASL by adding same number to itself with no carry
    _alu.performSum<BDecimalSupported, false>(data, data, false, false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslMemory0() {
    _currentInstruction = &Chip::aslMemory1;
    asl(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslImm0() {
    _currentInstruction = &Chip::aslImm1;
    asl(_accumulator);
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslImm1() {
    // Write result back
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslZp0() {
    _currentInstruction = &Chip::aslZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslZp1() {
    _currentInstruction = &Chip::aslMemory0;
    zeroPageLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslZpX0() {
    _currentInstruction = &Chip::aslZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslZpX1() {
    _currentInstruction = &Chip::aslZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslZpX2() {
    _currentInstruction = &Chip::aslMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslAbs0() {
    _currentInstruction = &Chip::aslAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslAbs1() {
    _currentInstruction = &Chip::aslAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslAbs2() {
    _currentInstruction = &Chip::aslMemory0;
    absoluteLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslAbsX0() {
    _currentInstruction = &Chip::aslAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslAbsX1() {
    _currentInstruction = &Chip::aslAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslAbsX2() {
    _currentInstruction = &Chip::aslAbsX3;
    absoluteIndexedLoad0(&Chip::aslAbsX3);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::aslAbsX3() {
    _currentInstruction = &Chip::aslMemory0;
    absoluteIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsr(uint8_t data) {
    // LSR by shift right with no carry
    _alu.performShiftRight(data, false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrMemory0() {
    _currentInstruction = &Chip::lsrMemory1;
    lsr(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrImm0() {
    _currentInstruction = &Chip::lsrImm1;
    lsr(_accumulator);
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrImm1() {
    // Write result back
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrZp0() {
    _currentInstruction = &Chip::lsrZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrZp1() {
    _currentInstruction = &Chip::lsrMemory0;
    zeroPageLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrZpX0() {
    _currentInstruction = &Chip::lsrZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrZpX1() {
    _currentInstruction = &Chip::lsrZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrZpX2() {
    _currentInstruction = &Chip::lsrMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrAbs0() {
    _currentInstruction = &Chip::lsrAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrAbs1() {
    _currentInstruction = &Chip::lsrAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrAbs2() {
    _currentInstruction = &Chip::lsrMemory0;
    absoluteLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrAbsX0() {
    _currentInstruction = &Chip::lsrAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrAbsX1() {
    _currentInstruction = &Chip::lsrAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrAbsX2() {
    _currentInstruction = &Chip::lsrAbsX3;
    absoluteIndexedLoad0(&Chip::lsrAbsX3);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lsrAbsX3() {
    _currentInstruction = &Chip::lsrMemory0;
    absoluteIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rol(uint8_t data) {
    // ROL by adding same number to itself with carry
    _alu.performSum<BDecimalSupported, false>(data, data, false, _flagsHelper.get<Flag::Carry>());
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolMemory0() {
    _currentInstruction = &Chip::rolMemory1;
    rol(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolImm0() {
    _currentInstruction = &Chip::rolImm1;
    rol(_accumulator);
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolImm1() {
    // Write result back
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolZp0() {
    _currentInstruction = &Chip::rolZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolZp1() {
    _currentInstruction = &Chip::rolMemory0;
    zeroPageLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolZpX0() {
    _currentInstruction = &Chip::rolZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolZpX1() {
    _currentInstruction = &Chip::rolZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolZpX2() {
    _currentInstruction = &Chip::rolMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolAbs0() {
    _currentInstruction = &Chip::rolAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolAbs1() {
    _currentInstruction = &Chip::rolAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolAbs2() {
    _currentInstruction = &Chip::rolMemory0;
    absoluteLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolAbsX0() {
    _currentInstruction = &Chip::rolAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolAbsX1() {
    _currentInstruction = &Chip::rolAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolAbsX2() {
    _currentInstruction = &Chip::rolAbsX3;
    absoluteIndexedLoad0(&Chip::rolAbsX3);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rolAbsX3() {
    _currentInstruction = &Chip::rolMemory0;
    absoluteIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ror(uint8_t data) {
    // ROR by shift right with current carry
    _alu.performShiftRight(data, _flagsHelper.get<Flag::Carry>());
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorMemory0() {
    _currentInstruction = &Chip::rorMemory1;
    ror(_inputDataLatch);
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, _inputDataLatch);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorMemory1() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorImm0() {
    _currentInstruction = &Chip::rorImm1;
    ror(_accumulator);
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorImm1() {
    // Write result back
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorZp0() {
    _currentInstruction = &Chip::rorZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorZp1() {
    _currentInstruction = &Chip::rorMemory0;
    zeroPageLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorZpX0() {
    _currentInstruction = &Chip::rorZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorZpX1() {
    _currentInstruction = &Chip::rorZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorZpX2() {
    _currentInstruction = &Chip::rorMemory0;
    zeroPageIndexedLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorAbs0() {
    _currentInstruction = &Chip::rorAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorAbs1() {
    _currentInstruction = &Chip::rorAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorAbs2() {
    _currentInstruction = &Chip::rorMemory0;
    absoluteLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorAbsX0() {
    _currentInstruction = &Chip::rorAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorAbsX1() {
    _currentInstruction = &Chip::rorAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorAbsX2() {
    _currentInstruction = &Chip::rorAbsX3;
    absoluteIndexedLoad0(&Chip::rorAbsX3);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rorAbsX3() {
    _currentInstruction = &Chip::rorMemory0;
    absoluteIndexedLoad1();
}

#endif /* Cpu6502_Internal_Logic_s_hpp */
