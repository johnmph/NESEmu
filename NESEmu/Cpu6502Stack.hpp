//
//  Cpu6502Stack.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Stack_hpp
#define Cpu6502Stack_hpp


// PHA, PHP, PLA, PLP, TSX, TXS

void pha0() {
    pushToStack0(_accumulator);
} // + implied avant

void php0() {
    pushToStack0(_statusFlags);
}   // + implied avant

void ph1() {
    pushToStack1();
    finishInstruction();
}

void pla() {
    _accumulator = _inputDataLatch;
    
    // Update status
    clearStatusFlags({ Flags::Zero, Flags::Negative });     // TODO: par apres si beaucoup d'instructions utilisent ca, avoir une methode setZeroNegative(data)
    setStatusFlag(Flags::Zero, (_accumulator == 0));
    setStatusFlag(Flags::Negative, (_accumulator & 0x80));
    
    finishInstruction();
}   // +implied avant + pullFromStack0() + pullFromStack1() avant

void plp() {
    _statusFlags = _inputDataLatch;
    
    finishInstruction();
}   // +implied avant + pullFromStack0() + pullFromStack1() avant

#endif /* Cpu6502Stack_hpp */
