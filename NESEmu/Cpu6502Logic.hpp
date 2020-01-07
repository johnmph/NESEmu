//
//  Cpu6502Logic.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Logic_hpp
#define Cpu6502Logic_hpp


// AND, ASL, EOR, ORA, ROL, ROR, LSR

void ora();
void and_();
void eor();
void asl0(uint8_t data);
void aslAccumulator0();
void aslAccumulator1();
void asl0();
void asl1();

#endif /* Cpu6502Logic_hpp */
