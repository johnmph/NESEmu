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
void bpl();
void bmi();
void bvc();
void bvs();
void bcc();
void bcs();
void bne();
void beq();

#endif /* Cpu6502Branch_hpp */
