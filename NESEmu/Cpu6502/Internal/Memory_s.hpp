//
//  Memory_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Memory_s_hpp
#define Cpu6502_Internal_Memory_s_hpp


template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ld0(uint8_t &data) {
    data = _inputDataLatch;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(data);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::lda0() {
    ld0(_accumulator);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaImm0() {
    _currentInstruction = &Chip::lda0;
    immediate();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaZp0() {
    _currentInstruction = &Chip::ldaZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaZp1() {
    _currentInstruction = &Chip::lda0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaZpX0() {
    _currentInstruction = &Chip::ldaZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaZpX1() {
    _currentInstruction = &Chip::ldaZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaZpX2() {
    _currentInstruction = &Chip::lda0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaAbs0() {
    _currentInstruction = &Chip::ldaAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaAbs1() {
    _currentInstruction = &Chip::ldaAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaAbs2() {
    _currentInstruction = &Chip::lda0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaAbsX0() {
    _currentInstruction = &Chip::ldaAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaAbsX1() {
    _currentInstruction = &Chip::ldaAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaAbsX2() {
    _currentInstruction = &Chip::ldaAbsX3;
    absoluteIndexedLoad0(&Chip::lda0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaAbsX3() {
    _currentInstruction = &Chip::lda0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaAbsY0() {
    _currentInstruction = &Chip::ldaAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaAbsY1() {
    _currentInstruction = &Chip::ldaAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaAbsY2() {
    _currentInstruction = &Chip::ldaAbsY3;
    absoluteIndexedLoad0(&Chip::lda0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaAbsY3() {
    _currentInstruction = &Chip::lda0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaIndX0() {
    _currentInstruction = &Chip::ldaIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaIndX1() {
    _currentInstruction = &Chip::ldaIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaIndX2() {
    _currentInstruction = &Chip::ldaIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaIndX3() {
    _currentInstruction = &Chip::ldaIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaIndX4() {
    _currentInstruction = &Chip::lda0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaIndY0() {
    _currentInstruction = &Chip::ldaIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaIndY1() {
    _currentInstruction = &Chip::ldaIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaIndY2() {
    _currentInstruction = &Chip::ldaIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaIndY3() {
    _currentInstruction = &Chip::ldaIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::lda0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldaIndY4() {
    _currentInstruction = &Chip::lda0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldx0() {
    ld0(_xIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxImm0() {
    _currentInstruction = &Chip::ldx0;
    immediate();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxZp0() {
    _currentInstruction = &Chip::ldxZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxZp1() {
    _currentInstruction = &Chip::ldx0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxZpY0() {
    _currentInstruction = &Chip::ldxZpY1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxZpY1() {
    _currentInstruction = &Chip::ldxZpY2;
    zeroPageIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxZpY2() {
    _currentInstruction = &Chip::ldx0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxAbs0() {
    _currentInstruction = &Chip::ldxAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxAbs1() {
    _currentInstruction = &Chip::ldxAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxAbs2() {
    _currentInstruction = &Chip::ldx0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxAbsY0() {
    _currentInstruction = &Chip::ldxAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxAbsY1() {
    _currentInstruction = &Chip::ldxAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxAbsY2() {
    _currentInstruction = &Chip::ldxAbsY3;
    absoluteIndexedLoad0(&Chip::ldx0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldxAbsY3() {
    _currentInstruction = &Chip::ldx0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldy0() {
    ld0(_yIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyImm0() {
    _currentInstruction = &Chip::ldy0;
    immediate();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyZp0() {
    _currentInstruction = &Chip::ldyZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyZp1() {
    _currentInstruction = &Chip::ldy0;
    zeroPageLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyZpX0() {
    _currentInstruction = &Chip::ldyZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyZpX1() {
    _currentInstruction = &Chip::ldyZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyZpX2() {
    _currentInstruction = &Chip::ldy0;
    zeroPageIndexedLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyAbs0() {
    _currentInstruction = &Chip::ldyAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyAbs1() {
    _currentInstruction = &Chip::ldyAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyAbs2() {
    _currentInstruction = &Chip::ldy0;
    absoluteLoad();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyAbsX0() {
    _currentInstruction = &Chip::ldyAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyAbsX1() {
    _currentInstruction = &Chip::ldyAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyAbsX2() {
    _currentInstruction = &Chip::ldyAbsX3;
    absoluteIndexedLoad0(&Chip::ldy0);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::ldyAbsX3() {
    _currentInstruction = &Chip::ldy0;
    absoluteIndexedLoad1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staZp0() {
    _currentInstruction = &Chip::staZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageStore(_accumulator);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staZpX0() {
    _currentInstruction = &Chip::staZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staZpX1() {
    _currentInstruction = &Chip::staZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedStore(_accumulator);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staAbs0() {
    _currentInstruction = &Chip::staAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staAbs1() {
    _currentInstruction = &Chip::staAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteStore(_accumulator);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staAbsX0() {
    _currentInstruction = &Chip::staAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staAbsX1() {
    _currentInstruction = &Chip::staAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staAbsX2() {
    _currentInstruction = &Chip::staAbsX3;
    absoluteIndexedStore0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staAbsX3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedStore1(_accumulator);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staAbsY0() {
    _currentInstruction = &Chip::staAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staAbsY1() {
    _currentInstruction = &Chip::staAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staAbsY2() {
    _currentInstruction = &Chip::staAbsY3;
    absoluteIndexedStore0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staAbsY3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedStore1(_accumulator);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staIndX0() {
    _currentInstruction = &Chip::staIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staIndX1() {
    _currentInstruction = &Chip::staIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staIndX2() {
    _currentInstruction = &Chip::staIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staIndX3() {
    _currentInstruction = &Chip::staIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staIndX4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPagePreIndexedIndirectStore(_accumulator);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staIndY0() {
    _currentInstruction = &Chip::staIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staIndY1() {
    _currentInstruction = &Chip::staIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staIndY2() {
    _currentInstruction = &Chip::staIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staIndY3() {
    _currentInstruction = &Chip::staIndY4;
    zeroPageIndirectPostIndexedStore0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::staIndY4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndirectPostIndexedStore1(_accumulator);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::stxZp0() {
    _currentInstruction = &Chip::stxZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::stxZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageStore(_xIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::stxZpY0() {
    _currentInstruction = &Chip::stxZpY1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::stxZpY1() {
    _currentInstruction = &Chip::stxZpY2;
    zeroPageIndexedY1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::stxZpY2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedStore(_xIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::stxAbs0() {
    _currentInstruction = &Chip::stxAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::stxAbs1() {
    _currentInstruction = &Chip::stxAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::stxAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteStore(_xIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::styZp0() {
    _currentInstruction = &Chip::styZp1;
    zeroPage();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::styZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageStore(_yIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::styZpX0() {
    _currentInstruction = &Chip::styZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::styZpX1() {
    _currentInstruction = &Chip::styZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::styZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedStore(_yIndex);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::styAbs0() {
    _currentInstruction = &Chip::styAbs1;
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::styAbs1() {
    _currentInstruction = &Chip::styAbs2;
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::styAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteStore(_yIndex);
}

#endif /* Cpu6502_Internal_Memory_s_hpp */
