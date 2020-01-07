//
//  Cpu6502Logic.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Logic_hpp
#define Cpu6502Logic_hpp


// AND, ASL, EOR, ORA, ROL, ROR, LSR

void ora() {         // TODO: ca doit etre ainsi mais comment ca se fait qu'on ait directement accès au resultat de l'ALU ???
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformOr();
    
    // Store ALU result in accumulator
    _accumulator = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Zero, Flags::Negative });     // TODO: par apres si beaucoup d'instructions utilisent ca, avoir une methode setZeroNegative(data)
    setStatusFlag(Flags::Zero, (_accumulator == 0));
    setStatusFlag(Flags::Negative, (_accumulator & 0x80));
    
    finishInstruction();
}

void and_() {         // TODO: ca doit etre ainsi mais comment ca se fait qu'on ait directement accès au resultat de l'ALU ???
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformAnd();
    
    // Store ALU result in accumulator
    _accumulator = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Zero, Flags::Negative });     // TODO: par apres si beaucoup d'instructions utilisent ca, avoir une methode setZeroNegative(data)
    setStatusFlag(Flags::Zero, (_accumulator == 0));
    setStatusFlag(Flags::Negative, (_accumulator & 0x80));
    
    finishInstruction();
}

void eor() {         // TODO: ca doit etre ainsi mais comment ca se fait qu'on ait directement accès au resultat de l'ALU ???
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformEor();
    
    // Store ALU result in accumulator
    _accumulator = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Zero, Flags::Negative });     // TODO: par apres si beaucoup d'instructions utilisent ca, avoir une methode setZeroNegative(data)
    setStatusFlag(Flags::Zero, (_accumulator == 0));
    setStatusFlag(Flags::Negative, (_accumulator & 0x80));
    
    finishInstruction();
}

void asl0(uint8_t data) {
    // ASL by adding same number to itself
    _aInput = data;
    _bInput = data;
    aluPerformSum(false, false);
}

void aslAccumulator0() {
    asl0(_accumulator);
}

void aslAccumulator1() {
    // Write result back
    _accumulator = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _adderHold);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
    
    finishInstruction();
}

void asl0() {
    asl0(_inputDataLatch);
}

void asl1() {
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, _adderHold);
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, _adderHold);
    setStatusFlag(Flags::Zero, (_adderHold == 0));
    setStatusFlag(Flags::Negative, (_adderHold & 0x80));
}// TODO: + finishInstruction mais dans le step d'apres

#endif /* Cpu6502Logic_hpp */
