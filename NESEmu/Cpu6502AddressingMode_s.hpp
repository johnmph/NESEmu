//
//  Cpu6502AddressingMode_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502AddressingMode_s_hpp
#define Cpu6502AddressingMode_s_hpp


template <class TBus>
void Cpu6502<TBus>::implied() {
    // In implied addressing mode, there is a unused read which doesn't increment PC
    readDataBus(_programCounterLow, _programCounterHigh);//TODO: normalement on peut ne rien avoir car juste avant ca avait deja fait ca
}

template <class TBus>
void Cpu6502<TBus>::immediate() {
    // Read one data and increment PC
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::absolute0() {
    // Read low byte of address and increment PC
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::absolute1() {
    // Read high byte of address and increment PC
    fetchData();
    
    // Store low byte of address in addressBusLowRegister
    _addressBusLowRegister = _inputDataLatch;
}

template <class TBus>
void Cpu6502<TBus>::absoluteLoad() {
    readDataBus(_addressBusLowRegister, _inputDataLatch);
}

template <class TBus>
void Cpu6502<TBus>::absoluteStore(uint8_t data) {
    writeDataBus(_addressBusLowRegister, _inputDataLatch, data);
}

template <class TBus>
void Cpu6502<TBus>::zeroPage() {
    // Read low byte of address and increment PC
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageLoad() {
    readDataBus(_inputDataLatch, 0);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageStore(uint8_t data) {
    writeDataBus(_inputDataLatch, 0, data);
}

template <class TBus>
void Cpu6502<TBus>::relative() {
    // Read offset and increment PC
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::relativeBranch0() {    // TODO: a voir (surtout pour le test du ++PC et --PC)
    _programCounterLow = _adderHold;
    
    // bInput must be programCounterLow and aInput the offset
    if ((_bInput ^ _aInput) & (_bInput ^ _adderHold) & 0x80) {
        // There is a page boundary cross, this data will not be used (because its not the good address), we don't increment PC
        readDataBus(_programCounterLow, _programCounterHigh);
        
        // Correct programCounterHigh
        if (_bInput & 0x80) {
            ++_programCounterHigh;
        }
        else {
            --_programCounterHigh;
        }
        
        return;
    }
    
    // There is no page boundary cross, so it's the correct data, we can fetch opcode
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::relativeBranch1() {
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexed0() {
    absolute0();
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexed1(uint8_t index) {
    absolute0();
    
    // Adding index with inputDataLatch using ALU
    _aInput = index;
    _bInput = _inputDataLatch;
    aluPerformSum(false, false);
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedX1() {
    absoluteIndexed1(_xIndex);
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedY1() {
    absoluteIndexed1(_yIndex);
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedLoad0(InstructionPipeline nextInstruction) {
    // Load data
    readDataBus(_adderHold, _inputDataLatch);
    
    // If we need to correct inputDataLatch (address high)
    if (_aluCarry == true) {
        // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502)
        _aInput = 0;
        _bInput = _inputDataLatch;
        aluPerformSum(false, true);
        
        // Need the extra step to correct address high
        return;
    }
    
    // No need of extra step because we already have the correct data (inputDataLatch was correct)
    _currentInstruction = nextInstruction;
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedLoad1() {
    readDataBus(_addressBusLow, _adderHold);
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedStore0() {
    // Load data
    readDataBus(_adderHold, _inputDataLatch);
    
    // If we need to correct inputDataLatch
    if (_aluCarry == true) {
        // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502)
        _aInput = 0;
        _bInput = _inputDataLatch;
        aluPerformSum(false, true);
    }
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedStore1(uint8_t data) {
    writeDataBus(_addressBusLow, _adderHold, data);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndexed0() {
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndexed1(uint8_t index) {
    zeroPageLoad();
    
    // Adding index with inputDataLatch using ALU
    _aInput = index;
    _bInput = _inputDataLatch;
    aluPerformSum(false, false);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndexedX1() {
    zeroPageIndexed1(_xIndex);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndexedY1() {
    zeroPageIndexed1(_yIndex);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndexedLoad() {
    readDataBus(_adderHold, 0);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndexedStore(uint8_t data) {
    writeDataBus(_adderHold, 0, data);
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirect0() {
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirect1() {
    zeroPageIndexedX1();
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirect2() {
    // Read low byte of address
    zeroPageIndexedLoad();
    
    // Adding 1 with adderHold using ALU (Add 0 with carry set like true 6502)
    _aInput = 0;
    _bInput = _adderHold;
    aluPerformSum(false, true);
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirect3() {
    // Read high byte of address
    zeroPageIndexedLoad();
    
    // Store low byte of address in addressBusLowRegister
    _addressBusLowRegister = _inputDataLatch;
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirectLoad() {
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirectStore(uint8_t data) {
    absoluteStore(data);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed0() {
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed1() {
    // Read low byte of address
    zeroPageLoad();
    
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502)
    _aInput = 0;
    _bInput = _inputDataLatch;
    aluPerformSum(false, true);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed2() {
    // Read high byte of address
    zeroPageIndexedLoad();
    
    // Adding yIndex with inputDataLatch using ALU
    _aInput = _yIndex;
    _bInput = _inputDataLatch;
    aluPerformSum(false, false);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexedLoad0(InstructionPipeline nextInstruction) {
    absoluteIndexedLoad0(nextInstruction);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexedLoad1() {
    absoluteIndexedLoad1();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexedStore0() {
    absoluteIndexedStore0();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexedStore1(uint8_t data) {
    absoluteIndexedStore1(data);
}

#endif /* Cpu6502AddressingMode_s_hpp */
