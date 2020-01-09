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
// TODO: par apres, peut etre a la place d'avoir _instruction, _instrPipelineStartIndex et _pipelineStep, avoir un _currentInstruction qui sera setté dans le decodeOpcode et a chaque fin d'instruction le setter sur l'instruction suivante (et surtout gérer les interruptions dans le decodeOpcode et pas le fetchOpcode ?)
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
    
    // Execute current stage
    (this->*_currentInstruction)();
    
    // Each clock, there is a memory access
    fetchMemory();
}

template <class TBus>
void Cpu6502<TBus>::reset(bool high) {  // TODO: voir pour les registres si on les laisse ainsi ou si on les reset : normalement on les laisse ainsi puisqu'un reset c'est juste un signal comme une interruption (mais a voir, on ne sait jamais)
    // Save signal
    _resetLine = high;
    
    // If reset
    if ((_resetLine == false) && (_resetRequested == false)) {
        _currentInstruction = &Cpu6502::reset0;
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

// Memory

template <class TBus>
void Cpu6502<TBus>::fetchMemory() {
    if ((_readWrite == static_cast<bool>(ReadWrite::Read)) || (_resetRequested == true)) {
        _inputDataLatch = _bus.read((_addressBusHigh << 8) | _addressBusLow);
        _predecode = _inputDataLatch;   // TODO: voir si ok de deplacer ca dans fetchOpcode (apres le else fetchData())
        
        return;
    }
    
    _bus.write((_addressBusHigh << 8) | _addressBusLow, _dataOutput);
}

template <class TBus>
void Cpu6502<TBus>::readDataBus(uint8_t low, uint8_t high) {
    // Write address bus
    _addressBusLow = low;
    _addressBusHigh = high;
    
    // Set R/W to read
    _readWrite = static_cast<bool>(ReadWrite::Read);
}

template <class TBus>
void Cpu6502<TBus>::writeDataBus(uint8_t low, uint8_t high, uint8_t data) {
    // Write address bus
    _addressBusLow = low;
    _addressBusHigh = high;
    
    // Write data bus
    _dataOutput = data;
    
    // Set R/W to write
    _readWrite = static_cast<bool>(ReadWrite::Write);
}

// Program flow

template <class TBus>
void Cpu6502<TBus>::incrementProgramCounter() {
    ++_programCounterLow;
    _programCounterHigh += (_programCounterLow == 0);
}

template <class TBus>
void Cpu6502<TBus>::fetchData() {
    readDataBus(_programCounterLow, _programCounterHigh);
    incrementProgramCounter();
}

template <class TBus>
void Cpu6502<TBus>::fetchOpcode(InstructionPipeline nextInstruction) {
    // Set next instruction
    _currentInstruction = nextInstruction;
    
    // If interrupt requested
    if (checkInterrupts() == true) {    //TODO: voir si la gestion des interruptions ici ou dans decodeOpcode : DANS DECODEOPCODE obligé !!!
        // Read opcode without increment PC
        readDataBus(_programCounterLow, _programCounterHigh);   // TODO: voir si ok
        
        // Brk
        _predecode = 0;
        
        // Save interrupt flag to know that an interrupt is occur (and not brk opcode)
        _interruptRequested = true;
    } else {
        fetchData();
    }
}

template <class TBus>
void Cpu6502<TBus>::fetchOpcode() {/*
    // If interrupt requested
    if (checkInterrupts() == true) {    //TODO: voir si la gestion des interruptions ici ou dans decodeOpcode : DANS DECODEOPCODE obligé !!!
        // Read opcode without increment PC
        readDataBus(_programCounterLow, _programCounterHigh);   // TODO: voir si ok
        
        // Brk
        _predecode = 0;
        
        // Save interrupt flag to know that an interrupt is occur (and not brk opcode)
        _interruptRequested = true;
    } else {
        fetchData();
    }
    
    // Decode opcode on next step
    _currentInstruction = &Cpu6502::decodeOpcode;*/
    fetchOpcode(&Cpu6502::decodeOpcode);
}

template <class TBus>
void Cpu6502<TBus>::decodeOpcode() {
    // Get current instruction from opcode
    _instruction = _predecode;
    _currentInstruction = _instrPipelineFuncs[_instruction];
    
    // Execute current instruction
    (this->*_currentInstruction)();
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

// Stack

template <class TBus>
void Cpu6502<TBus>::pushToStack0(uint8_t data) {
    _dataOutput = data;
    writeDataBus(_stackPointer, _stackPageNumber, data);
    
    // Removing 1 from inputDataLatch using ALU (Add 0xFF without carry set like true 6502)
    _aInput = 0xFF;
    _bInput = _stackPointer;
    aluPerformSum(false, false);
}

template <class TBus>
void Cpu6502<TBus>::pushToStack1() {
    _stackPointer = _adderHold;
}

template <class TBus>
void Cpu6502<TBus>::pullFromStack0() {
    // Adding 1 with stackPointer using ALU (Add 0 with carry set like true 6502)
    _aInput = 0x0;
    _bInput = _stackPointer;
    aluPerformSum(false, true);
}

template <class TBus>
void Cpu6502<TBus>::pullFromStack1() {
    _stackPointer = _adderHold;
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

#endif /* Cpu6502_s_hpp */
