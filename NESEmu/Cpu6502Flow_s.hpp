//
//  Cpu6502Flow_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Flow_s_hpp
#define Cpu6502Flow_s_hpp


template <class TBus>
void Cpu6502<TBus>::jmpAbs0() {
    _currentInstruction = &Cpu6502::jmpAbs1;
    
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::jmpAbs1() {
    _currentInstruction = &Cpu6502::jmpAbs2;
    
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::jmpAbs2() {
    // Set program counter
    _programCounterLow = _addressBusLowRegister;
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::jmpInd0() {
    _currentInstruction = &Cpu6502::jmpInd1;
    
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::jmpInd1() {
    _currentInstruction = &Cpu6502::jmpInd2;
    
    absolute1();
}

template <class TBus>
void Cpu6502<TBus>::jmpInd2() {
    _currentInstruction = &Cpu6502::jmpInd3;
    
    // Adding 1 with addressBusLowRegister using ALU (Add 0 with carry set like true 6502)
    _aInput = 0;
    _bInput = _addressBusLowRegister;
    aluPerformSum(false, true);
    
    // Save loaded addressBusHigh
    _addressBusHighRegister = _inputDataLatch;  // TODO: voir si ca ou bien on garde addressBusHigh mais dans ce cas a quoi sert ce registre ? (et il y a d'autres endroit ou j'ai fait ca en ne gardant que le addressBusHigh, doit etre consistent)
    
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::jmpInd3() {
    _currentInstruction = &Cpu6502::jmpInd4;
    
    // Save addressBusLow
    //_addressBusLowRegister = _inputDataLatch;
    _programCounterLow = _inputDataLatch;
    
    // Read addressBusHigh
    readDataBus(_adderHold, _addressBusHighRegister);
}

template <class TBus>
void Cpu6502<TBus>::jmpInd4() {
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::jsr0() {
    _currentInstruction = &Cpu6502::jsr1;
    
    // Fetch ADL
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::jsr1() {
    _currentInstruction = &Cpu6502::jsr2;
    
    // Store ADL
    _addressBusLowRegister = _inputDataLatch;
    
    // ???
}

template <class TBus>
void Cpu6502<TBus>::jsr2() {
    _currentInstruction = &Cpu6502::jsr3;
    
    // Push PCH to stack
    pushToStack0(_programCounterHigh);
}

template <class TBus>
void Cpu6502<TBus>::jsr3() {
    _currentInstruction = &Cpu6502::jsr4;
    
    // Finish stack operation and push PCL to stack
    pushToStack1();
    pushToStack0(_programCounterLow);
}

template <class TBus>
void Cpu6502<TBus>::jsr4() {
    _currentInstruction = &Cpu6502::jsr5;
    
    // Finish stack operation
    pushToStack1();
    
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::jsr5() {
    _programCounterLow = _addressBusLowRegister;
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::nop0() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    implied();
}

template <class TBus>
void Cpu6502<TBus>::rts0() {
    _currentInstruction = &Cpu6502::rts1;
    
    pullFromStack0();
}

template <class TBus>
void Cpu6502<TBus>::rts1() {
    _currentInstruction = &Cpu6502::rts2;
    
    pullFromStack1();
    pullFromStack0();
}

template <class TBus>
void Cpu6502<TBus>::rts2() {
    _currentInstruction = &Cpu6502::rts3;
    
    _programCounterLow = _inputDataLatch;
    
    pullFromStack1();
}

template <class TBus>
void Cpu6502<TBus>::rts3() {    // TODO: verifier car pas de read/write
    _currentInstruction = &Cpu6502::rts4;
    
    _programCounterHigh = _inputDataLatch;
}

template <class TBus>
void Cpu6502<TBus>::rts4() {    // TODO: verifier
    incrementProgramCounter();
    fetchOpcode();
}

#endif /* Cpu6502Flow_s_hpp */
