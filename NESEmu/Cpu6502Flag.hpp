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

void clearFlag(Flags flag) {
    clearStatusFlags({ flag });
    
    finishInstruction();
}

void clc() {
    clearFlag(Flags::Carry);
}

void cld() {
    clearFlag(Flags::DecimalMode);
}

void cli() {
    clearFlag(Flags::InterruptDisable);
}

void clv() {
    clearFlag(Flags::Overflow);
}

void setFlag(Flags flag) {
    setStatusFlag(flag, true);
    
    finishInstruction();
}

void sec() {
    setFlag(Flags::Carry);
}

void sed() {
    setFlag(Flags::DecimalMode);
}

void sei() {
    setFlag(Flags::InterruptDisable);
}

#endif /* Cpu6502Flag_hpp */
