//
//  Memory_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Memory_s_hpp
#define Cpu6502_Internal_Memory_s_hpp


template <class TConfiguration>
void Chip<TConfiguration>::ld0(uint8_t &data) {
    data = _inputDataLatch;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(data);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TConfiguration>
void Chip<TConfiguration>::lda0() {
    ld0(_accumulator);
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaImm0() {
    _currentInstruction = &Chip::lda0;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaZp0() {
    _currentInstruction = &Chip::ldaZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaZp1() {
    _currentInstruction = &Chip::lda0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaZpX0() {
    _currentInstruction = &Chip::ldaZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaZpX1() {
    _currentInstruction = &Chip::ldaZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaZpX2() {
    _currentInstruction = &Chip::lda0;
    zeroPageIndexedRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaAbs0() {
    _currentInstruction = &Chip::ldaAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaAbs1() {
    _currentInstruction = &Chip::ldaAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaAbs2() {
    _currentInstruction = &Chip::lda0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaAbsX0() {
    _currentInstruction = &Chip::ldaAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaAbsX1() {
    _currentInstruction = &Chip::ldaAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaAbsX2() {
    _currentInstruction = &Chip::ldaAbsX3;
    absoluteIndexedRead0(&Chip::lda0);
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaAbsX3() {
    _currentInstruction = &Chip::lda0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaAbsY0() {
    _currentInstruction = &Chip::ldaAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaAbsY1() {
    _currentInstruction = &Chip::ldaAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaAbsY2() {
    _currentInstruction = &Chip::ldaAbsY3;
    absoluteIndexedRead0(&Chip::lda0);
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaAbsY3() {
    _currentInstruction = &Chip::lda0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaIndX0() {
    _currentInstruction = &Chip::ldaIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaIndX1() {
    _currentInstruction = &Chip::ldaIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaIndX2() {
    _currentInstruction = &Chip::ldaIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaIndX3() {
    _currentInstruction = &Chip::ldaIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaIndX4() {
    _currentInstruction = &Chip::lda0;
    zeroPagePreIndexedIndirectRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaIndY0() {
    _currentInstruction = &Chip::ldaIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaIndY1() {
    _currentInstruction = &Chip::ldaIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaIndY2() {
    _currentInstruction = &Chip::ldaIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaIndY3() {
    _currentInstruction = &Chip::ldaIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::lda0);
}

template <class TConfiguration>
void Chip<TConfiguration>::ldaIndY4() {
    _currentInstruction = &Chip::lda0;
    zeroPageIndirectPostIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldx0() {
    ld0(_xIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxImm0() {
    _currentInstruction = &Chip::ldx0;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxZp0() {
    _currentInstruction = &Chip::ldxZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxZp1() {
    _currentInstruction = &Chip::ldx0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxZpY0() {
    _currentInstruction = &Chip::ldxZpY1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxZpY1() {
    _currentInstruction = &Chip::ldxZpY2;
    zeroPageIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxZpY2() {
    _currentInstruction = &Chip::ldx0;
    zeroPageIndexedRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxAbs0() {
    _currentInstruction = &Chip::ldxAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxAbs1() {
    _currentInstruction = &Chip::ldxAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxAbs2() {
    _currentInstruction = &Chip::ldx0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxAbsY0() {
    _currentInstruction = &Chip::ldxAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxAbsY1() {
    _currentInstruction = &Chip::ldxAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxAbsY2() {
    _currentInstruction = &Chip::ldxAbsY3;
    absoluteIndexedRead0(&Chip::ldx0);
}

template <class TConfiguration>
void Chip<TConfiguration>::ldxAbsY3() {
    _currentInstruction = &Chip::ldx0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldy0() {
    ld0(_yIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyImm0() {
    _currentInstruction = &Chip::ldy0;
    immediate();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyZp0() {
    _currentInstruction = &Chip::ldyZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyZp1() {
    _currentInstruction = &Chip::ldy0;
    zeroPageRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyZpX0() {
    _currentInstruction = &Chip::ldyZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyZpX1() {
    _currentInstruction = &Chip::ldyZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyZpX2() {
    _currentInstruction = &Chip::ldy0;
    zeroPageIndexedRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyAbs0() {
    _currentInstruction = &Chip::ldyAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyAbs1() {
    _currentInstruction = &Chip::ldyAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyAbs2() {
    _currentInstruction = &Chip::ldy0;
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyAbsX0() {
    _currentInstruction = &Chip::ldyAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyAbsX1() {
    _currentInstruction = &Chip::ldyAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyAbsX2() {
    _currentInstruction = &Chip::ldyAbsX3;
    absoluteIndexedRead0(&Chip::ldy0);
}

template <class TConfiguration>
void Chip<TConfiguration>::ldyAbsX3() {
    _currentInstruction = &Chip::ldy0;
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::staZp0() {
    _currentInstruction = &Chip::staZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::staZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageWrite(_accumulator);
}

template <class TConfiguration>
void Chip<TConfiguration>::staZpX0() {
    _currentInstruction = &Chip::staZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::staZpX1() {
    _currentInstruction = &Chip::staZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::staZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedWrite(_accumulator);
}

template <class TConfiguration>
void Chip<TConfiguration>::staAbs0() {
    _currentInstruction = &Chip::staAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::staAbs1() {
    _currentInstruction = &Chip::staAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::staAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteWrite(_accumulator);
}

template <class TConfiguration>
void Chip<TConfiguration>::staAbsX0() {
    _currentInstruction = &Chip::staAbsX1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::staAbsX1() {
    _currentInstruction = &Chip::staAbsX2;
    absoluteIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::staAbsX2() {
    _currentInstruction = &Chip::staAbsX3;
    absoluteIndexedWrite0();
}

template <class TConfiguration>
void Chip<TConfiguration>::staAbsX3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedWrite1(_accumulator);
}

template <class TConfiguration>
void Chip<TConfiguration>::staAbsY0() {
    _currentInstruction = &Chip::staAbsY1;
    absoluteIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::staAbsY1() {
    _currentInstruction = &Chip::staAbsY2;
    absoluteIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::staAbsY2() {
    _currentInstruction = &Chip::staAbsY3;
    absoluteIndexedWrite0();
}

template <class TConfiguration>
void Chip<TConfiguration>::staAbsY3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedWrite1(_accumulator);
}

template <class TConfiguration>
void Chip<TConfiguration>::staIndX0() {
    _currentInstruction = &Chip::staIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TConfiguration>
void Chip<TConfiguration>::staIndX1() {
    _currentInstruction = &Chip::staIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TConfiguration>
void Chip<TConfiguration>::staIndX2() {
    _currentInstruction = &Chip::staIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TConfiguration>
void Chip<TConfiguration>::staIndX3() {
    _currentInstruction = &Chip::staIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TConfiguration>
void Chip<TConfiguration>::staIndX4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPagePreIndexedIndirectWrite(_accumulator);
}

template <class TConfiguration>
void Chip<TConfiguration>::staIndY0() {
    _currentInstruction = &Chip::staIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::staIndY1() {
    _currentInstruction = &Chip::staIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TConfiguration>
void Chip<TConfiguration>::staIndY2() {
    _currentInstruction = &Chip::staIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TConfiguration>
void Chip<TConfiguration>::staIndY3() {
    _currentInstruction = &Chip::staIndY4;
    zeroPageIndirectPostIndexedWrite0();
}

template <class TConfiguration>
void Chip<TConfiguration>::staIndY4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndirectPostIndexedWrite1(_accumulator);
}

template <class TConfiguration>
void Chip<TConfiguration>::stxZp0() {
    _currentInstruction = &Chip::stxZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::stxZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageWrite(_xIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::stxZpY0() {
    _currentInstruction = &Chip::stxZpY1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::stxZpY1() {
    _currentInstruction = &Chip::stxZpY2;
    zeroPageIndexedY1();
}

template <class TConfiguration>
void Chip<TConfiguration>::stxZpY2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedWrite(_xIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::stxAbs0() {
    _currentInstruction = &Chip::stxAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::stxAbs1() {
    _currentInstruction = &Chip::stxAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::stxAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteWrite(_xIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::styZp0() {
    _currentInstruction = &Chip::styZp1;
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::styZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageWrite(_yIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::styZpX0() {
    _currentInstruction = &Chip::styZpX1;
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::styZpX1() {
    _currentInstruction = &Chip::styZpX2;
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::styZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedWrite(_yIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::styAbs0() {
    _currentInstruction = &Chip::styAbs1;
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::styAbs1() {
    _currentInstruction = &Chip::styAbs2;
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::styAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteWrite(_yIndex);
}

#endif /* Cpu6502_Internal_Memory_s_hpp */
