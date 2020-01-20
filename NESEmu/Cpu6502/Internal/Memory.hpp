//
//  Memory.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Memory_hpp
#define Cpu6502_Internal_Memory_hpp


// LDA, LDX, LDY, STA, STX, STY

void ld0(uint8_t &data);

void lda0();

void ldaImm0();

void ldaZp0();
void ldaZp1();

void ldaZpX0();
void ldaZpX1();
void ldaZpX2();

void ldaAbs0();
void ldaAbs1();
void ldaAbs2();

void ldaAbsX0();
void ldaAbsX1();
void ldaAbsX2();
void ldaAbsX3();

void ldaAbsY0();
void ldaAbsY1();
void ldaAbsY2();
void ldaAbsY3();

void ldaIndX0();
void ldaIndX1();
void ldaIndX2();
void ldaIndX3();
void ldaIndX4();

void ldaIndY0();
void ldaIndY1();
void ldaIndY2();
void ldaIndY3();
void ldaIndY4();

void ldx0();

void ldxImm0();

void ldxZp0();
void ldxZp1();

void ldxZpY0();
void ldxZpY1();
void ldxZpY2();

void ldxAbs0();
void ldxAbs1();
void ldxAbs2();

void ldxAbsY0();
void ldxAbsY1();
void ldxAbsY2();
void ldxAbsY3();

void ldy0();

void ldyImm0();

void ldyZp0();
void ldyZp1();

void ldyZpX0();
void ldyZpX1();
void ldyZpX2();

void ldyAbs0();
void ldyAbs1();
void ldyAbs2();

void ldyAbsX0();
void ldyAbsX1();
void ldyAbsX2();
void ldyAbsX3();

void staZp0();
void staZp1();

void staZpX0();
void staZpX1();
void staZpX2();

void staAbs0();
void staAbs1();
void staAbs2();

void staAbsX0();
void staAbsX1();
void staAbsX2();
void staAbsX3();

void staAbsY0();
void staAbsY1();
void staAbsY2();
void staAbsY3();

void staIndX0();
void staIndX1();
void staIndX2();
void staIndX3();
void staIndX4();

void staIndY0();
void staIndY1();
void staIndY2();
void staIndY3();
void staIndY4();

void stxZp0();
void stxZp1();

void stxZpY0();
void stxZpY1();
void stxZpY2();

void stxAbs0();
void stxAbs1();
void stxAbs2();

void styZp0();
void styZp1();

void styZpX0();
void styZpX1();
void styZpX2();

void styAbs0();
void styAbs1();
void styAbs2();

#endif /* Cpu6502_Internal_Memory_hpp */
