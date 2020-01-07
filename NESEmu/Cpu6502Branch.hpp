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

void branch(bool condition) {    // TODO: voir les interruptions avec les opcode de branchement (et voir aussi CLI SEI et PLP)
    // Adding offset with programCounterLow using ALU
    _aInput = _inputDataLatch;
    _bInput = _programCounterLow;
    aluPerformSum(false, false);
    
    if (condition == true) {
        // Branch is taken, this data will not be used
        fetchData();    // TODO: voir si fetchData ou read (pour ne pas incrementer le PC)
        return;
    }
    
    // Branch is not taken, read next instruction
    finishInstruction();
}

void bpl() {
    branch(getStatusFlag(Flags::Negative) == false);
}

void bmi() {
    branch(getStatusFlag(Flags::Negative) == true);
}

void bvc() {
    branch(getStatusFlag(Flags::Overflow) == false);
}

void bvs() {
    branch(getStatusFlag(Flags::Overflow) == true);
}

void bcc() {
    branch(getStatusFlag(Flags::Carry) == false);
}

void bcs() {
    branch(getStatusFlag(Flags::Carry) == true);
}

void bne() {
    branch(getStatusFlag(Flags::Zero) == false);
}

void beq() {
    branch(getStatusFlag(Flags::Zero) == true);
}

#endif /* Cpu6502Branch_hpp */
