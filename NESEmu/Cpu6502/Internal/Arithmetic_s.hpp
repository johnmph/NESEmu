//
//  Arithmetic_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Arithmetic_s_hpp
#define Cpu6502_Internal_Arithmetic_s_hpp


template <class TConfiguration>
void Chip<TConfiguration>::adc0() {
    // Adding data to accumulator with possible carry
    _alu.performSum<DecimalSupported, false>(_accumulator, _inputDataLatch, _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Carry, Flag::Zero, Flag::Overflow, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::adcImm0() {
    _currentInstruction = &Chip::adc0;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcZp0() {
    _currentInstruction = &Chip::adcZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcZp1() {
    _currentInstruction = &Chip::adc0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcZpX0() {
    _currentInstruction = &Chip::adcZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcZpX1() {
    _currentInstruction = &Chip::adcZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcZpX2() {
    _currentInstruction = &Chip::adc0;
    zeroPageIndexedRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcAbs0() {
    _currentInstruction = &Chip::adcAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcAbs1() {
    _currentInstruction = &Chip::adcAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcAbs2() {
    _currentInstruction = &Chip::adc0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcAbsX0() {
    _currentInstruction = &Chip::adcAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcAbsX1() {
    _currentInstruction = &Chip::adcAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::adcAbsX3;
    absoluteIndexedRead0(&Chip::adc0);
}

template <class TConfiguration>
void Chip<TConfiguration>::adcAbsX3() {
    _currentInstruction = &Chip::adc0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcAbsY0() {
    _currentInstruction = &Chip::adcAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcAbsY1() {
    _currentInstruction = &Chip::adcAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::adcAbsY3;
    absoluteIndexedRead0(&Chip::adc0);
}

template <class TConfiguration>
void Chip<TConfiguration>::adcAbsY3() {
    _currentInstruction = &Chip::adc0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcIndX0() {
    _currentInstruction = &Chip::adcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcIndX1() {
    _currentInstruction = &Chip::adcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcIndX2() {
    _currentInstruction = &Chip::adcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcIndX3() {
    _currentInstruction = &Chip::adcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcIndX4() {
    _currentInstruction = &Chip::adc0;
    zeroPagePreIndexedIndirectRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcIndY0() {
    _currentInstruction = &Chip::adcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcIndY1() {
    _currentInstruction = &Chip::adcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcIndY2() {
    _currentInstruction = &Chip::adcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::adcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedRead0 because it can modify it
    _currentInstruction = &Chip::adcIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::adc0);
}

template <class TConfiguration>
void Chip<TConfiguration>::adcIndY4() {
    _currentInstruction = &Chip::adc0;
    zeroPageIndirectPostIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbc0() {
    // Substracting data to accumulator with possible carry by inverting bInput
    _alu.performSum<DecimalSupported, true>(_accumulator, _inputDataLatch, _flagsHelper.get<Flag::DecimalMode>(), _flagsHelper.get<Flag::Carry>());
    
    // Fetch next opcode during performing ALU
    fetchOpcode(&Chip::finishAluForAccumulator<Flag::Carry, Flag::Zero, Flag::Overflow, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcImm0() {
    _currentInstruction = &Chip::sbc0;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcZp0() {
    _currentInstruction = &Chip::sbcZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcZp1() {
    _currentInstruction = &Chip::sbc0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcZpX0() {
    _currentInstruction = &Chip::sbcZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcZpX1() {
    _currentInstruction = &Chip::sbcZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcZpX2() {
    _currentInstruction = &Chip::sbc0;
    zeroPageIndexedRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcAbs0() {
    _currentInstruction = &Chip::sbcAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcAbs1() {
    _currentInstruction = &Chip::sbcAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcAbs2() {
    _currentInstruction = &Chip::sbc0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcAbsX0() {
    _currentInstruction = &Chip::sbcAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcAbsX1() {
    _currentInstruction = &Chip::sbcAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::sbcAbsX3;
    absoluteIndexedRead0(&Chip::sbc0);
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcAbsX3() {
    _currentInstruction = &Chip::sbc0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcAbsY0() {
    _currentInstruction = &Chip::sbcAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcAbsY1() {
    _currentInstruction = &Chip::sbcAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::sbcAbsY3;
    absoluteIndexedRead0(&Chip::sbc0);
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcAbsY3() {
    _currentInstruction = &Chip::sbc0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcIndX0() {
    _currentInstruction = &Chip::sbcIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcIndX1() {
    _currentInstruction = &Chip::sbcIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcIndX2() {
    _currentInstruction = &Chip::sbcIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcIndX3() {
    _currentInstruction = &Chip::sbcIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcIndX4() {
    _currentInstruction = &Chip::sbc0;
    zeroPagePreIndexedIndirectRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcIndY0() {
    _currentInstruction = &Chip::sbcIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcIndY1() {
    _currentInstruction = &Chip::sbcIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcIndY2() {
    _currentInstruction = &Chip::sbcIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedRead0 because it can modify it
    _currentInstruction = &Chip::sbcIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::sbc0);
}

template <class TConfiguration>
void Chip<TConfiguration>::sbcIndY4() {
    _currentInstruction = &Chip::sbc0;
    zeroPageIndirectPostIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cp0(uint8_t data) {
    // Substracting data fetched to data to compare them
    _alu.performSum<DecimalSupported, true>(data, _inputDataLatch, false, true);
    
    // Fetch next opcode during performing ALU (don't save result, just affect flags)
    fetchOpcode(&Chip::finishAlu<Flag::Carry, Flag::Zero, Flag::Negative>);
}

template <class TConfiguration>
void Chip<TConfiguration>::cmp0() {
    cp0(_accumulator);
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpImm0() {
    _currentInstruction = &Chip::cmp0;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpZp0() {
    _currentInstruction = &Chip::cmpZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpZp1() {
    _currentInstruction = &Chip::cmp0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpZpX0() {
    _currentInstruction = &Chip::cmpZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpZpX1() {
    _currentInstruction = &Chip::cmpZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpZpX2() {
    _currentInstruction = &Chip::cmp0;
    zeroPageIndexedRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpAbs0() {
    _currentInstruction = &Chip::cmpAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpAbs1() {
    _currentInstruction = &Chip::cmpAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpAbs2() {
    _currentInstruction = &Chip::cmp0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpAbsX0() {
    _currentInstruction = &Chip::cmpAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpAbsX1() {
    _currentInstruction = &Chip::cmpAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpAbsX2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::cmpAbsX3;
    absoluteIndexedRead0(&Chip::cmp0);
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpAbsX3() {
    _currentInstruction = &Chip::cmp0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpAbsY0() {
    _currentInstruction = &Chip::cmpAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpAbsY1() {
    _currentInstruction = &Chip::cmpAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpAbsY2() {
    // It's important to have currentInstruction set before calling absoluteIndexRead0 because it can modify it
    _currentInstruction = &Chip::cmpAbsY3;
    absoluteIndexedRead0(&Chip::cmp0);
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpAbsY3() {
    _currentInstruction = &Chip::cmp0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpIndX0() {
    _currentInstruction = &Chip::cmpIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpIndX1() {
    _currentInstruction = &Chip::cmpIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpIndX2() {
    _currentInstruction = &Chip::cmpIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpIndX3() {
    _currentInstruction = &Chip::cmpIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpIndX4() {
    _currentInstruction = &Chip::cmp0;
    zeroPagePreIndexedIndirectRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpIndY0() {
    _currentInstruction = &Chip::cmpIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpIndY1() {
    _currentInstruction = &Chip::cmpIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpIndY2() {
    _currentInstruction = &Chip::cmpIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpIndY3() {
    // It's important to have currentInstruction set before calling zeroPageIndirectPostIndexedRead0 because it can modify it
    _currentInstruction = &Chip::cmpIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::cmp0);
}

template <class TConfiguration>
void Chip<TConfiguration>::cmpIndY4() {
    _currentInstruction = &Chip::cmp0;
    zeroPageIndirectPostIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpx0() {
    cp0(_xIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::cpxImm0() {
    _currentInstruction = &Chip::cpx0;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpxZp0() {
    _currentInstruction = &Chip::cpxZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpxZp1() {
    _currentInstruction = &Chip::cpx0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpxAbs0() {
    _currentInstruction = &Chip::cpxAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpxAbs1() {
    _currentInstruction = &Chip::cpxAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpxAbs2() {
    _currentInstruction = &Chip::cpx0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpy0() {
    cp0(_yIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::cpyImm0() {
    _currentInstruction = &Chip::cpy0;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpyZp0() {
    _currentInstruction = &Chip::cpyZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpyZp1() {
    _currentInstruction = &Chip::cpy0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpyAbs0() {
    _currentInstruction = &Chip::cpyAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpyAbs1() {
    _currentInstruction = &Chip::cpyAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::cpyAbs2() {
    _currentInstruction = &Chip::cpy0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::dec(uint8_t data) {
    // Removing 1 from data using ALU (Add 0xFF without carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _alu.performSum<DecimalSupported, false>(0xFF, data, false, false);
}

template <class TConfiguration>
void Chip<TConfiguration>::inc(uint8_t data) {
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502, data is loaded on B register because it comes from DB signal)
    _alu.performSum<DecimalSupported, false>(0x0, data, false, true);
}

template <class TConfiguration>
void Chip<TConfiguration>::decMemory0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::finishAluForMemory<Flag::Zero, Flag::Negative>, _inputDataLatch);
    
    // Decrement
    dec(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::decZp0() {
    _currentInstruction = &Chip::decZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::decZp1() {
    _currentInstruction = &Chip::decMemory0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::decZpX0() {
    _currentInstruction = &Chip::decZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::decZpX1() {
    _currentInstruction = &Chip::decZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::decZpX2() {
    _currentInstruction = &Chip::decMemory0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::decAbs0() {
    _currentInstruction = &Chip::decAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::decAbs1() {
    _currentInstruction = &Chip::decAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::decAbs2() {
    _currentInstruction = &Chip::decMemory0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::decAbsX0() {
    _currentInstruction = &Chip::decAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::decAbsX1() {
    _currentInstruction = &Chip::decAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::decAbsX2() {
    _currentInstruction = &Chip::decAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::decAbsX3() {
    _currentInstruction = &Chip::decMemory0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::incMemory0() {
    // Modify (just write non modified value back)
    anyRMWModify(&Chip::finishAluForMemory<Flag::Zero, Flag::Negative>, _inputDataLatch);
    
    // Increment
    inc(_inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::incZp0() {
    _currentInstruction = &Chip::incZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::incZp1() {
    _currentInstruction = &Chip::incMemory0;
    zeroPageRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::incZpX0() {
    _currentInstruction = &Chip::incZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::incZpX1() {
    _currentInstruction = &Chip::incZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::incZpX2() {
    _currentInstruction = &Chip::incMemory0;
    zeroPageIndexedRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::incAbs0() {
    _currentInstruction = &Chip::incAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::incAbs1() {
    _currentInstruction = &Chip::incAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::incAbs2() {
    _currentInstruction = &Chip::incMemory0;
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::incAbsX0() {
    _currentInstruction = &Chip::incAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::incAbsX1() {
    _currentInstruction = &Chip::incAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::incAbsX2() {
    _currentInstruction = &Chip::incAbsX3;
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::incAbsX3() {
    _currentInstruction = &Chip::incMemory0;
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::incdecRegister2(uint8_t &data) {
    // Write result back
    data = _alu.getAdderHold();
    
    // Finish performing alu
    finishAlu<Flag::Zero, Flag::Negative>();
}

template <class TConfiguration>
void Chip<TConfiguration>::decRegister1(OpcodeInstruction nextInstruction, uint8_t data) {
    dec(data);
    
    // Fetch opcode during performing ALU
    fetchOpcode(nextInstruction);
}

template <class TConfiguration>
void Chip<TConfiguration>::dex0() {
    _currentInstruction = &Chip::dex1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::dex1() {
    decRegister1(&Chip::dex2, _xIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::dex2() {
    incdecRegister2(_xIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::dey0() {
    _currentInstruction = &Chip::dey1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::dey1() {
    decRegister1(&Chip::dey2, _yIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::dey2() {
    incdecRegister2(_yIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::incRegister1(OpcodeInstruction nextInstruction, uint8_t data) {
    inc(data);
    
    // Fetch opcode during performing ALU
    fetchOpcode(nextInstruction);
}

template <class TConfiguration>
void Chip<TConfiguration>::inx0() {
    _currentInstruction = &Chip::inx1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::inx1() {
    incRegister1(&Chip::inx2, _xIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::inx2() {
    incdecRegister2(_xIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::iny0() {
    _currentInstruction = &Chip::iny1;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::iny1() {
    incRegister1(&Chip::iny2, _yIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::iny2() {
    incdecRegister2(_yIndex);
}

#endif /* Cpu6502_Internal_Arithmetic_s_hpp */
