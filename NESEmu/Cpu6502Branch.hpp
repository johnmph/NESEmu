//
//  Cpu6502Branch.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Branch_hpp
#define Cpu6502Branch_hpp


// BPL, BMI, BVC, BVS, BCC, BCS, BNE, BEQ

void bpl0();
void bpl1();

void bmi0();
void bmi1();

void bvc0();
void bvc1();

void bvs0();
void bvs1();

void bcc0();
void bcc1();

void bcs0();
void bcs1();

void bne0();
void bne1();

void beq0();
void beq1();

#endif /* Cpu6502Branch_hpp */
