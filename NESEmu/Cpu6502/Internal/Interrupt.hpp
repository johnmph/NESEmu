//
//  Interrupt.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Interrupt_hpp
#define Cpu6502_Internal_Interrupt_hpp


// BRK (+ IRQ/NMI/RESET), RTI

template <bool BResetAccurate, typename std::enable_if<BResetAccurate == true, int>::type = 0>
void reset0();

template <bool BResetAccurate, typename std::enable_if<BResetAccurate == false, int>::type = 0>
void reset0();

template <bool BResetAccurate, typename std::enable_if<BResetAccurate == false, int>::type = 0>
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

#endif /* Cpu6502_Internal_Interrupt_hpp */
