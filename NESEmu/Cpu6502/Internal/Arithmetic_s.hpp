//
//  Arithmetic_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Arithmetic_s_hpp
#define Cpu6502_Internal_Arithmetic_s_hpp


template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::arithmetic1() {
    // Store ALU result in accumulator
    _accumulator = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Overflow, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adc0() {
    // Adding data to accumulator with possible carry
    _alu.performSum<BDecimalSupported, false>(_accumulator, _inputDataLatch, _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::arithmetic1);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcImm0() {
    _currentInstruction = &Chip::adc0;
    immediate();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcZp0() {
    _currentInstruction = &Chip::adcZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcZp1() {
    _currentInstruction = &Chip::adc0;
    zeroPageRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcZpX0() {
    _currentInstruction = &Chip::adcZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcZpX1() {
    _currentInstruction = &Chip::adcZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcZpX2() {
    _currentInstruction = &Chip::adc0;
    zeroPageIndexedRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcAbs0() {
    _currentInstruction = &Chip::adcAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcAbs1() {
    _currentInstruction = &Chip::adcAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcAbs2() {
    _currentInstruction = &Chip::adc0;
    absoluteRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcAbsX0() {
    _currentInstruction = &Chip::adcAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcAbsX1() {
    _currentInstruction = &Chip::adcAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::adcAbsX3;
    absoluteIndexedRead0(&Chip::adc0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcAbsX3() {
    _currentInstruction = &Chip::adc0;
    absoluteIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcAbsY0() {
    _currentInstruction = &Chip::adcAbsY1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcAbsY1() {
    _currentInstruction = &Chip::adcAbsY2;
    absoluteIndexedY1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::adcAbsY3;
    absoluteIndexedRead0(&Chip::adc0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcAbsY3() {
    _currentInstruction = &Chip::adc0;
    absoluteIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcIndX0() {
    _currentInstruction = &Chip::adcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcIndX1() {
    _currentInstruction = &Chip::adcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcIndX2() {
    _currentInstruction = &Chip::adcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcIndX3() {
    _currentInstruction = &Chip::adcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcIndX4() {
    _currentInstruction = &Chip::adc0;
    zeroPagePreIndexedIndirectRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcIndY0() {
    _currentInstruction = &Chip::adcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcIndY1() {
    _currentInstruction = &Chip::adcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcIndY2() {
    _currentInstruction = &Chip::adcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedRead0 because it can modify it
    _currentInstruction = &Chip::adcIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::adc0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::adcIndY4() {
    _currentInstruction = &Chip::adc0;
    zeroPageIndirectPostIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbc0() {
    // Substracting data to accumulator with possible carry by inverting bInput
    _alu.performSum<BDecimalSupported, true>(_accumulator, _inputDataLatch, _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::arithmetic1);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcImm0() {
    _currentInstruction = &Chip::sbc0;
    immediate();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcZp0() {
    _currentInstruction = &Chip::sbcZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcZp1() {
    _currentInstruction = &Chip::sbc0;
    zeroPageRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcZpX0() {
    _currentInstruction = &Chip::sbcZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcZpX1() {
    _currentInstruction = &Chip::sbcZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcZpX2() {
    _currentInstruction = &Chip::sbc0;
    zeroPageIndexedRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcAbs0() {
    _currentInstruction = &Chip::sbcAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcAbs1() {
    _currentInstruction = &Chip::sbcAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcAbs2() {
    _currentInstruction = &Chip::sbc0;
    absoluteRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcAbsX0() {
    _currentInstruction = &Chip::sbcAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcAbsX1() {
    _currentInstruction = &Chip::sbcAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::sbcAbsX3;
    absoluteIndexedRead0(&Chip::sbc0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcAbsX3() {
    _currentInstruction = &Chip::sbc0;
    absoluteIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcAbsY0() {
    _currentInstruction = &Chip::sbcAbsY1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcAbsY1() {
    _currentInstruction = &Chip::sbcAbsY2;
    absoluteIndexedY1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::sbcAbsY3;
    absoluteIndexedRead0(&Chip::sbc0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcAbsY3() {
    _currentInstruction = &Chip::sbc0;
    absoluteIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcIndX0() {
    _currentInstruction = &Chip::sbcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcIndX1() {
    _currentInstruction = &Chip::sbcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcIndX2() {
    _currentInstruction = &Chip::sbcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcIndX3() {
    _currentInstruction = &Chip::sbcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcIndX4() {
    _currentInstruction = &Chip::sbc0;
    zeroPagePreIndexedIndirectRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcIndY0() {
    _currentInstruction = &Chip::sbcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcIndY1() {
    _currentInstruction = &Chip::sbcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcIndY2() {
    _currentInstruction = &Chip::sbcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedRead0 because it can modify it
    _currentInstruction = &Chip::sbcIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::sbc0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::sbcIndY4() {
    _currentInstruction = &Chip::sbc0;
    zeroPageIndirectPostIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cp0(uint8_t data) {
    // Substracting data fetched to data to compare them
    _alu.performSum<BDecimalSupported, true>(data, _inputDataLatch, false, true);
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::cp1);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cp1() {
    // Don't save result, it's just to set the flags
    
    // Update status
    _flagsHelper.refresh<Flag::Carry, Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmp0() {
    cp0(_accumulator);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpImm0() {
    _currentInstruction = &Chip::cmp0;
    immediate();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpZp0() {
    _currentInstruction = &Chip::cmpZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpZp1() {
    _currentInstruction = &Chip::cmp0;
    zeroPageRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpZpX0() {
    _currentInstruction = &Chip::cmpZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpZpX1() {
    _currentInstruction = &Chip::cmpZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpZpX2() {
    _currentInstruction = &Chip::cmp0;
    zeroPageIndexedRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpAbs0() {
    _currentInstruction = &Chip::cmpAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpAbs1() {
    _currentInstruction = &Chip::cmpAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpAbs2() {
    _currentInstruction = &Chip::cmp0;
    absoluteRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpAbsX0() {
    _currentInstruction = &Chip::cmpAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpAbsX1() {
    _currentInstruction = &Chip::cmpAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::cmpAbsX3;
    absoluteIndexedRead0(&Chip::cmp0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpAbsX3() {
    _currentInstruction = &Chip::cmp0;
    absoluteIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpAbsY0() {
    _currentInstruction = &Chip::cmpAbsY1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpAbsY1() {
    _currentInstruction = &Chip::cmpAbsY2;
    absoluteIndexedY1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::cmpAbsY3;
    absoluteIndexedRead0(&Chip::cmp0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpAbsY3() {
    _currentInstruction = &Chip::cmp0;
    absoluteIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpIndX0() {
    _currentInstruction = &Chip::cmpIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpIndX1() {
    _currentInstruction = &Chip::cmpIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpIndX2() {
    _currentInstruction = &Chip::cmpIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpIndX3() {
    _currentInstruction = &Chip::cmpIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpIndX4() {
    _currentInstruction = &Chip::cmp0;
    zeroPagePreIndexedIndirectRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpIndY0() {
    _currentInstruction = &Chip::cmpIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpIndY1() {
    _currentInstruction = &Chip::cmpIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpIndY2() {
    _currentInstruction = &Chip::cmpIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedRead0 because it can modify it
    _currentInstruction = &Chip::cmpIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::cmp0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cmpIndY4() {
    _currentInstruction = &Chip::cmp0;
    zeroPageIndirectPostIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpx0() {
    cp0(_xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpxImm0() {
    _currentInstruction = &Chip::cpx0;
    immediate();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpxZp0() {
    _currentInstruction = &Chip::cpxZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpxZp1() {
    _currentInstruction = &Chip::cpx0;
    zeroPageRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpxAbs0() {
    _currentInstruction = &Chip::cpxAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpxAbs1() {
    _currentInstruction = &Chip::cpxAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpxAbs2() {
    _currentInstruction = &Chip::cpx0;
    absoluteRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpy0() {
    cp0(_yIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpyImm0() {
    _currentInstruction = &Chip::cpy0;
    immediate();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpyZp0() {
    _currentInstruction = &Chip::cpyZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpyZp1() {
    _currentInstruction = &Chip::cpy0;
    zeroPageRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpyAbs0() {
    _currentInstruction = &Chip::cpyAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpyAbs1() {
    _currentInstruction = &Chip::cpyAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::cpyAbs2() {
    _currentInstruction = &Chip::cpy0;
    absoluteRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::dec(uint8_t data) {
    // Removing 1 from data using ALU (Add 0xFF without carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _alu.performSum<BDecimalSupported, false>(0xFF, data, false, false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::inc(uint8_t data) {
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _alu.performSum<BDecimalSupported, false>(0x0, data, false, true);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incdecMemory1() {
    // Write result back
    anyRMWWrite(_alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_alu.getAdderHold());
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decMemory0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::incdecMemory1, _inputDataLatch);
    
    // Decrement
    dec(_inputDataLatch);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decZp0() {
    _currentInstruction = &Chip::decZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decZp1() {
    _currentInstruction = &Chip::decMemory0;
    zeroPageRMWRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decZpX0() {
    _currentInstruction = &Chip::decZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decZpX1() {
    _currentInstruction = &Chip::decZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decZpX2() {
    _currentInstruction = &Chip::decMemory0;
    zeroPageIndexedRMWRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decAbs0() {
    _currentInstruction = &Chip::decAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decAbs1() {
    _currentInstruction = &Chip::decAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decAbs2() {
    _currentInstruction = &Chip::decMemory0;
    absoluteRMWRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decAbsX0() {
    _currentInstruction = &Chip::decAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decAbsX1() {
    _currentInstruction = &Chip::decAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decAbsX2() {
    _currentInstruction = &Chip::decAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decAbsX3() {
    _currentInstruction = &Chip::decMemory0;
    absoluteIndexedRMWRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incMemory0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::incdecMemory1, _inputDataLatch);
    
    // Increment
    inc(_inputDataLatch);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incZp0() {
    _currentInstruction = &Chip::incZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incZp1() {
    _currentInstruction = &Chip::incMemory0;
    zeroPageRMWRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incZpX0() {
    _currentInstruction = &Chip::incZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incZpX1() {
    _currentInstruction = &Chip::incZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incZpX2() {
    _currentInstruction = &Chip::incMemory0;
    zeroPageIndexedRMWRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incAbs0() {
    _currentInstruction = &Chip::incAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incAbs1() {
    _currentInstruction = &Chip::incAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incAbs2() {
    _currentInstruction = &Chip::incMemory0;
    absoluteRMWRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incAbsX0() {
    _currentInstruction = &Chip::incAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incAbsX1() {
    _currentInstruction = &Chip::incAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incAbsX2() {
    _currentInstruction = &Chip::incAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incAbsX3() {
    _currentInstruction = &Chip::incMemory0;
    absoluteIndexedRMWRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incdecRegister2(uint8_t &data) {
    // Write result back
    data = _alu.getAdderHold();
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decRegister1(OpcodeInstruction nextInstruction, uint8_t data) {
    dec(data);
    
    // Fetch opcode during performing ALU
    fetchOpcode(nextInstruction);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::dex0() {
    _currentInstruction = &Chip::dex1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::dex1() {
    decRegister1(&Chip::dex2, _xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::dex2() {
    incdecRegister2(_xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::dey0() {
    _currentInstruction = &Chip::dey1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::dey1() {
    decRegister1(&Chip::dey2, _yIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::dey2() {
    incdecRegister2(_yIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incRegister1(OpcodeInstruction nextInstruction, uint8_t data) {
    inc(data);
    
    // Fetch opcode during performing ALU
    fetchOpcode(nextInstruction);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::inx0() {
    _currentInstruction = &Chip::inx1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::inx1() {
    incRegister1(&Chip::inx2, _xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::inx2() {
    incdecRegister2(_xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::iny0() {
    _currentInstruction = &Chip::iny1;
    implied();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::iny1() {
    incRegister1(&Chip::iny2, _yIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::iny2() {
    incdecRegister2(_yIndex);
}

#endif /* Cpu6502_Internal_Arithmetic_s_hpp */
