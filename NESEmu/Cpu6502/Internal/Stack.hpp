//
//  Stack.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Stack_hpp
#define Cpu6502_Internal_Stack_hpp


// PHA, PHP, PLA, PLP

void pha0();
void pha1();
void pha2();

void php0();
void php1();
void php2();

void pla0();
void pla1();
void pla2();
void pla3();

void plp0();
void plp1();
void plp2();
void plp3();

#endif /* Cpu6502_Internal_Stack_hpp */
