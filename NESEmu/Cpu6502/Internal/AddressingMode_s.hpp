//
//  AddressingMode_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_AddressingMode_s_hpp
#define Cpu6502_Internal_AddressingMode_s_hpp


template <class TConfiguration>
void Chip<TConfiguration>::implied() {
    // In implied addressing mode, there is a unused read which doesn't increment PC
    readDataBus(_programCounterLow, _programCounterHigh);
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::immediate() {
    // Read one data and increment PC
    fetchData();
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::absolute0() {
    // Read low byte of address and increment PC
    fetchData();
}

template <class TConfiguration>
void Chip<TConfiguration>::absolute1() {
    // 6502 uses the ALU to store temporary low byte of address (by adding 0 to it to keep its value in adderHold)
    absoluteIndexed1(0x0);
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteRead() {
    // Read data
    readDataBus(_alu.getAdderHold(), _inputDataLatch);
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteWrite(uint8_t data) {
    // Write data
    writeDataBus(_alu.getAdderHold(), _inputDataLatch, data);
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteRMWRead() {
    // Read data
    readDataBus(_alu.getAdderHold(), _inputDataLatch);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPage() {
    // Read low byte of address and increment PC
    fetchData();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageRead() {
    // Read data
    readDataBus(_inputDataLatch, 0);
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageWrite(uint8_t data) {
    // Write data
    writeDataBus(_inputDataLatch, 0, data);
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageRMWRead() {
    // Read data
    readDataBus(_inputDataLatch, 0);
}

template <class TConfiguration>
void Chip<TConfiguration>::relative0() {
    // Read offset and increment PC
    fetchData();
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::relative1(bool condition) {
    if (condition == true) {
        // Adding offset with programCounterLow using ALU
        _alu.performSum<DecimalSupported, false>(_inputDataLatch, _programCounterLow, false, false);
        
        // Branch is taken, this data will not be used, we don't increment PC
        readDataBus(_programCounterLow, _programCounterHigh);
        
        // Set next instruction
        _currentInstruction = &Chip::relativeBranch0;
        
        // There is a bug? which delay the possible nmi or cancel the possible irq execution by not checking here (just after this) the interrupts, so we don't call checkInterrupts here
        // See http://forum.6502.org/viewtopic.php?f=4&t=1634
        // And http://forums.nesdev.com/viewtopic.php?t=6510
        // And http://wiki.nesdev.com/w/index.php/CPU_interrupts
        
        return;
    }
    
    // Branch is not taken, fetch next opcode
    fetchOpcode();
}

template <class TConfiguration>
void Chip<TConfiguration>::relativeBranch0() {
    _programCounterLow = _alu.getAdderHold();
    
    // bInput must be programCounterLow and aInput the offset
    if ((_alu.getBInput() ^ _alu.getAInput()) & (_alu.getBInput() ^ _alu.getAdderHold()) & 0x80) {
        // There is a page boundary cross, this data will not be used (because its not the good address), we don't increment PC
        readDataBus(_programCounterLow, _programCounterHigh);
        
        // Correct PCH in next instruction
        _currentInstruction = &Chip::relativeBranch1;
        
        // Check interrupts
        checkInterrupts();
        
        return;
    }
    
    // Good address (no page boundary cross), fetch next opcode
    fetchOpcode();
}

template <class TConfiguration>
void Chip<TConfiguration>::relativeBranch1() {
    // Correct programCounterHigh
    if (_alu.getBInput() & 0x80) {
        ++_programCounterHigh;
    }
    else {
        --_programCounterHigh;
    }
    
    fetchOpcode();
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteIndexed0() {
    absolute0();
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteIndexed1(uint8_t index) {
    // Read high byte of address and increment PC
    fetchData();
    
    // Adding index with inputDataLatch using ALU
    _alu.performSum<DecimalSupported, false>(index, _inputDataLatch, false, false);
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteIndexedX1() {
    absoluteIndexed1(_xIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteIndexedY1() {
    absoluteIndexed1(_yIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteIndexedRead0(OpcodeInstruction nextInstruction) {
    // If we don't need to correct inputDataLatch (address high), skip next instruction
    if (_alu.getCarry() == false) {
        _currentInstruction = nextInstruction;
        
        // Check interrupts
        checkInterrupts();
    }
    
    // Read data
    readDataBus(_alu.getAdderHold(), _inputDataLatch);
    
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502) if necessary
    _alu.performSum<DecimalSupported, false>(0x0, _inputDataLatch, false, _alu.getCarry());
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteIndexedRead1() {
    // Read data
    readDataBus(_addressBusLow, _alu.getAdderHold());
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteIndexedWrite0() {
    // Read data
    readDataBus(_alu.getAdderHold(), _inputDataLatch);
    
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502) if necessary
    _alu.performSum<DecimalSupported, false>(0x0, _inputDataLatch, false, _alu.getCarry());
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteIndexedWrite1(uint8_t data) {
    writeDataBus(_addressBusLow, _alu.getAdderHold(), data);
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteIndexedRMWRead0() {
    // Read data
    readDataBus(_alu.getAdderHold(), _inputDataLatch);
    
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502) if necessary
    _alu.performSum<DecimalSupported, false>(0x0, _inputDataLatch, false, _alu.getCarry());
}

template <class TConfiguration>
void Chip<TConfiguration>::absoluteIndexedRMWRead1() {
    // Read data
    readDataBus(_addressBusLow, _alu.getAdderHold());
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndexed0() {
    zeroPage();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndexed1(uint8_t index) {
    // Read data
    readDataBus(_inputDataLatch, 0);
    
    // Adding index with inputDataLatch using ALU
    _alu.performSum<DecimalSupported, false>(index, _inputDataLatch, false, false);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndexedX1() {
    zeroPageIndexed1(_xIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndexedY1() {
    zeroPageIndexed1(_yIndex);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndexedRead() {
    // Read data
    readDataBus(_alu.getAdderHold(), 0);
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndexedWrite(uint8_t data) {
    // Write data
    writeDataBus(_alu.getAdderHold(), 0, data);
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndexedRMWRead() {
    // Read data
    readDataBus(_alu.getAdderHold(), 0);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPagePreIndexedIndirect0() {
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPagePreIndexedIndirect1() {
    zeroPageIndexedX1();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPagePreIndexedIndirect2() {
    // Read low byte of address
    readDataBus(_alu.getAdderHold(), 0);
    
    // Adding 1 with adderHold using ALU (Add 0 with carry set like true 6502)
    _alu.performSum<DecimalSupported, false>(0x0, _alu.getAdderHold(), false, true);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPagePreIndexedIndirect3() {
    // Read high byte of address
    readDataBus(_alu.getAdderHold(), 0);
    
    // 6502 uses the ALU to store temporary low byte of address (by adding 0 to it to keep its value in adderHold)
    _alu.performSum<DecimalSupported, false>(0x0, _inputDataLatch, false, false);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPagePreIndexedIndirectRead() {
    absoluteRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPagePreIndexedIndirectWrite(uint8_t data) {
    absoluteWrite(data);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPagePreIndexedIndirectRMWRead() {
    absoluteRMWRead();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndirectPostIndexed0() {
    zeroPageIndexed0();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndirectPostIndexed1() {
    // Read low byte of address
    readDataBus(_inputDataLatch, 0);
    
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502)
    _alu.performSum<DecimalSupported, false>(0x0, _inputDataLatch, false, true);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndirectPostIndexed2() {
    // Read high byte of address
    readDataBus(_alu.getAdderHold(), 0);
    
    // Adding yIndex with inputDataLatch using ALU
    _alu.performSum<DecimalSupported, false>(_yIndex, _inputDataLatch, false, false);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndirectPostIndexedRead0(OpcodeInstruction nextInstruction) {
    absoluteIndexedRead0(nextInstruction);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndirectPostIndexedRead1() {
    absoluteIndexedRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndirectPostIndexedWrite0() {
    absoluteIndexedWrite0();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndirectPostIndexedWrite1(uint8_t data) {
    absoluteIndexedWrite1(data);
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndirectPostIndexedRMWRead0() {
    absoluteIndexedRMWRead0();
}

template <class TConfiguration>
void Chip<TConfiguration>::zeroPageIndirectPostIndexedRMWRead1() {
    absoluteIndexedRMWRead1();
}

template <class TConfiguration>
void Chip<TConfiguration>::anyRMWModify(OpcodeInstruction nextInstruction, uint8_t data) {
    // Set next instruction
    _currentInstruction = nextInstruction;
    
    // Write read memory back (like true 6502)
    writeDataBus(_addressBusLow, _addressBusHigh, data);
}

template <class TConfiguration>
void Chip<TConfiguration>::anyRMWWrite(uint8_t data) {
    // Set next instruction
    _currentInstruction = &Chip::fetchOpcode;
    
    // Write result back
    writeDataBus(_addressBusLow, _addressBusHigh, data);
    
    // Check interrupts
    checkInterrupts();
}

#endif /* Cpu6502_Internal_AddressingMode_s_hpp */
