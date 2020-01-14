//
//  Cpu6502Flag.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Flag_hpp
#define Cpu6502Flag_hpp


// CLC, CLD, CLI, CLV, SEC, SED, SEI

void clearFlag(Flags flag);

void clc0();
void clc1();

void cld0();
void cld1();

void cli0();
void cli1();

void clv0();
void clv1();

void setFlag(Flags flag);

void sec0();
void sec1();

void sed0();
void sed1();

void sei0();
void sei1();

#endif /* Cpu6502Flag_hpp */
