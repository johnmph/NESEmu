//
//  Cpu6502Flow.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Flow_hpp
#define Cpu6502Flow_hpp


// JMP, JSR, NOP, RTS

void jmpAbs0();
void jmpAbs1();
void jmpAbs2();

void jmpInd0();
void jmpInd1();
void jmpInd2();
void jmpInd3();
void jmpInd4();

void jsr0();
void jsr1();
void jsr2();
void jsr3();
void jsr4();
void jsr5();

void nop0();

void rts0();
void rts1();
void rts2();
void rts3();
void rts4();
void rts5();

#endif /* Cpu6502Flow_hpp */
