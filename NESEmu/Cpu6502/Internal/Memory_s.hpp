//
//  Memory_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Memory_s_hpp
#define Cpu6502_Internal_Memory_s_hpp


template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ld0(uint8_t &data) {
    data = _inputDataLatch;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(data);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::lda0() {
    ld0(_accumulator);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaImm0() {
    _currentInstruction = &Chip::lda0;
    immediate();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaZp0() {
    _currentInstruction = &Chip::ldaZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaZp1() {
    _currentInstruction = &Chip::lda0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaZpX0() {
    _currentInstruction = &Chip::ldaZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaZpX1() {
    _currentInstruction = &Chip::ldaZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaZpX2() {
    _currentInstruction = &Chip::lda0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaAbs0() {
    _currentInstruction = &Chip::ldaAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaAbs1() {
    _currentInstruction = &Chip::ldaAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaAbs2() {
    _currentInstruction = &Chip::lda0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaAbsX0() {
    _currentInstruction = &Chip::ldaAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaAbsX1() {
    _currentInstruction = &Chip::ldaAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaAbsX2() {
    _currentInstruction = &Chip::ldaAbsX3;
    absoluteIndexedLoad0(&Chip::lda0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaAbsX3() {
    _currentInstruction = &Chip::lda0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaAbsY0() {
    _currentInstruction = &Chip::ldaAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaAbsY1() {
    _currentInstruction = &Chip::ldaAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaAbsY2() {
    _currentInstruction = &Chip::ldaAbsY3;
    absoluteIndexedLoad0(&Chip::lda0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaAbsY3() {
    _currentInstruction = &Chip::lda0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaIndX0() {
    _currentInstruction = &Chip::ldaIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaIndX1() {
    _currentInstruction = &Chip::ldaIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaIndX2() {
    _currentInstruction = &Chip::ldaIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaIndX3() {
    _currentInstruction = &Chip::ldaIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaIndX4() {
    _currentInstruction = &Chip::lda0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaIndY0() {
    _currentInstruction = &Chip::ldaIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaIndY1() {
    _currentInstruction = &Chip::ldaIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaIndY2() {
    _currentInstruction = &Chip::ldaIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaIndY3() {
    _currentInstruction = &Chip::ldaIndY4;
    zeroPageIndirectPostIndexedLoad0(&Chip::lda0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldaIndY4() {
    _currentInstruction = &Chip::lda0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldx0() {
    ld0(_xIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxImm0() {
    _currentInstruction = &Chip::ldx0;
    immediate();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxZp0() {
    _currentInstruction = &Chip::ldxZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxZp1() {
    _currentInstruction = &Chip::ldx0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxZpY0() {
    _currentInstruction = &Chip::ldxZpY1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxZpY1() {
    _currentInstruction = &Chip::ldxZpY2;
    zeroPageIndexedY1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxZpY2() {
    _currentInstruction = &Chip::ldx0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxAbs0() {
    _currentInstruction = &Chip::ldxAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxAbs1() {
    _currentInstruction = &Chip::ldxAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxAbs2() {
    _currentInstruction = &Chip::ldx0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxAbsY0() {
    _currentInstruction = &Chip::ldxAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxAbsY1() {
    _currentInstruction = &Chip::ldxAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxAbsY2() {
    _currentInstruction = &Chip::ldxAbsY3;
    absoluteIndexedLoad0(&Chip::ldx0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldxAbsY3() {
    _currentInstruction = &Chip::ldx0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldy0() {
    ld0(_yIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyImm0() {
    _currentInstruction = &Chip::ldy0;
    immediate();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyZp0() {
    _currentInstruction = &Chip::ldyZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyZp1() {
    _currentInstruction = &Chip::ldy0;
    zeroPageLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyZpX0() {
    _currentInstruction = &Chip::ldyZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyZpX1() {
    _currentInstruction = &Chip::ldyZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyZpX2() {
    _currentInstruction = &Chip::ldy0;
    zeroPageIndexedLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyAbs0() {
    _currentInstruction = &Chip::ldyAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyAbs1() {
    _currentInstruction = &Chip::ldyAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyAbs2() {
    _currentInstruction = &Chip::ldy0;
    absoluteLoad();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyAbsX0() {
    _currentInstruction = &Chip::ldyAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyAbsX1() {
    _currentInstruction = &Chip::ldyAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyAbsX2() {
    _currentInstruction = &Chip::ldyAbsX3;
    absoluteIndexedLoad0(&Chip::ldy0);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::ldyAbsX3() {
    _currentInstruction = &Chip::ldy0;
    absoluteIndexedLoad1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staZp0() {
    _currentInstruction = &Chip::staZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageStore(_accumulator);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staZpX0() {
    _currentInstruction = &Chip::staZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staZpX1() {
    _currentInstruction = &Chip::staZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedStore(_accumulator);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staAbs0() {
    _currentInstruction = &Chip::staAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staAbs1() {
    _currentInstruction = &Chip::staAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteStore(_accumulator);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staAbsX0() {
    _currentInstruction = &Chip::staAbsX1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staAbsX1() {
    _currentInstruction = &Chip::staAbsX2;
    absoluteIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staAbsX2() {
    _currentInstruction = &Chip::staAbsX3;
    absoluteIndexedStore0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staAbsX3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedStore1(_accumulator);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staAbsY0() {
    _currentInstruction = &Chip::staAbsY1;
    absoluteIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staAbsY1() {
    _currentInstruction = &Chip::staAbsY2;
    absoluteIndexedY1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staAbsY2() {
    _currentInstruction = &Chip::staAbsY3;
    absoluteIndexedStore0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staAbsY3() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteIndexedStore1(_accumulator);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staIndX0() {
    _currentInstruction = &Chip::staIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staIndX1() {
    _currentInstruction = &Chip::staIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staIndX2() {
    _currentInstruction = &Chip::staIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staIndX3() {
    _currentInstruction = &Chip::staIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staIndX4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPagePreIndexedIndirectStore(_accumulator);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staIndY0() {
    _currentInstruction = &Chip::staIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staIndY1() {
    _currentInstruction = &Chip::staIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staIndY2() {
    _currentInstruction = &Chip::staIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staIndY3() {
    _currentInstruction = &Chip::staIndY4;
    zeroPageIndirectPostIndexedStore0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::staIndY4() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndirectPostIndexedStore1(_accumulator);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::stxZp0() {
    _currentInstruction = &Chip::stxZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::stxZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageStore(_xIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::stxZpY0() {
    _currentInstruction = &Chip::stxZpY1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::stxZpY1() {
    _currentInstruction = &Chip::stxZpY2;
    zeroPageIndexedY1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::stxZpY2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedStore(_xIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::stxAbs0() {
    _currentInstruction = &Chip::stxAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::stxAbs1() {
    _currentInstruction = &Chip::stxAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::stxAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteStore(_xIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::styZp0() {
    _currentInstruction = &Chip::styZp1;
    zeroPage();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::styZp1() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageStore(_yIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::styZpX0() {
    _currentInstruction = &Chip::styZpX1;
    zeroPageIndexed0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::styZpX1() {
    _currentInstruction = &Chip::styZpX2;
    zeroPageIndexedX1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::styZpX2() {
    _currentInstruction = &Chip::fetchOpcode;
    zeroPageIndexedStore(_yIndex);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::styAbs0() {
    _currentInstruction = &Chip::styAbs1;
    absolute0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::styAbs1() {
    _currentInstruction = &Chip::styAbs2;
    absolute1();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::styAbs2() {
    _currentInstruction = &Chip::fetchOpcode;
    absoluteStore(_yIndex);
}

#endif /* Cpu6502_Internal_Memory_s_hpp */
