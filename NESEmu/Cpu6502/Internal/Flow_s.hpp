//
//  Flow_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Flow_s_hpp
#define Cpu6502_Internal_Flow_s_hpp


template <class TConfiguration>
void Chip<TConfiguration>::jmpAbs0() {
    _currentInstruction = &Chip::jmpAbs1;
    
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::jmpAbs1() {
    _currentInstruction = &Chip::jmpAbs2;
    
    absolute1();
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::jmpAbs2() {
    // Set program counter
    _programCounterLow = _alu.getAdderHold();
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

template <class TConfiguration>
void Chip<TConfiguration>::jmpInd0() {
    _currentInstruction = &Chip::jmpInd1;
    
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::jmpInd1() {
    _currentInstruction = &Chip::jmpInd2;
    
    absolute1();
}

template <class TConfiguration>
void Chip<TConfiguration>::jmpInd2() {
    _currentInstruction = &Chip::jmpInd3;
    
    // Read address low (need to be put before ALU operation below because it need adderHold before incrementation)
    readDataBus(_alu.getAdderHold(), _inputDataLatch);
    
    // Adding 1 to address low using ALU (Add 0 with carry set like true 6502)
    _alu.performSum<DecimalSupported, false>(0x0, _alu.getAdderHold(), false, true);
}

template <class TConfiguration>
void Chip<TConfiguration>::jmpInd3() {
    _currentInstruction = &Chip::jmpInd4;
    
    // Read address high (need to be put before ALU operation below because it need adderHold before saving low byte of address)
    readDataBus(_alu.getAdderHold(), _addressBusHigh);
    
    // 6502 uses the ALU to store temporary low byte of address (by adding 0 to it to keep its value in adderHold)
    _alu.performSum<DecimalSupported, false>(0x0, _inputDataLatch, false, false);
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::jmpInd4() {
    // Set address low in PC
    _programCounterLow = _alu.getAdderHold();
    
    // Set address high in PC
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

template <class TConfiguration>
void Chip<TConfiguration>::jsr0() {
    _currentInstruction = &Chip::jsr1;
    
    // Fetch ADL
    fetchData();
}

template <class TConfiguration>
void Chip<TConfiguration>::jsr1() {
    _currentInstruction = &Chip::jsr2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    
    // Store address low in stack pointer
    _stackPointer = _inputDataLatch;
}

template <class TConfiguration>
void Chip<TConfiguration>::jsr2() {
    _currentInstruction = &Chip::jsr3;
    
    // Push PCH to stack
    pushToStack0(_programCounterHigh);
}

template <class TConfiguration>
void Chip<TConfiguration>::jsr3() {
    _currentInstruction = &Chip::jsr4;
    
    // Finish stack operation and push PCL to stack
    pushToStack1();
    pushToStack0(_programCounterLow);
}

template <class TConfiguration>
void Chip<TConfiguration>::jsr4() {
    _currentInstruction = &Chip::jsr5;
    
    // Don't finish stack operation because stack pointer is used to store address low and addressBusLow is set with PC to fetch address high
    
    // Read address high
    fetchData();
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::jsr5() {
    // Set program counter (address low was stored in stackPointer and address high was fetched in last cycle)
    _programCounterLow = _stackPointer;
    _programCounterHigh = _inputDataLatch;
    
    // Restore stack pointer
    _stackPointer = _alu.getAdderHold();
    
    fetchOpcode();
}

template <class TConfiguration>
void Chip<TConfiguration>::nop0() {
    _currentInstruction = &Chip::fetchOpcode;
    implied();
}

template <class TConfiguration>
void Chip<TConfiguration>::rts0() {
    _currentInstruction = &Chip::rts1;
    
    // Even if it's a implied addressing mode, PC is incremented when reading memory
    fetchData();
}

template <class TConfiguration>
void Chip<TConfiguration>::rts1() {
    _currentInstruction = &Chip::rts2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    pullFromStack0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rts2() {
    _currentInstruction = &Chip::rts3;
    
    pullFromStack1();
    pullFromStack0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rts3() {
    _currentInstruction = &Chip::rts4;
    
    pullFromStack1();
    stopStackOperation();
    
    // 6502 uses the ALU to store temporary low byte of address (by adding 0 to it to keep its value in adderHold)
    _alu.performSum<DecimalSupported, false>(0x0, _inputDataLatch, false, false);
}

template <class TConfiguration>
void Chip<TConfiguration>::rts4() {
    _currentInstruction = &Chip::fetchOpcode;
    
    // Set program counter (address low was stored in alu and address high was fetched in last cycle)
    _programCounterLow = _alu.getAdderHold();
    _programCounterHigh = _inputDataLatch;
    
    // Fetch data to increment PC (to pass last byte of JSR operand)
    fetchData();
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::unofficial() {  // TODO: a retirer une fois tous les opcodes gérés
    nop0();
}

#endif /* Cpu6502_Internal_Flow_s_hpp */
