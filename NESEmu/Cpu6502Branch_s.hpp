//
//  Cpu6502Branch_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Branch_s_hpp
#define Cpu6502Branch_s_hpp


template <class TBus>
void Cpu6502<TBus>::branch(bool condition) {    // TODO: voir les interruptions avec les opcode de branchement (et voir aussi CLI SEI et PLP)
    // Adding offset with programCounterLow using ALU
    _aInput = _inputDataLatch;
    _bInput = _programCounterLow;
    aluPerformSum(false, false);
    
    if (condition == true) {
        // Branch is taken, this data will not be used
        fetchData();    // TODO: voir si fetchData ou read (pour ne pas incrementer le PC)
        return;
    }
    
    // Branch is not taken, fetch next opcode
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::bpl0() {
    _currentInstruction = &Cpu6502::bpl1;
    relative();
}

template <class TBus>
void Cpu6502<TBus>::bpl1() {
    _currentInstruction = &Cpu6502::bpl2;
    branch(getStatusFlag(Flags::Negative) == false);
}

template <class TBus>
void Cpu6502<TBus>::bpl2() {
    _currentInstruction = &Cpu6502::bpl3;
    relativeBranch0();
}

template <class TBus>
void Cpu6502<TBus>::bpl3() {
    relativeBranch1();
}

template <class TBus>
void Cpu6502<TBus>::bmi0() {
    _currentInstruction = &Cpu6502::bmi1;
    relative();
}

template <class TBus>
void Cpu6502<TBus>::bmi1() {
    _currentInstruction = &Cpu6502::bmi2;
    branch(getStatusFlag(Flags::Negative) == true);
}

template <class TBus>
void Cpu6502<TBus>::bmi2() {
    _currentInstruction = &Cpu6502::bmi3;
    relativeBranch0();
}

template <class TBus>
void Cpu6502<TBus>::bmi3() {
    relativeBranch1();
}

template <class TBus>
void Cpu6502<TBus>::bvc0() {
    _currentInstruction = &Cpu6502::bvc1;
    relative();
}

template <class TBus>
void Cpu6502<TBus>::bvc1() {
    _currentInstruction = &Cpu6502::bvc2;
    branch(getStatusFlag(Flags::Overflow) == false);
}

template <class TBus>
void Cpu6502<TBus>::bvc2() {
    _currentInstruction = &Cpu6502::bvc3;
    relativeBranch0();
}

template <class TBus>
void Cpu6502<TBus>::bvc3() {
    relativeBranch1();
}

template <class TBus>
void Cpu6502<TBus>::bvs0() {
    _currentInstruction = &Cpu6502::bvs1;
    relative();
}

template <class TBus>
void Cpu6502<TBus>::bvs1() {
    _currentInstruction = &Cpu6502::bvs2;
    branch(getStatusFlag(Flags::Overflow) == true);
}

template <class TBus>
void Cpu6502<TBus>::bvs2() {
    _currentInstruction = &Cpu6502::bvs3;
    relativeBranch0();
}

template <class TBus>
void Cpu6502<TBus>::bvs3() {
    relativeBranch1();
}

template <class TBus>
void Cpu6502<TBus>::bcc0() {
    _currentInstruction = &Cpu6502::bcc1;
    relative();
}

template <class TBus>
void Cpu6502<TBus>::bcc1() {
    _currentInstruction = &Cpu6502::bcc2;
    branch(getStatusFlag(Flags::Carry) == false);
}

template <class TBus>
void Cpu6502<TBus>::bcc2() {
    _currentInstruction = &Cpu6502::bcc3;
    relativeBranch0();
}

template <class TBus>
void Cpu6502<TBus>::bcc3() {
    relativeBranch1();
}

template <class TBus>
void Cpu6502<TBus>::bcs0() {
    _currentInstruction = &Cpu6502::bcs1;
    relative();
}

template <class TBus>
void Cpu6502<TBus>::bcs1() {
    _currentInstruction = &Cpu6502::bcs2;
    branch(getStatusFlag(Flags::Carry) == true);
}

template <class TBus>
void Cpu6502<TBus>::bcs2() {
    _currentInstruction = &Cpu6502::bcs3;
    relativeBranch0();
}

template <class TBus>
void Cpu6502<TBus>::bcs3() {
    relativeBranch1();
}

template <class TBus>
void Cpu6502<TBus>::bne0() {
    _currentInstruction = &Cpu6502::bne1;
    relative();
}

template <class TBus>
void Cpu6502<TBus>::bne1() {
    _currentInstruction = &Cpu6502::bne2;
    branch(getStatusFlag(Flags::Zero) == false);
}

template <class TBus>
void Cpu6502<TBus>::bne2() {
    _currentInstruction = &Cpu6502::bne3;
    relativeBranch0();
}

template <class TBus>
void Cpu6502<TBus>::bne3() {
    relativeBranch1();
}

template <class TBus>
void Cpu6502<TBus>::beq0() {
    _currentInstruction = &Cpu6502::beq1;
    relative();
}

template <class TBus>
void Cpu6502<TBus>::beq1() {
    _currentInstruction = &Cpu6502::beq2;
    branch(getStatusFlag(Flags::Zero) == true);
}

template <class TBus>
void Cpu6502<TBus>::beq2() {
    _currentInstruction = &Cpu6502::beq3;
    relativeBranch0();
}

template <class TBus>
void Cpu6502<TBus>::beq3() {
    relativeBranch1();
}

#endif /* Cpu6502Branch_s_hpp */
