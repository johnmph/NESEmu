//
//  Flow_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Flow_s_hpp
#define Cpu6502_Internal_Flow_s_hpp


template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jmpAbs0() {
    _currentInstruction = &Chip::jmpAbs1;
    
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jmpAbs1() {
    _currentInstruction = &Chip::jmpAbs2;
    
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jmpAbs2() {
    // Set program counter
    _programCounterLow = _alu.getAdderHold();
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jmpInd0() {
    _currentInstruction = &Chip::jmpInd1;
    
    absolute0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jmpInd1() {
    _currentInstruction = &Chip::jmpInd2;
    
    absolute1();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jmpInd2() {
    _currentInstruction = &Chip::jmpInd3;
    
    // Read address low (need to be put before ALU operation below because it need adderHold before incrementation)
    absoluteLoad();
    
    // Adding 1 to address low using ALU (Add 0 with carry set like true 6502)
    _alu.performSum<BDecimalSupported, false>(0x0, _alu.getAdderHold(), false, true);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jmpInd3() {
    _currentInstruction = &Chip::jmpInd4;
    
    // Load address low in PC
    _programCounterLow = _inputDataLatch;
    
    // Read address high
    readDataBus(_alu.getAdderHold(), _addressBusHigh);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jmpInd4() {
    // Load address high in PC
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jsr0() {
    _currentInstruction = &Chip::jsr1;
    
    // Fetch ADL
    fetchData();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jsr1() {
    _currentInstruction = &Chip::jsr2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    
    // Store address low in stack pointer
    _stackPointer = _inputDataLatch;
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jsr2() {
    _currentInstruction = &Chip::jsr3;
    
    // Push PCH to stack
    pushToStack0(_programCounterHigh);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jsr3() {
    _currentInstruction = &Chip::jsr4;
    
    // Finish stack operation and push PCL to stack
    pushToStack1();
    pushToStack0(_programCounterLow);
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jsr4() {
    _currentInstruction = &Chip::jsr5;
    
    // Don't finish stack operation because stack pointer is used to store address low and addressBusLow is set with PC to fetch address high
    
    // Read address high
    fetchData();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::jsr5() {
    // Set program counter (address low was stored in stackPointer and address high was fetched in last cycle)
    _programCounterLow = _stackPointer;
    _programCounterHigh = _inputDataLatch;
    
    // Restore stack pointer
    _stackPointer = _alu.getAdderHold();
    
    fetchOpcode();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::nop0() {
    _currentInstruction = &Chip::fetchOpcode;
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rts0() {
    _currentInstruction = &Chip::rts1;
    
    implied();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rts1() {
    _currentInstruction = &Chip::rts2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    
    pullFromStack0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rts2() {
    _currentInstruction = &Chip::rts3;
    
    pullFromStack1();
    pullFromStack0();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rts3() {
    _currentInstruction = &Chip::rts4;
    
    _programCounterLow = _inputDataLatch;
    
    pullFromStack1();
    stopStackOperation();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::rts4() {
    _currentInstruction = &Chip::fetchOpcode;
    
    _programCounterHigh = _inputDataLatch;
    fetchData();
}

template <class TBus, bool BDecimalSupported>
void Chip<TBus, BDecimalSupported>::unofficial() {  // TODO: a retirer une fois tous les opcodes gérés
    nop0();
}

#endif /* Cpu6502_Internal_Flow_s_hpp */
