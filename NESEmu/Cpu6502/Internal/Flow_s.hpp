//
//  Flow_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Flow_s_hpp
#define Cpu6502_Internal_Flow_s_hpp


template <class TBus>
void Chip<TBus>::jmpAbs0() {
    _currentInstruction = &Chip::jmpAbs1;
    
    absolute0();
}

template <class TBus>
void Chip<TBus>::jmpAbs1() {
    _currentInstruction = &Chip::jmpAbs2;
    
    absolute1();
}

template <class TBus>
void Chip<TBus>::jmpAbs2() {
    // Set program counter
    _programCounterLow = _adderHold;
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

template <class TBus>
void Chip<TBus>::jmpInd0() {
    _currentInstruction = &Chip::jmpInd1;
    
    absolute0();
}

template <class TBus>
void Chip<TBus>::jmpInd1() {
    _currentInstruction = &Chip::jmpInd2;
    
    absolute1();
}

template <class TBus>
void Chip<TBus>::jmpInd2() {
    _currentInstruction = &Chip::jmpInd3;
    
    // Read address low (need to be put before ALU operation below because it need adderHold before incrementation)
    absoluteLoad();
    
    // Adding 1 to address low using ALU (Add 0 with carry set like true 6502)
    _aInput = 0;
    _bInput = _adderHold;
    aluPerformSum(false, true);
}

template <class TBus>
void Chip<TBus>::jmpInd3() {
    _currentInstruction = &Chip::jmpInd4;
    
    // Load address low in PC
    _programCounterLow = _inputDataLatch;
    
    // Read address high
    readDataBus(_adderHold, _addressBusHigh);
}

template <class TBus>
void Chip<TBus>::jmpInd4() {
    // Load address high in PC
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

template <class TBus>
void Chip<TBus>::jsr0() {
    _currentInstruction = &Chip::jsr1;
    
    // Fetch ADL
    fetchData();
}

template <class TBus>
void Chip<TBus>::jsr1() {
    _currentInstruction = &Chip::jsr2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    
    // Store address low in stack pointer
    _stackPointer = _inputDataLatch;
}

template <class TBus>
void Chip<TBus>::jsr2() {
    _currentInstruction = &Chip::jsr3;
    
    // Push PCH to stack
    pushToStack0(_programCounterHigh);
    
    // Removing 1 from inputDataLatch using ALU (Add 0xFF without carry set like true 6502)
    _aInput = 0xFF;
    _bInput = _addressBusLow;
    aluPerformSum(false, false);
}

template <class TBus>
void Chip<TBus>::jsr3() {
    _currentInstruction = &Chip::jsr4;
    
    // Finish stack operation and push PCL to stack
    pushToStack1();
    pushToStack0(_programCounterLow);
}

template <class TBus>
void Chip<TBus>::jsr4() {
    _currentInstruction = &Chip::jsr5;
    
    // Don't finish stack operation because stack pointer is used to store address low and addressBusLow is set with PC to fetch address high
    
    // Read address high
    fetchData();
}

template <class TBus>
void Chip<TBus>::jsr5() {
    // Set program counter (address low was stored in stackPointer and address high was fetched in last cycle)
    _programCounterLow = _stackPointer;
    _programCounterHigh = _inputDataLatch;
    
    // Restore stack pointer
    _stackPointer = _adderHold;
    
    fetchOpcode();
}

template <class TBus>
void Chip<TBus>::nop0() {
    _currentInstruction = &Chip::fetchOpcode;
    implied();
}

template <class TBus>
void Chip<TBus>::rts0() {
    _currentInstruction = &Chip::rts1;
    
    implied();
}

template <class TBus>
void Chip<TBus>::rts1() {
    _currentInstruction = &Chip::rts2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    
    pullFromStack0();
}

template <class TBus>
void Chip<TBus>::rts2() {
    _currentInstruction = &Chip::rts3;
    
    pullFromStack1();
    pullFromStack0();
}

template <class TBus>
void Chip<TBus>::rts3() {
    _currentInstruction = &Chip::rts4;
    
    _programCounterLow = _inputDataLatch;
    
    pullFromStack1();
    stopStackOperation();
}

template <class TBus>
void Chip<TBus>::rts4() {
    _currentInstruction = &Chip::fetchOpcode;
    
    _programCounterHigh = _inputDataLatch;
    fetchData();
}

template <class TBus>
void Chip<TBus>::unofficial() {  // TODO: a retirer une fois tous les opcodes gérés
    nop0();
}

#endif /* Cpu6502_Internal_Flow_s_hpp */
