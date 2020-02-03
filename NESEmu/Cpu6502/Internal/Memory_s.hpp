//
//  Memory_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Memory_s_hpp
#define Cpu6502_Internal_Memory_s_hpp


template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ld0(uint8_t &data) {
    data = _inputDataLatch;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(data);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::lda0() {
    ld0(_accumulator);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaImm0() {
    _currentInstruction = &Chip::lda0;
    immediate();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaZp0() {
    _currentInstruction = &Chip::ldaZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaZp1() {
    _currentInstruction = &Chip::lda0;
    zeroPageRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaZpX0() {
    _currentInstruction = &Chip::ldaZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaZpX1() {
    _currentInstruction = &Chip::ldaZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaZpX2() {
    _currentInstruction = &Chip::lda0;
    zeroPageIndexedRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaAbs0() {
    _currentInstruction = &Chip::ldaAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaAbs1() {
    _currentInstruction = &Chip::ldaAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaAbs2() {
    _currentInstruction = &Chip::lda0;
    absoluteRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaAbsX0() {
    _currentInstruction = &Chip::ldaAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaAbsX1() {
    _currentInstruction = &Chip::ldaAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaAbsX2() {
    _currentInstruction = &Chip::ldaAbsX3;
    absoluteIndexedRead0(&Chip::lda0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaAbsX3() {
    _currentInstruction = &Chip::lda0;
    absoluteIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaAbsY0() {
    _currentInstruction = &Chip::ldaAbsY1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaAbsY1() {
    _currentInstruction = &Chip::ldaAbsY2;
    absoluteIndexedY1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaAbsY2() {
    _currentInstruction = &Chip::ldaAbsY3;
    absoluteIndexedRead0(&Chip::lda0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaAbsY3() {
    _currentInstruction = &Chip::lda0;
    absoluteIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaIndX0() {
    _currentInstruction = &Chip::ldaIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaIndX1() {
    _currentInstruction = &Chip::ldaIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaIndX2() {
    _currentInstruction = &Chip::ldaIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaIndX3() {
    _currentInstruction = &Chip::ldaIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaIndX4() {
    _currentInstruction = &Chip::lda0;
    zeroPagePreIndexedIndirectRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaIndY0() {
    _currentInstruction = &Chip::ldaIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaIndY1() {
    _currentInstruction = &Chip::ldaIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaIndY2() {
    _currentInstruction = &Chip::ldaIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaIndY3() {
    _currentInstruction = &Chip::ldaIndY4;
    zeroPageIndirectPostIndexedRead0(&Chip::lda0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldaIndY4() {
    _currentInstruction = &Chip::lda0;
    zeroPageIndirectPostIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldx0() {
    ld0(_xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxImm0() {
    _currentInstruction = &Chip::ldx0;
    immediate();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxZp0() {
    _currentInstruction = &Chip::ldxZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxZp1() {
    _currentInstruction = &Chip::ldx0;
    zeroPageRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxZpY0() {
    _currentInstruction = &Chip::ldxZpY1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxZpY1() {
    _currentInstruction = &Chip::ldxZpY2;
    zeroPageIndexedY1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxZpY2() {
    _currentInstruction = &Chip::ldx0;
    zeroPageIndexedRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxAbs0() {
    _currentInstruction = &Chip::ldxAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxAbs1() {
    _currentInstruction = &Chip::ldxAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxAbs2() {
    _currentInstruction = &Chip::ldx0;
    absoluteRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxAbsY0() {
    _currentInstruction = &Chip::ldxAbsY1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxAbsY1() {
    _currentInstruction = &Chip::ldxAbsY2;
    absoluteIndexedY1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxAbsY2() {
    _currentInstruction = &Chip::ldxAbsY3;
    absoluteIndexedRead0(&Chip::ldx0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldxAbsY3() {
    _currentInstruction = &Chip::ldx0;
    absoluteIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldy0() {
    ld0(_yIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyImm0() {
    _currentInstruction = &Chip::ldy0;
    immediate();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyZp0() {
    _currentInstruction = &Chip::ldyZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyZp1() {
    _currentInstruction = &Chip::ldy0;
    zeroPageRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyZpX0() {
    _currentInstruction = &Chip::ldyZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyZpX1() {
    _currentInstruction = &Chip::ldyZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyZpX2() {
    _currentInstruction = &Chip::ldy0;
    zeroPageIndexedRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyAbs0() {
    _currentInstruction = &Chip::ldyAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyAbs1() {
    _currentInstruction = &Chip::ldyAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyAbs2() {
    _currentInstruction = &Chip::ldy0;
    absoluteRead();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyAbsX0() {
    _currentInstruction = &Chip::ldyAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyAbsX1() {
    _currentInstruction = &Chip::ldyAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyAbsX2() {
    _currentInstruction = &Chip::ldyAbsX3;
    absoluteIndexedRead0(&Chip::ldy0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ldyAbsX3() {
    _currentInstruction = &Chip::ldy0;
    absoluteIndexedRead1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staZp0() {
    _currentInstruction = &Chip::staZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageWrite(_accumulator);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staZpX0() {
    _currentInstruction = &Chip::staZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staZpX1() {
    _currentInstruction = &Chip::staZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedWrite(_accumulator);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staAbs0() {
    _currentInstruction = &Chip::staAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staAbs1() {
    _currentInstruction = &Chip::staAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteWrite(_accumulator);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staAbsX0() {
    _currentInstruction = &Chip::staAbsX1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staAbsX1() {
    _currentInstruction = &Chip::staAbsX2;
    absoluteIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staAbsX2() {
    _currentInstruction = &Chip::staAbsX3;
    absoluteIndexedWrite0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staAbsX3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedWrite1(_accumulator);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staAbsY0() {
    _currentInstruction = &Chip::staAbsY1;
    absoluteIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staAbsY1() {
    _currentInstruction = &Chip::staAbsY2;
    absoluteIndexedY1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staAbsY2() {
    _currentInstruction = &Chip::staAbsY3;
    absoluteIndexedWrite0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staAbsY3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedWrite1(_accumulator);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staIndX0() {
    _currentInstruction = &Chip::staIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staIndX1() {
    _currentInstruction = &Chip::staIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staIndX2() {
    _currentInstruction = &Chip::staIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staIndX3() {
    _currentInstruction = &Chip::staIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staIndX4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPagePreIndexedIndirectWrite(_accumulator);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staIndY0() {
    _currentInstruction = &Chip::staIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staIndY1() {
    _currentInstruction = &Chip::staIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staIndY2() {
    _currentInstruction = &Chip::staIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staIndY3() {
    _currentInstruction = &Chip::staIndY4;
    zeroPageIndirectPostIndexedWrite0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::staIndY4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndirectPostIndexedWrite1(_accumulator);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::stxZp0() {
    _currentInstruction = &Chip::stxZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::stxZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageWrite(_xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::stxZpY0() {
    _currentInstruction = &Chip::stxZpY1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::stxZpY1() {
    _currentInstruction = &Chip::stxZpY2;
    zeroPageIndexedY1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::stxZpY2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedWrite(_xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::stxAbs0() {
    _currentInstruction = &Chip::stxAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::stxAbs1() {
    _currentInstruction = &Chip::stxAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::stxAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteWrite(_xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::styZp0() {
    _currentInstruction = &Chip::styZp1;
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::styZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageWrite(_yIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::styZpX0() {
    _currentInstruction = &Chip::styZpX1;
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::styZpX1() {
    _currentInstruction = &Chip::styZpX2;
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::styZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedWrite(_yIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::styAbs0() {
    _currentInstruction = &Chip::styAbs1;
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::styAbs1() {
    _currentInstruction = &Chip::styAbs2;
    absolute1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::styAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteWrite(_yIndex);
}

#endif /* Cpu6502_Internal_Memory_s_hpp */
