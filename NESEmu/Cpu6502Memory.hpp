//
//  Cpu6502Memory.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Memory_hpp
#define Cpu6502Memory_hpp


// LDA, STA, LDX, STX, LDY, STY, INC, DEC

void lda();
void staAbsolute();
void staZeroPage();
void staAbsoluteIndexed();
void staZeroPageIndexed();

#endif /* Cpu6502Memory_hpp */
