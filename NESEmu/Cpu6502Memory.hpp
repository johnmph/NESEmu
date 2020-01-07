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

void lda() {
    _accumulator = _inputDataLatch;
    
    clearStatusFlags({ Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Zero, _accumulator == 0);
    setStatusFlag(Flags::Negative, _accumulator & 0x80);
    
    finishInstruction();
}

void staAbsolute() {
    absoluteStore(_accumulator);
}// TODO: + finishInstruction mais dans le step d'apres

void staZeroPage() {
    zeroPageStore(_accumulator);
}// TODO: + finishInstruction mais dans le step d'apres

void staAbsoluteIndexed() {
    absoluteIndexedStore1(_accumulator);
}// TODO: + commencer par absoluteIndexedStore0 et finishInstruction mais dans le step d'apres

void staZeroPageIndexed() {
    zeroPageIndexedStore(_accumulator);
}// TODO: + finishInstruction mais dans le step d'apres

#endif /* Cpu6502Memory_hpp */
