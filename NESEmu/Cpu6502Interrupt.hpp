//
//  Cpu6502Interrupt.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Interrupt_hpp
#define Cpu6502Interrupt_hpp


// BRK (+ IRQ/NMI/RESET), RTI

void reset0();
void reset1();

void brk0();
void brk1();
void brk2();
void brk3();
void brk4();
void brk5();
void brk6();

void rti0();
void rti1();
void rti2();
void rti3();
void rti4();
void rti5();

#endif /* Cpu6502Interrupt_hpp */
