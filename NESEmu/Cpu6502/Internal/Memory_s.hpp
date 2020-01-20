//
//  Memory_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Memory_s_hpp
#define Cpu6502_Internal_Memory_s_hpp


template <class TBus>
void Chip<TBus>::ld0(uint8_t &data) {
    data = _inputDataLatch;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(data);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus>
void Chip<TBus>::lda0() {
    ld0(_accumulator);
}

template <class TBus>
void Chip<TBus>::ldaImm0() {
    _currentInstruction = &Chip::lda0;
    immediate();
}

template <class TBus>
void Chip<TBus>::ldaZp0() {
    _currentInstruction = &Chip::ldaZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::ldaZp1() {
    _currentInstruction = &Chip::lda0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::ldaZpX0() {
    _currentInstruction = &Chip::ldaZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::ldaZpX1() {
    _currentInstruction = &Chip::ldaZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::ldaZpX2() {
    _currentInstruction = &Chip::lda0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::ldaAbs0() {
    _currentInstruction = &Chip::ldaAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::ldaAbs1() {
    _currentInstruction = &Chip::ldaAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::ldaAbs2() {
    _currentInstruction = &Chip::lda0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::ldaAbsX0() {
    _currentInstruction = &Chip::ldaAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::ldaAbsX1() {
    _currentInstruction = &Chip::ldaAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::ldaAbsX2() {
    _currentInstruction = &Chip::ldaAbsX3;
    absoluteIndexedLoad0(&Chip::lda0);
}

template <class TBus>
void Chip<TBus>::ldaAbsX3() {
    _currentInstruction = &Chip::lda0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::ldaAbsY0() {
    _currentInstruction = &Chip::ldaAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::ldaAbsY1() {
    _currentInstruction = &Chip::ldaAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::ldaAbsY2() {
    _currentInstruction = &Chip::ldaAbsY3;
    absoluteIndexedLoad0(&Chip::lda0);
}

template <class TBus>
void Chip<TBus>::ldaAbsY3() {
    _currentInstruction = &Chip::lda0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::ldaIndX0() {
    _currentInstruction = &Chip::ldaIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::ldaIndX1() {
    _currentInstruction = &Chip::ldaIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::ldaIndX2() {
    _currentInstruction = &Chip::ldaIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::ldaIndX3() {
    _currentInstruction = &Chip::ldaIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::ldaIndX4() {
    _currentInstruction = &Chip::lda0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Chip<TBus>::ldaIndY0() {
    _currentInstruction = &Chip::ldaIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::ldaIndY1() {
    _currentInstruction = &Chip::ldaIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::ldaIndY2() {
    _currentInstruction = &Chip::ldaIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::ldaIndY3() {
    _currentInstruction = &Chip::ldaIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::lda0);
}

template <class TBus>
void Chip<TBus>::ldaIndY4() {
    _currentInstruction = &Chip::lda0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::ldx0() {
    ld0(_xIndex);
}

template <class TBus>
void Chip<TBus>::ldxImm0() {
    _currentInstruction = &Chip::ldx0;
    immediate();
}

template <class TBus>
void Chip<TBus>::ldxZp0() {
    _currentInstruction = &Chip::ldxZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::ldxZp1() {
    _currentInstruction = &Chip::ldx0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::ldxZpY0() {
    _currentInstruction = &Chip::ldxZpY1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::ldxZpY1() {
    _currentInstruction = &Chip::ldxZpY2;
    zeroPageIndexedY1();
}

template <class TBus>
void Chip<TBus>::ldxZpY2() {
    _currentInstruction = &Chip::ldx0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::ldxAbs0() {
    _currentInstruction = &Chip::ldxAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::ldxAbs1() {
    _currentInstruction = &Chip::ldxAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::ldxAbs2() {
    _currentInstruction = &Chip::ldx0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::ldxAbsY0() {
    _currentInstruction = &Chip::ldxAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::ldxAbsY1() {
    _currentInstruction = &Chip::ldxAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::ldxAbsY2() {
    _currentInstruction = &Chip::ldxAbsY3;
    absoluteIndexedLoad0(&Chip::ldx0);
}

template <class TBus>
void Chip<TBus>::ldxAbsY3() {
    _currentInstruction = &Chip::ldx0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::ldy0() {
    ld0(_yIndex);
}

template <class TBus>
void Chip<TBus>::ldyImm0() {
    _currentInstruction = &Chip::ldy0;
    immediate();
}

template <class TBus>
void Chip<TBus>::ldyZp0() {
    _currentInstruction = &Chip::ldyZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::ldyZp1() {
    _currentInstruction = &Chip::ldy0;
    zeroPageLoad();
}

template <class TBus>
void Chip<TBus>::ldyZpX0() {
    _currentInstruction = &Chip::ldyZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::ldyZpX1() {
    _currentInstruction = &Chip::ldyZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::ldyZpX2() {
    _currentInstruction = &Chip::ldy0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Chip<TBus>::ldyAbs0() {
    _currentInstruction = &Chip::ldyAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::ldyAbs1() {
    _currentInstruction = &Chip::ldyAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::ldyAbs2() {
    _currentInstruction = &Chip::ldy0;
    absoluteLoad();
}

template <class TBus>
void Chip<TBus>::ldyAbsX0() {
    _currentInstruction = &Chip::ldyAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::ldyAbsX1() {
    _currentInstruction = &Chip::ldyAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::ldyAbsX2() {
    _currentInstruction = &Chip::ldyAbsX3;
    absoluteIndexedLoad0(&Chip::ldy0);
}

template <class TBus>
void Chip<TBus>::ldyAbsX3() {
    _currentInstruction = &Chip::ldy0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Chip<TBus>::staZp0() {
    _currentInstruction = &Chip::staZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::staZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageStore(_accumulator);
}

template <class TBus>
void Chip<TBus>::staZpX0() {
    _currentInstruction = &Chip::staZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::staZpX1() {
    _currentInstruction = &Chip::staZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::staZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedStore(_accumulator);
}

template <class TBus>
void Chip<TBus>::staAbs0() {
    _currentInstruction = &Chip::staAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::staAbs1() {
    _currentInstruction = &Chip::staAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::staAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteStore(_accumulator);
}

template <class TBus>
void Chip<TBus>::staAbsX0() {
    _currentInstruction = &Chip::staAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::staAbsX1() {
    _currentInstruction = &Chip::staAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Chip<TBus>::staAbsX2() {
    _currentInstruction = &Chip::staAbsX3;
    absoluteIndexedStore0();
}

template <class TBus>
void Chip<TBus>::staAbsX3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedStore1(_accumulator);
}

template <class TBus>
void Chip<TBus>::staAbsY0() {
    _currentInstruction = &Chip::staAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Chip<TBus>::staAbsY1() {
    _currentInstruction = &Chip::staAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Chip<TBus>::staAbsY2() {
    _currentInstruction = &Chip::staAbsY3;
    absoluteIndexedStore0();
}

template <class TBus>
void Chip<TBus>::staAbsY3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedStore1(_accumulator);
}

template <class TBus>
void Chip<TBus>::staIndX0() {
    _currentInstruction = &Chip::staIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Chip<TBus>::staIndX1() {
    _currentInstruction = &Chip::staIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Chip<TBus>::staIndX2() {
    _currentInstruction = &Chip::staIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Chip<TBus>::staIndX3() {
    _currentInstruction = &Chip::staIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Chip<TBus>::staIndX4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPagePreIndexedIndirectStore(_accumulator);
}

template <class TBus>
void Chip<TBus>::staIndY0() {
    _currentInstruction = &Chip::staIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Chip<TBus>::staIndY1() {
    _currentInstruction = &Chip::staIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Chip<TBus>::staIndY2() {
    _currentInstruction = &Chip::staIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Chip<TBus>::staIndY3() {
    _currentInstruction = &Chip::staIndY4;
    zeroPageIndirectPostIndexedStore0();
}

template <class TBus>
void Chip<TBus>::staIndY4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndirectPostIndexedStore1(_accumulator);
}

template <class TBus>
void Chip<TBus>::stxZp0() {
    _currentInstruction = &Chip::stxZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::stxZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageStore(_xIndex);
}

template <class TBus>
void Chip<TBus>::stxZpY0() {
    _currentInstruction = &Chip::stxZpY1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::stxZpY1() {
    _currentInstruction = &Chip::stxZpY2;
    zeroPageIndexedY1();
}

template <class TBus>
void Chip<TBus>::stxZpY2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedStore(_xIndex);
}

template <class TBus>
void Chip<TBus>::stxAbs0() {
    _currentInstruction = &Chip::stxAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::stxAbs1() {
    _currentInstruction = &Chip::stxAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::stxAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteStore(_xIndex);
}

template <class TBus>
void Chip<TBus>::styZp0() {
    _currentInstruction = &Chip::styZp1;
    zeroPage();
}

template <class TBus>
void Chip<TBus>::styZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageStore(_yIndex);
}

template <class TBus>
void Chip<TBus>::styZpX0() {
    _currentInstruction = &Chip::styZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Chip<TBus>::styZpX1() {
    _currentInstruction = &Chip::styZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Chip<TBus>::styZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedStore(_yIndex);
}

template <class TBus>
void Chip<TBus>::styAbs0() {
    _currentInstruction = &Chip::styAbs1;
    absolute0();
}

template <class TBus>
void Chip<TBus>::styAbs1() {
    _currentInstruction = &Chip::styAbs2;
    absolute1();
}

template <class TBus>
void Chip<TBus>::styAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteStore(_yIndex);
}

#endif /* Cpu6502_Internal_Memory_s_hpp */
