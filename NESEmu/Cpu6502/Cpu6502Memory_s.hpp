//
//  Cpu6502Memory_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Memory_s_hpp
#define Cpu6502Memory_s_hpp


template <class TBus>
void Cpu6502<TBus>::ld0(uint8_t &data) {
    data = _inputDataLatch;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(data);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::lda0() {
    ld0(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::ldaImm0() {
    _currentInstruction = &Cpu6502::lda0;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::ldaZp0() {
    _currentInstruction = &Cpu6502::ldaZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::ldaZp1() {
    _currentInstruction = &Cpu6502::lda0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::ldaZpX0() {
    _currentInstruction = &Cpu6502::ldaZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::ldaZpX1() {
    _currentInstruction = &Cpu6502::ldaZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::ldaZpX2() {
    _currentInstruction = &Cpu6502::lda0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::ldaAbs0() {
    _currentInstruction = &Cpu6502::ldaAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::ldaAbs1() {
    _currentInstruction = &Cpu6502::ldaAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::ldaAbs2() {
    _currentInstruction = &Cpu6502::lda0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::ldaAbsX0() {
    _currentInstruction = &Cpu6502::ldaAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::ldaAbsX1() {
    _currentInstruction = &Cpu6502::ldaAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::ldaAbsX2() {
    _currentInstruction = &Cpu6502::ldaAbsX3;
    absoluteIndexedLoad0(&Cpu6502::lda0);
}

template <class TBus>
void Cpu6502<TBus>::ldaAbsX3() {
    _currentInstruction = &Cpu6502::lda0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::ldaAbsY0() {
    _currentInstruction = &Cpu6502::ldaAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::ldaAbsY1() {
    _currentInstruction = &Cpu6502::ldaAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::ldaAbsY2() {
    _currentInstruction = &Cpu6502::ldaAbsY3;
    absoluteIndexedLoad0(&Cpu6502::lda0);
}

template <class TBus>
void Cpu6502<TBus>::ldaAbsY3() {
    _currentInstruction = &Cpu6502::lda0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::ldaIndX0() {
    _currentInstruction = &Cpu6502::ldaIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::ldaIndX1() {
    _currentInstruction = &Cpu6502::ldaIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::ldaIndX2() {
    _currentInstruction = &Cpu6502::ldaIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::ldaIndX3() {
    _currentInstruction = &Cpu6502::ldaIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::ldaIndX4() {
    _currentInstruction = &Cpu6502::lda0;
    zeroPagePreIndexedIndirectLoad();
}

template <class TBus>
void Cpu6502<TBus>::ldaIndY0() {
    _currentInstruction = &Cpu6502::ldaIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::ldaIndY1() {
    _currentInstruction = &Cpu6502::ldaIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::ldaIndY2() {
    _currentInstruction = &Cpu6502::ldaIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::ldaIndY3() {
    _currentInstruction = &Cpu6502::ldaIndY4;
    zeroPageIndirectPostIndexedLoad0(&Cpu6502::lda0);
}

template <class TBus>
void Cpu6502<TBus>::ldaIndY4() {
    _currentInstruction = &Cpu6502::lda0;
    zeroPageIndirectPostIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::ldx0() {
    ld0(_xIndex);
}

template <class TBus>
void Cpu6502<TBus>::ldxImm0() {
    _currentInstruction = &Cpu6502::ldx0;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::ldxZp0() {
    _currentInstruction = &Cpu6502::ldxZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::ldxZp1() {
    _currentInstruction = &Cpu6502::ldx0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::ldxZpY0() {
    _currentInstruction = &Cpu6502::ldxZpY1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::ldxZpY1() {
    _currentInstruction = &Cpu6502::ldxZpY2;
    zeroPageIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::ldxZpY2() {
    _currentInstruction = &Cpu6502::ldx0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::ldxAbs0() {
    _currentInstruction = &Cpu6502::ldxAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::ldxAbs1() {
    _currentInstruction = &Cpu6502::ldxAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::ldxAbs2() {
    _currentInstruction = &Cpu6502::ldx0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::ldxAbsY0() {
    _currentInstruction = &Cpu6502::ldxAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::ldxAbsY1() {
    _currentInstruction = &Cpu6502::ldxAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::ldxAbsY2() {
    _currentInstruction = &Cpu6502::ldxAbsY3;
    absoluteIndexedLoad0(&Cpu6502::ldx0);
}

template <class TBus>
void Cpu6502<TBus>::ldxAbsY3() {
    _currentInstruction = &Cpu6502::ldx0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::ldy0() {
    ld0(_yIndex);
}

template <class TBus>
void Cpu6502<TBus>::ldyImm0() {
    _currentInstruction = &Cpu6502::ldy0;
    immediate();
}

template <class TBus>
void Cpu6502<TBus>::ldyZp0() {
    _currentInstruction = &Cpu6502::ldyZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::ldyZp1() {
    _currentInstruction = &Cpu6502::ldy0;
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::ldyZpX0() {
    _currentInstruction = &Cpu6502::ldyZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::ldyZpX1() {
    _currentInstruction = &Cpu6502::ldyZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::ldyZpX2() {
    _currentInstruction = &Cpu6502::ldy0;
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::ldyAbs0() {
    _currentInstruction = &Cpu6502::ldyAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::ldyAbs1() {
    _currentInstruction = &Cpu6502::ldyAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::ldyAbs2() {
    _currentInstruction = &Cpu6502::ldy0;
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::ldyAbsX0() {
    _currentInstruction = &Cpu6502::ldyAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::ldyAbsX1() {
    _currentInstruction = &Cpu6502::ldyAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::ldyAbsX2() {
    _currentInstruction = &Cpu6502::ldyAbsX3;
    absoluteIndexedLoad0(&Cpu6502::ldy0);
}

template <class TBus>
void Cpu6502<TBus>::ldyAbsX3() {
    _currentInstruction = &Cpu6502::ldy0;
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::staZp0() {
    _currentInstruction = &Cpu6502::staZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::staZp1() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPageStore(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::staZpX0() {
    _currentInstruction = &Cpu6502::staZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::staZpX1() {
    _currentInstruction = &Cpu6502::staZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::staZpX2() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPageIndexedStore(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::staAbs0() {
    _currentInstruction = &Cpu6502::staAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::staAbs1() {
    _currentInstruction = &Cpu6502::staAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::staAbs2() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    absoluteStore(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::staAbsX0() {
    _currentInstruction = &Cpu6502::staAbsX1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::staAbsX1() {
    _currentInstruction = &Cpu6502::staAbsX2;
    absoluteIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::staAbsX2() {
    _currentInstruction = &Cpu6502::staAbsX3;
    absoluteIndexedStore0();
}

template <class TBus>
void Cpu6502<TBus>::staAbsX3() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    absoluteIndexedStore1(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::staAbsY0() {
    _currentInstruction = &Cpu6502::staAbsY1;
    absoluteIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::staAbsY1() {
    _currentInstruction = &Cpu6502::staAbsY2;
    absoluteIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::staAbsY2() {
    _currentInstruction = &Cpu6502::staAbsY3;
    absoluteIndexedStore0();
}

template <class TBus>
void Cpu6502<TBus>::staAbsY3() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    absoluteIndexedStore1(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::staIndX0() {
    _currentInstruction = &Cpu6502::staIndX1;
    zeroPagePreIndexedIndirect0();
}

template <class TBus>
void Cpu6502<TBus>::staIndX1() {
    _currentInstruction = &Cpu6502::staIndX2;
    zeroPagePreIndexedIndirect1();
}

template <class TBus>
void Cpu6502<TBus>::staIndX2() {
    _currentInstruction = &Cpu6502::staIndX3;
    zeroPagePreIndexedIndirect2();
}

template <class TBus>
void Cpu6502<TBus>::staIndX3() {
    _currentInstruction = &Cpu6502::staIndX4;
    zeroPagePreIndexedIndirect3();
}

template <class TBus>
void Cpu6502<TBus>::staIndX4() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPagePreIndexedIndirectStore(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::staIndY0() {
    _currentInstruction = &Cpu6502::staIndY1;
    zeroPageIndirectPostIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::staIndY1() {
    _currentInstruction = &Cpu6502::staIndY2;
    zeroPageIndirectPostIndexed1();
}

template <class TBus>
void Cpu6502<TBus>::staIndY2() {
    _currentInstruction = &Cpu6502::staIndY3;
    zeroPageIndirectPostIndexed2();
}

template <class TBus>
void Cpu6502<TBus>::staIndY3() {
    _currentInstruction = &Cpu6502::staIndY4;
    zeroPageIndirectPostIndexedStore0();
}

template <class TBus>
void Cpu6502<TBus>::staIndY4() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPageIndirectPostIndexedStore1(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::stxZp0() {
    _currentInstruction = &Cpu6502::stxZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::stxZp1() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPageStore(_xIndex);
}

template <class TBus>
void Cpu6502<TBus>::stxZpY0() {
    _currentInstruction = &Cpu6502::stxZpY1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::stxZpY1() {
    _currentInstruction = &Cpu6502::stxZpY2;
    zeroPageIndexedY1();
}

template <class TBus>
void Cpu6502<TBus>::stxZpY2() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPageIndexedStore(_xIndex);
}

template <class TBus>
void Cpu6502<TBus>::stxAbs0() {
    _currentInstruction = &Cpu6502::stxAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::stxAbs1() {
    _currentInstruction = &Cpu6502::stxAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::stxAbs2() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    absoluteStore(_xIndex);
}

template <class TBus>
void Cpu6502<TBus>::styZp0() {
    _currentInstruction = &Cpu6502::styZp1;
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::styZp1() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPageStore(_yIndex);
}

template <class TBus>
void Cpu6502<TBus>::styZpX0() {
    _currentInstruction = &Cpu6502::styZpX1;
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::styZpX1() {
    _currentInstruction = &Cpu6502::styZpX2;
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::styZpX2() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    zeroPageIndexedStore(_yIndex);
}

template <class TBus>
void Cpu6502<TBus>::styAbs0() {
    _currentInstruction = &Cpu6502::styAbs1;
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::styAbs1() {
    _currentInstruction = &Cpu6502::styAbs2;
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::styAbs2() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    absoluteStore(_yIndex);
}



#endif /* Cpu6502Memory_s_hpp */
