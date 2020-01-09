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

void branch(bool condition);

void bpl0();
void bpl1();
void bpl2();
void bpl3();

void bmi0();
void bmi1();
void bmi2();
void bmi3();

void bvc0();
void bvc1();
void bvc2();
void bvc3();

void bvs0();
void bvs1();
void bvs2();
void bvs3();

void bcc0();
void bcc1();
void bcc2();
void bcc3();

void bcs0();
void bcs1();
void bcs2();
void bcs3();

void bne0();
void bne1();
void bne2();
void bne3();

void beq0();
void beq1();
void beq2();
void beq3();

#endif /* Cpu6502Branch_hpp */
