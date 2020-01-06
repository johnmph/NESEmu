//
//  Cpu6502_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 31/12/19.
//  Copyright © 2019 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_s_hpp
#define Cpu6502_s_hpp

// TODO: par apres, si assez rapide, decomposer en 2 phases de cycles (Ph0, Ph1, Ph2, RDY, R/W, Sync)
// TODO: tester avec les read modify write instruction pour voir si ok
#include "Cpu6502_s_data.hpp"


// Public interface

template <class TBus>
Cpu6502<TBus>::Cpu6502(TBus &bus) : _bus(bus), _programCounterLow(0xFF), _programCounterHigh(0), _stackPointer(0), _accumulator(0xAA), _xIndex(0), _yIndex(0), _statusFlags(0x2), _resetRequested(false), _nmiLine(true), _nmiLinePrevious(true), _nmiRequested(false), _irqLine(true), _irqRequested(false), _interruptRequested(false) {
    reset(false);
}

template <class TBus>
void Cpu6502<TBus>::clock() {
    // Interrupts are checked each clock
    checkNmi();
    checkIrq();
    
    // Decode current instruction
    if (_pipelineStep == 0) {
        decodeOpcode();
    }
    
    // Execute current stage
    (this->*_instrPipelineFuncs[_instrPipelineStartIndex + _pipelineStep])();
    ++_pipelineStep;
}

template <class TBus>
void Cpu6502<TBus>::reset(bool high) {  // TODO: voir pour les registres si on les laisse ainsi ou si on les reset : normalement on les laisse ainsi puisqu'un reset c'est juste un signal comme une interruption (mais a voir, on ne sait jamais)
    // Save signal
    _resetLine = high;
    
    // If reset
    if ((_resetLine == false) && (_resetRequested == false)) {
        // Trick to avoid calling decodeOpcode in next clock but execute start "opcode"
        _instrPipelineStartIndex = -1;
        _pipelineStep = 1;
        _resetRequested = true;
        _interruptRequested = true; // TODO: voir si ok pour reset aussi
    }
}

template <class TBus>
void Cpu6502<TBus>::nmi(bool high) {    // TODO: a terminer/tester (il parait qu'il faut 2 cycles pour que le signal low soit pris en compte, a voir)
    // Save signal
    _nmiLine = high;
}

template <class TBus>
void Cpu6502<TBus>::irq(bool high) {    // TODO: a tester
    // Save signal
    _irqLine = high;
}

template <class TBus>
uint16_t Cpu6502<TBus>::getProgramCounter() {
    return (_programCounterHigh << 8) | _programCounterLow;
}

template <class TBus>
uint16_t Cpu6502<TBus>::getAddressBus() {
    return (_addressBusHigh << 8) | _addressBusLow;
}

// Status

template <class TBus>
constexpr uint8_t Cpu6502<TBus>::getStatusFlagsEnableMask(std::initializer_list<Flags> const &flags) {
    uint8_t mask = 0x0;
    
    for (auto const &flag : flags) {
        mask |= 1 << static_cast<uint8_t>(flag);
    }
    
    return mask;
}

template <class TBus>
constexpr uint8_t Cpu6502<TBus>::getStatusFlagsDisableMask(std::initializer_list<Flags> const &flags) {
    /*uint8_t mask = 0xFF;
    
    for (auto const &flag : flags) {
        mask &= ~(1 << static_cast<uint8_t>(flag));
    }
    
    return mask;*/
    return ~getStatusFlagsEnableMask(flags);
}

template <class TBus>
constexpr void Cpu6502<TBus>::clearStatusFlags(std::initializer_list<Flags> const &flags) {
    _statusFlags &= getStatusFlagsDisableMask(flags);
}

template <class TBus>
constexpr bool Cpu6502<TBus>::getStatusFlag(Flags flag) {
    return _statusFlags & getStatusFlagsEnableMask({ flag });
}

template <class TBus>
constexpr void Cpu6502<TBus>::setStatusFlag(Flags flag, bool value) {
    // Be careful, if value can be false, we need to call clearStatusFlags before (It's for multiple clear/set flags optimization by avoiding if statement)
    _statusFlags |= value << static_cast<uint8_t>(flag);
}

// Program flow

template <class TBus>
void Cpu6502<TBus>::incrementProgramCounter() {
    ++_programCounterLow;
    _programCounterHigh += (_programCounterLow == 0);
}

template <class TBus>
void Cpu6502<TBus>::readDataBus(uint8_t low, uint8_t high) {
    _inputDataLatch = _bus.read((high << 8) | low);
    _predecode = _inputDataLatch;   // TODO: voir si ok de deplacer ca dans fetchOpcode (apres le else fetchData())
}

template <class TBus>
void Cpu6502<TBus>::writeDataBus(uint8_t low, uint8_t high) {
    _bus.write((high << 8) | low, _dataOutput);
}

template <class TBus>
void Cpu6502<TBus>::fetchData() {
    readDataBus(_programCounterLow, _programCounterHigh);
    incrementProgramCounter();
}

template <class TBus>
void Cpu6502<TBus>::fetchOpcode() {
    // If interrupt requested
    if (checkInterrupts() == true) {    //TODO: voir si la gestion des interruptions ici ou dans decodeOpcode
        // Read opcode without increment PC
        readDataBus(_programCounterLow, _programCounterHigh);   // TODO: voir si ok
        
        // Brk
        _predecode = 0;
        
        // Save interrupt flag to know that an interrupt is occur (and not brk opcode)
        _interruptRequested = true;
    } else {
        fetchData();
    }
    
    _pipelineStep = -1;
}

template <class TBus>
void Cpu6502<TBus>::decodeOpcode() {
    _instruction = _predecode;
    _instrPipelineStartIndex = _instrPipelineStartIndexes[_instruction];
}

template <class TBus>
void Cpu6502<TBus>::finishInstruction() {
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::branch(bool condition) {    // TODO: voir les interruptions avec les opcode de branchement (et voir aussi CLI SEI et PLP)
    // Adding offset with programCounterLow using ALU
    _aInput = _inputDataLatch;
    _bInput = _programCounterLow;
    aluPerformSum(false, false);
    
    if (condition == true) {
        // Branch is taken, this data will not be used
        fetchData();
        return;
    }
    
    // Branch is not taken, read next instruction
    finishInstruction();
}

// Interrupts

template <class TBus>
void Cpu6502<TBus>::checkNmi() {
    // Nmi is requested if nmi line has transition from high to low
    if ((_nmiLinePrevious == true) && (_nmiLine == false)) {
        _nmiRequested = true;
    }
    
    // Save previous nmi signal
    _nmiLinePrevious = _nmiLine;
}

template <class TBus>
void Cpu6502<TBus>::checkIrq() {
    // Irq is requested if interrupts are not disabled and if irq line goes to low (and stay for one cycle)
    _irqRequested = (getStatusFlag(Flags::InterruptDisable) == false) && (_irqLine == false);
}

template <class TBus>
bool Cpu6502<TBus>::checkInterrupts() {
    // If Nmi requested
    if (_nmiRequested == true) {
        return true;
    }
    
    // If Irq requested
    if (_irqRequested == true) {
        return true;
    }
    
    return false;
}

template <class TBus>
int Cpu6502<TBus>::getCurrentInterruptVectorsIndex() {
    // If reset
    if (_resetRequested == true) {
        return static_cast<int>(Interrupts::Reset);
    }
    
    // If nmi
    if (_nmiRequested == true) {
        return static_cast<int>(Interrupts::Nmi);
    }
    
    // Irq/Brk
    return static_cast<int>(Interrupts::IrqBrk);
}

// Addressing modes

template <class TBus>
void Cpu6502<TBus>::implied() {
    // In implied addressing mode, there is a unused read which doesn't increment PC
    readDataBus(_programCounterLow, _programCounterHigh);
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
    // Store low byte of address in addressBusLow
    _addressBusLow = _inputDataLatch;
    
    // Read high byte of address and increment PC
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::absoluteLoad() {
    readDataBus(_addressBusLow, _inputDataLatch);
}

template <class TBus>
void Cpu6502<TBus>::absoluteStore() {
    writeDataBus(_addressBusLow, _inputDataLatch);
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
void Cpu6502<TBus>::zeroPageStore() {
    writeDataBus(_inputDataLatch, 0);
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
    
    // There is no page boundary cross, so it's the correct data, we can fetch the instruction
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
    absolute1();
    
    // Adding index with addressBusLow using ALU
    _aInput = index;
    _bInput = _addressBusLow;
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
void Cpu6502<TBus>::absoluteIndexedLoad0() {
    _addressBusLow = _adderHold;
    
    // If we need to correct inputDataLatch
    if (_aluCarry == true) {
        // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502)
        _aInput = 0;
        _bInput = _inputDataLatch;
        aluPerformSum(false, true);
        
        // Load unused data (because address is incorrect)
        absoluteLoad();
        return;
    }
    
    // Load data
    absoluteLoad();
    
    // No need of extra step because we already have the correct data (inputDataLatch was correct)
    ++_pipelineStep;
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedLoad1() {
    readDataBus(_addressBusLow, _adderHold);
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedStore0() {
    _addressBusLow = _adderHold;
    
    // If we need to correct inputDataLatch
    if (_aluCarry == true) {
        // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502)
        _aInput = 0;
        _bInput = _inputDataLatch;
        aluPerformSum(false, true);
    }
    
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedStore1() {
    writeDataBus(_addressBusLow, _adderHold);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndexed0() {
    zeroPage();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndexed1(uint8_t index) {
    // Adding index with inputDataLatch using ALU
    _aInput = index;
    _bInput = _inputDataLatch;
    aluPerformSum(false, false);
    
    zeroPageLoad();
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
void Cpu6502<TBus>::zeroPageIndexedStore() {
    writeDataBus(_adderHold, 0);
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
    // Adding 1 with adderHold using ALU (Add 0 with carry set like true 6502)
    _aInput = 0;
    _bInput = _adderHold;
    aluPerformSum(false, true);
    
    // Read low byte of address
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirect3() {
    // Store low byte of address in addressBusLow
    _addressBusLow = _inputDataLatch;
    
    // Read high byte of address
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirectLoad() {
    absoluteLoad();
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirectStore() {
    absoluteStore();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed0() {
    zeroPageIndexed0();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed1() {
    // Adding 1 with inputDataLatch using ALU (Add 0 with carry set like true 6502)
    _aInput = 0;
    _bInput = _inputDataLatch;
    aluPerformSum(false, true);
    
    // Read low byte of address
    zeroPageLoad();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed2() {
    // Adding yIndex with inputDataLatch using ALU
    _aInput = _yIndex;
    _bInput = _inputDataLatch;
    aluPerformSum(false, false);
    
    // Read high byte of address
    zeroPageIndexedLoad();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexedLoad0() {
    absoluteIndexedLoad0();
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
void Cpu6502<TBus>::zeroPageIndirectPostIndexedStore1() {
    absoluteIndexedStore1();
}

// Stack

template <class TBus>
void Cpu6502<TBus>::pushToStackReset(uint8_t data) {    // TODO: normalement c'est implementé via le signal R/W mais c'est trop bas niveau
    _dataOutput = data;
    readDataBus(_stackPointer, _stackPageNumber);
    
    --_stackPointer;
}

template <class TBus>
void Cpu6502<TBus>::pushToStack(uint8_t data) {
    _dataOutput = data;
    writeDataBus(_stackPointer, _stackPageNumber);
    
    --_stackPointer;
}

template <class TBus>
void Cpu6502<TBus>::pullFromStack() {
    ++_stackPointer;
    
    readDataBus(_stackPointer, _stackPageNumber);
}

// ALU

template <class TBus>
void Cpu6502<TBus>::aluInvertBInput() {
    _bInput = ~_bInput;
}

template <class TBus>
void Cpu6502<TBus>::aluPerformSum(bool decimalEnable, bool carryIn) {//TODO: terminer (halfCarry, decimalEnable)
    uint16_t add = _aInput + _bInput + carryIn;
    _adderHold = static_cast<uint8_t>(add);
    
    _aluOverflow = (_aInput ^ _adderHold) & (_bInput ^ _adderHold) & 0x80;
    _aluCarry = add & 0x100;
    _aluHalfCarry = false;  // TODO: voir ce que c'est et si c'est utilisé
}

template <class TBus>
void Cpu6502<TBus>::aluPerformAnd() {//TODO: terminer
    _adderHold = _aInput & _bInput;
    
    //_aluOverflow = (_aInput ^ _adderHold) & (_bInput ^ _adderHold) & 0x80;
    //_aluCarry = false;
    //_aluHalfCarry = false;  // TODO: voir ce que c'est et si c'est utilisé
}

template <class TBus>
void Cpu6502<TBus>::aluPerformOr() {//TODO: terminer
    _adderHold = _aInput | _bInput;
    
    //_aluOverflow = (_aInput ^ _adderHold) & (_bInput ^ _adderHold) & 0x80;
    //_aluCarry = false;
    //_aluHalfCarry = false;  // TODO: voir ce que c'est et si c'est utilisé
}

template <class TBus>
void Cpu6502<TBus>::aluPerformEor() {//TODO: terminer
    _adderHold = _aInput ^ _bInput;
    
    //_aluOverflow = (_aInput ^ _adderHold) & (_bInput ^ _adderHold) & 0x80;
    //_aluCarry = false;
    //_aluHalfCarry = false;  // TODO: voir ce que c'est et si c'est utilisé
}

template <class TBus>
void Cpu6502<TBus>::aluPerformShiftRight(bool carryIn) {//TODO: terminer
    _adderHold = (_aInput >> 1) | (carryIn << 7);
    
    //_aluOverflow = (_aInput ^ _adderHold) & (_bInput ^ _adderHold) & 0x80;  // TODO: voir ici et pour les autres aussi
    _aluCarry = _aInput & 0x1;
    //_aluHalfCarry = false;  // TODO: voir ce que c'est et si c'est utilisé
}

// Instructions

template <class TBus>
void Cpu6502<TBus>::startLow() {
    readDataBus(_programCounterLow, _programCounterHigh);
    
    // If reset line stays low, continue this step
    if (_resetLine == false) {
        --_pipelineStep;
    }
}

template <class TBus>
void Cpu6502<TBus>::startHigh() {
    readDataBus(_programCounterLow, _programCounterHigh);
}

template <class TBus>
void Cpu6502<TBus>::brk0() {    // TODO: a voir
    // Read data without increment PC for reset, nmi and irq
    if (_interruptRequested == true) {
        readDataBus(_programCounterLow, _programCounterHigh);
        return;
    }
    
    // Read data and increment PC for brk
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::brk1() {
    // In reset, write is disabled
    if (_resetRequested == true) {
        pushToStackReset(_programCounterHigh);
        return;
    }
    
    pushToStack(_programCounterHigh);
}

template <class TBus>
void Cpu6502<TBus>::brk2() {
    // In reset, write is disabled
    if (_resetRequested == true) {
        pushToStackReset(_programCounterLow);
        return;
    }
    
    pushToStack(_programCounterLow);
}

template <class TBus>
void Cpu6502<TBus>::brk3() {
    uint8_t data = _statusFlags | ((!_interruptRequested) << static_cast<int>(Flags::Break));   // TODO: voir si c'est bon
    
    // In reset, write is disabled
    if (_resetRequested == true) {
        pushToStackReset(data);
        return;
    }
    
    pushToStack(data);
}

template <class TBus>
void Cpu6502<TBus>::brk4() {
    // Disable interrupts
    setStatusFlag(Flags::InterruptDisable, true);
    
    // Calculate interrupt vectors index
    _interruptVectorsIndex = getCurrentInterruptVectorsIndex();
    
    // Read low byte of address
    readDataBus(_interruptVectors[_interruptVectorsIndex][0], _interruptVectors[_interruptVectorsIndex][1]);
}

template <class TBus>
void Cpu6502<TBus>::brk5() {
    // Store low byte of address to programCounterLow
    _programCounterLow = _inputDataLatch;
    
    // Read high byte of address
    readDataBus(_interruptVectors[_interruptVectorsIndex][0] + 1, _interruptVectors[_interruptVectorsIndex][1]);
}

template <class TBus>
void Cpu6502<TBus>::brk6() {
    // Store high byte of address to programCounterHigh
    _programCounterHigh = _inputDataLatch;
    
    // TODO: voir si ok : (car il faut voir si c'est possible d'avoir la detection d'une interruption dans le stage du brk)
    _nmiRequested = false;
    _resetRequested = false;
    _interruptRequested = false;
    
    // Fetch instruction
    finishInstruction();
}


template <class TBus>
void Cpu6502<TBus>::ora() {         // TODO: ca doit etre ainsi mais comment ca se fait qu'on ait directement accès au resultat de l'ALU ???
    _aInput = _accumulator;
    _bInput = _inputDataLatch;
    aluPerformOr();
    
    // Store ALU result in accumulator
    _accumulator = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Zero, Flags::Negative });     // TODO: par apres si beaucoup d'instructions utilisent ca, avoir une methode setZeroNegative(data)
    setStatusFlag(Flags::Zero, (_accumulator == 0));
    setStatusFlag(Flags::Negative, (_accumulator & 0x80));
    
    finishInstruction();
}

template <class TBus>
void Cpu6502<TBus>::asl0(uint8_t data) {
    // ASL by adding same number to itself
    _aInput = data;
    _bInput = data;
    aluPerformSum(false, false);
    
    implied();  // TODO: voir car pas dans tous les modes d'adressages, voir aussi les modes d'adressages car ils devront sauver leur adresse plutot que d'utiliser inputDataLatch, adderHold, ... car dans les operations on va les modifier !!! (+ implied charge a partir de l'adresse du ogram counter, dans les autres modes d'adressages, d'ou ca charge ? -> de la derniere adresse)
}

template <class TBus>
void Cpu6502<TBus>::asl1(uint8_t &data) {
    data = _adderHold;
    
    // Update status
    clearStatusFlags({ Flags::Carry, Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Carry, data);
    setStatusFlag(Flags::Zero, (data == 0));
    setStatusFlag(Flags::Negative, (data & 0x80));
}

template <class TBus>
void Cpu6502<TBus>::aslAccumulator0() {
    asl0(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::aslAccumulator1() {
    asl1(_accumulator);
    finishInstruction();
}

template <class TBus>
void Cpu6502<TBus>::asl0() {
    asl0(_inputDataLatch);
}

template <class TBus>
void Cpu6502<TBus>::asl1ZeroPage() {
    asl1(_dataOutput);
    zeroPageStore();
}// TODO: + finishInstruction mais dans le step d'apres

template <class TBus>
void Cpu6502<TBus>::asl1ZeroPageIndexed() {
    asl1(_dataOutput);
    zeroPageIndexedStore();
}// TODO: + finishInstruction mais dans le step d'apres

template <class TBus>
void Cpu6502<TBus>::asl1Absolute() {
    asl1(_dataOutput);
    absoluteStore();
}// TODO: + finishInstruction mais dans le step d'apres

template <class TBus>
void Cpu6502<TBus>::asl1AbsoluteIndexed() {
    asl1(_dataOutput);
    absoluteIndexedStore0();
}// TODO: + absoluteIndexedStore1 et finishInstruction mais dans le step d'apres



template <class TBus>
void Cpu6502<TBus>::clv() {
    clearStatusFlags({ Flags::Overflow });
    
    finishInstruction();
}

template <class TBus>
void Cpu6502<TBus>::lda() {
    _accumulator = _inputDataLatch;
    
    clearStatusFlags({ Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Zero, _accumulator == 0);
    setStatusFlag(Flags::Negative, _accumulator & 0x80);
    
    finishInstruction();
}

template <class TBus>
void Cpu6502<TBus>::staAbsolute() {
    sta();
    absoluteStore();
}// TODO: + finishInstruction mais dans le step d'apres

template <class TBus>
void Cpu6502<TBus>::staZeroPage() {
    sta();
    zeroPageStore();
}// TODO: + finishInstruction mais dans le step d'apres

template <class TBus>
void Cpu6502<TBus>::staAbsoluteIndexed() {
    sta();
    absoluteIndexedStore0();
}// TODO: + absoluteIndexedStore1 et finishInstruction mais dans le step d'apres

template <class TBus>
void Cpu6502<TBus>::staZeroPageIndexed() {
    sta();
    zeroPageIndexedStore();
}// TODO: + finishInstruction mais dans le step d'apres

template <class TBus>
void Cpu6502<TBus>::sta() {
    _dataOutput = _accumulator;
}

template <class TBus>
void Cpu6502<TBus>::bcs() {
    branch(getStatusFlag(Flags::Carry) == true);
}



#endif /* Cpu6502_s_hpp */
