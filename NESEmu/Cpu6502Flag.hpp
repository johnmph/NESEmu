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
void clc();
void cld();
void cli();
void clv();
void setFlag(Flags flag);
void sec();
void sed();
void sei();

#endif /* Cpu6502Flag_hpp */
