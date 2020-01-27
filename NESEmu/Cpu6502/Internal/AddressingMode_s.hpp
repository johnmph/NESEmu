//
//  AddressingMode_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_AddressingMode_s_hpp
#define Cpu6502_Internal_AddressingMode_s_hpp


template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::implied() {
    // In implied addressing mode, there is a unused read which doesn't increment PC (but we need to do this to update address bus with current PC incremented by last step)
    readDataBus(_programCounterLow, _programCounterHigh);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::immediate() {
    // Read one data and increment PC
    fetchData();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absolute0() {
    // Read low byte of address and increment PC
    fetchData();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absolute1() {
    // 6502 uses the ALU to store temporary low byte of address (by adding 0 to it to keep its value in adderHold)
    absoluteIndexed1(0x0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absoluteLoad() {
    readDataBus(_alu.getAdderHold(), _inputDataLatch);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absoluteStore(uint8_t data) {
    writeDataBus(_alu.getAdderHold(), _inputDataLatch, data);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPage() {
    // Read low byte of address and increment PC
    fetchData();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageLoad() {
    readDataBus(_inputDataLatch, 0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageStore(uint8_t data) {
    writeDataBus(_inputDataLatch, 0, data);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::relative0() {
    // Read offset and increment PC
    fetchData();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::relative1(bool condition) { // TODO: voir les interruptions avec les opcode de branchement
    if (condition == true) {
        // Adding offset with programCounterLow using ALU
        _alu.performSum<BDecimalSupported, false>(_inputDataLatch, _programCounterLow, false, false);
        
        // Branch is taken, this data will not be used, we don't increment PC
        readDataBus(_programCounterLow, _programCounterHigh);
        
        // Set next instruction
        _currentInstruction = &Chip::relativeBranch0;
        
        return;
    }
    
    // Branch is not taken, fetch next opcode
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::relativeBranch0() {
    _programCounterLow = _alu.getAdderHold();
    
    // bInput must be programCounterLow and aInput the offset
    if ((_alu.getBInput() ^ _alu.getAInput()) & (_alu.getBInput() ^ _alu.getAdderHold()) & 0x80) {
        // There is a page boundary cross, this data will not be used (because its not the good address), we don't increment PC
        readDataBus(_programCounterLow, _programCounterHigh);
        
        // Correct PCH in next instruction
        _currentInstruction = &Chip::relativeBranch1;
        
        return;
    }
    
    // There is no page boundary cross, so it's the correct data, we can fetch opcode (but we don't use fetchOpcode because there is a bug? in taken branch instructions (without carry) that delay the possible interrupt execution after executing the next instruction
    // See http://forum.6502.org/viewtopic.php?f=4&t=1634
    // And http://forums.nesdev.com/viewtopic.php?t=6510
    // And http://wiki.nesdev.com/w/index.php/CPU_interrupts
    //fetchOpcode();
    fetchData();                                // TODO: soit on fait ainsi mais alors si le signal d'interruption est arreté avant la fin de l'instruction suivante, l'interruption ne sera pas executée car pas détectée (seulement pour IRQ, le nmi est detecté), si ce n'est pas ainsi en reel, il faut aussi faire un checkInterrupt et setter le flag _interruptRequested ici : normalement on s'en fout car le NMI est sauvé et l'IRQ de toutes facons doit etre actif au moment du fetchOpcode qui va detecter l'interruption
    _currentInstruction = &Chip::decodeOpcodeAndExecuteInstruction;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::relativeBranch1() {
    // Correct programCounterHigh
    if (_alu.getBInput() & 0x80) {
        ++_programCounterHigh;
    }
    else {
        --_programCounterHigh;
    }
    
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absoluteIndexed0() {
    absolute0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absoluteIndexed1(uint8_t index) {
    // Read high byte of address and increment PC
    fetchData();
    
    // Adding index with inputDataLatch using ALU
    _alu.performSum<BDecimalSupported, false>(index, _inputDataLatch, false, false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absoluteIndexedX1() {
    absoluteIndexed1(_xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absoluteIndexedY1() {
    absoluteIndexed1(_yIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absoluteIndexedLoad0(OpcodeInstruction nextInstruction) {
    // If we don't need to correct inputDataLatch (address high), skip next instruction
    if (_alu.getCarry() == false) {
        _currentInstruction = nextInstruction;
    }
    
    // Load data
    readDataBus(_alu.getAdderHold(), _inputDataLatch);
    
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502) if necessary
    _alu.performSum<BDecimalSupported, false>(0x0, _inputDataLatch, false, _alu.getCarry());
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absoluteIndexedLoad1() {
    readDataBus(_addressBusLow, _alu.getAdderHold());
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absoluteIndexedStore0() {
    // Load data
    readDataBus(_alu.getAdderHold(), _inputDataLatch);
    
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502) if necessary
    _alu.performSum<BDecimalSupported, false>(0x0, _inputDataLatch, false, _alu.getCarry());
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::absoluteIndexedStore1(uint8_t data) {
    writeDataBus(_addressBusLow, _alu.getAdderHold(), data);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndexed0() {
    zeroPage();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndexed1(uint8_t index) {
    zeroPageLoad();
    
    // Adding index with inputDataLatch using ALU
    _alu.performSum<BDecimalSupported, false>(index, _inputDataLatch, false, false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndexedX1() {
    zeroPageIndexed1(_xIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndexedY1() {
    zeroPageIndexed1(_yIndex);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndexedLoad() {
    readDataBus(_alu.getAdderHold(), 0);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndexedStore(uint8_t data) {
    writeDataBus(_alu.getAdderHold(), 0, data);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPagePreIndexedIndirect0() {
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPagePreIndexedIndirect1() {
    zeroPageIndexedX1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPagePreIndexedIndirect2() {
    // Read low byte of address
    zeroPageIndexedLoad();
    
    // Adding 1 with adderHold using ALU (Add 0 with carry set like true 6502)
    _alu.performSum<BDecimalSupported, false>(0x0, _alu.getAdderHold(), false, true);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPagePreIndexedIndirect3() {
    // Read high byte of address
    zeroPageIndexedLoad();
    
    // 6502 uses the ALU to store temporary low byte of address (by adding 0 to it to keep its value in adderHold)
    _alu.performSum<BDecimalSupported, false>(0x0, _inputDataLatch, false, false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPagePreIndexedIndirectLoad() {
    absoluteLoad();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPagePreIndexedIndirectStore(uint8_t data) {
    absoluteStore(data);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndirectPostIndexed0() {
    zeroPageIndexed0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndirectPostIndexed1() {
    // Read low byte of address
    zeroPageLoad();
    
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502)
    _alu.performSum<BDecimalSupported, false>(0x0, _inputDataLatch, false, true);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndirectPostIndexed2() {
    // Read high byte of address
    zeroPageIndexedLoad();
    
    // Adding yIndex with inputDataLatch using ALU
    _alu.performSum<BDecimalSupported, false>(_yIndex, _inputDataLatch, false, false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndirectPostIndexedLoad0(OpcodeInstruction nextInstruction) {
    absoluteIndexedLoad0(nextInstruction);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndirectPostIndexedLoad1() {
    absoluteIndexedLoad1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndirectPostIndexedStore0() {
    absoluteIndexedStore0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::zeroPageIndirectPostIndexedStore1(uint8_t data) {
    absoluteIndexedStore1(data);
}

#endif /* Cpu6502_Internal_AddressingMode_s_hpp */
