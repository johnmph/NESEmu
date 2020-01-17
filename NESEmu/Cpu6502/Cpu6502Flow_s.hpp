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
    _programCounterLow = _adderHold;
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
    
    // Read address low (need to be put before ALU operation below because it need adderHold before incrementation)
    absoluteLoad();
    
    // Adding 1 to address low using ALU (Add 0 with carry set like true 6502)
    _aInput = 0;
    _bInput = _adderHold;
    aluPerformSum(false, true);
}

template <class TBus>
void Cpu6502<TBus>::jmpInd3() {
    _currentInstruction = &Cpu6502::jmpInd4;
    
    // Load address low in PC
    _programCounterLow = _inputDataLatch;
    
    // Read address high
    readDataBus(_adderHold, _addressBusHigh);
}

template <class TBus>
void Cpu6502<TBus>::jmpInd4() {
    // Load address high in PC
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
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    
    // Store address low in stack pointer
    _stackPointer = _inputDataLatch;
}

template <class TBus>
void Cpu6502<TBus>::jsr2() {
    _currentInstruction = &Cpu6502::jsr3;
    
    // Push PCH to stack
    pushToStack0(_programCounterHigh);
    
    // Removing 1 from inputDataLatch using ALU (Add 0xFF without carry set like true 6502)
    _aInput = 0xFF;
    _bInput = _addressBusLow;
    aluPerformSum(false, false);
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
    
    // Don't finish stack operation because stack pointer is used to store address low and addressBusLow is set with PC to fetch address high
    
    // Read address high
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::jsr5() {
    // Set program counter (address low was stored in stackPointer and address high was fetched in last cycle)
    _programCounterLow = _stackPointer;
    _programCounterHigh = _inputDataLatch;
    
    // Restore stack pointer
    _stackPointer = _adderHold;
    
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
    
    implied();
}

template <class TBus>
void Cpu6502<TBus>::rts1() {
    _currentInstruction = &Cpu6502::rts2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    
    pullFromStack0();
}

template <class TBus>
void Cpu6502<TBus>::rts2() {
    _currentInstruction = &Cpu6502::rts3;
    
    pullFromStack1();
    pullFromStack0();
}

template <class TBus>
void Cpu6502<TBus>::rts3() {
    _currentInstruction = &Cpu6502::rts4;
    
    _programCounterLow = _inputDataLatch;
    
    pullFromStack1();
    stopStackOperation();
}

template <class TBus>
void Cpu6502<TBus>::rts4() {
    _currentInstruction = &Cpu6502::fetchOpcode;
    
    _programCounterHigh = _inputDataLatch;
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::unofficial() {  // TODO: a retirer une fois tous les opcodes gérés
    nop0();
}

#endif /* Cpu6502Flow_s_hpp */
