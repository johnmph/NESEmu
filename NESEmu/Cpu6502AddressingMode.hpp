//
//  Cpu6502AddressingMode.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502AddressingMode_hpp
#define Cpu6502AddressingMode_hpp


void implied() {
    // In implied addressing mode, there is a unused read which doesn't increment PC
    readDataBus(_programCounterLow, _programCounterHigh);//TODO: normalement on peut ne rien avoir car juste avant ca avait deja fait ca
}

void immediate() {
    // Read one data and increment PC
    fetchData();
}

void absolute0() {
    // Read low byte of address and increment PC
    fetchData();
}

void absolute1() {
    // Read high byte of address and increment PC
    fetchData();
    
    // Store low byte of address in addressBusLowRegister
    _addressBusLowRegister = _inputDataLatch;
}

void absoluteLoad() {
    readDataBus(_addressBusLowRegister, _inputDataLatch);
}

void absoluteStore(uint8_t data) {
    writeDataBus(_addressBusLowRegister, _inputDataLatch, data);
}

void zeroPage() {
    // Read low byte of address and increment PC
    fetchData();
}

void zeroPageLoad() {
    readDataBus(_inputDataLatch, 0);
}

void zeroPageStore(uint8_t data) {
    writeDataBus(_inputDataLatch, 0, data);
}

void relative() {
    // Read offset and increment PC
    fetchData();
}

void relativeBranch0() {    // TODO: a voir (surtout pour le test du ++PC et --PC)
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
    
    // There is no page boundary cross, so it's the correct data, we can fetch the instruction
    fetchOpcode();//TODO: ou finishInstruction (c'est le meme donc peut etre utilsier le meme partout)
}

void relativeBranch1() {
    fetchOpcode();//TODO: ou finishInstruction (c'est le meme donc peut etre utilsier le meme partout)
}

void absoluteIndexed0() {
    absolute0();
}

void absoluteIndexed1(uint8_t index) {
    absolute0();
    
    // Adding index with inputDataLatch using ALU
    _aInput = index;
    _bInput = _inputDataLatch;
    aluPerformSum(false, false);
}

void absoluteIndexedX1() {
    absoluteIndexed1(_xIndex);
}

void absoluteIndexedY1() {
    absoluteIndexed1(_yIndex);
}

void absoluteIndexedLoad0() {
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
    ++_pipelineStep;
}

void absoluteIndexedLoad1() {
    readDataBus(_addressBusLow, _adderHold);
}

void absoluteIndexedStore0() {
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

void absoluteIndexedStore1(uint8_t data) {
    writeDataBus(_addressBusLow, _adderHold, data);
}

void zeroPageIndexed0() {
    zeroPage();
}

void zeroPageIndexed1(uint8_t index) {
    zeroPageLoad();
    
    // Adding index with inputDataLatch using ALU
    _aInput = index;
    _bInput = _inputDataLatch;
    aluPerformSum(false, false);
}

void zeroPageIndexedX1() {
    zeroPageIndexed1(_xIndex);
}

void zeroPageIndexedY1() {
    zeroPageIndexed1(_yIndex);
}

void zeroPageIndexedLoad() {
    readDataBus(_adderHold, 0);
}

void zeroPageIndexedStore(uint8_t data) {
    writeDataBus(_adderHold, 0, data);
}

void zeroPagePreIndexedIndirect0() {
    zeroPageIndexed0();
}

void zeroPagePreIndexedIndirect1() {
    zeroPageIndexedX1();
}

void zeroPagePreIndexedIndirect2() {
    // Read low byte of address
    zeroPageIndexedLoad();
    
    // Adding 1 with adderHold using ALU (Add 0 with carry set like true 6502)
    _aInput = 0;
    _bInput = _adderHold;
    aluPerformSum(false, true);
}

void zeroPagePreIndexedIndirect3() {
    // Read high byte of address
    zeroPageIndexedLoad();
    
    // Store low byte of address in addressBusLowRegister
    _addressBusLowRegister = _inputDataLatch;
}

void zeroPagePreIndexedIndirectLoad() {
    absoluteLoad();
}

void zeroPagePreIndexedIndirectStore(uint8_t data) {
    absoluteStore(data);
}

void zeroPageIndirectPostIndexed0() {
    zeroPageIndexed0();
}

void zeroPageIndirectPostIndexed1() {
    // Read low byte of address
    zeroPageLoad();
    
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502)
    _aInput = 0;
    _bInput = _inputDataLatch;
    aluPerformSum(false, true);
}

void zeroPageIndirectPostIndexed2() {
    // Read high byte of address
    zeroPageIndexedLoad();
    
    // Adding yIndex with inputDataLatch using ALU
    _aInput = _yIndex;
    _bInput = _inputDataLatch;
    aluPerformSum(false, false);
}

void zeroPageIndirectPostIndexedLoad0() {
    absoluteIndexedLoad0();
}

void zeroPageIndirectPostIndexedLoad1() {
    absoluteIndexedLoad1();
}

void zeroPageIndirectPostIndexedStore0() {
    absoluteIndexedStore0();
}

void zeroPageIndirectPostIndexedStore1(uint8_t data) {
    absoluteIndexedStore1(data);
}

#endif /* Cpu6502AddressingMode_hpp */
