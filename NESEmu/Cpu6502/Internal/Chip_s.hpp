//
//  Chip_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 31/12/19.
//  Copyright © 2019 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Chip_s_hpp
#define Cpu6502_Internal_Chip_s_hpp

// TODO: par apres, si assez rapide, decomposer en 2 phases de cycles (Ph0, Ph1, Ph2, RDY, R/W, Sync) : pas besoin car le cpu a besoin de ca car tout arrive en meme temps (les signaux electriques) mais ici on peut appeler des fonctions a la suite des autres dans le meme cycle et simuler ca : NON car les interruptions sont peut etre checkées a ph1 et ph2 !!!
// TODO: pour une meilleure emulation des opcodes (surtout les undocumented), peut etre faire comme le vrai cpu et decomposer l'opcode en lignes actives/non actives pour activer certains circuits (appeler certaines fonctions) : https://www.pagetable.com/?p=39
#include "Data.hpp"


namespace _Detail {
    
    // Alu
    
    template <bool BDecimalSupported, bool BSubstractMode>
    void Alu::performSum(uint8_t aInput, uint8_t bInput, bool decimalEnable, bool carryIn) {//TODO: terminer (halfCarry, decimalEnable)
        _aInput = aInput;
        _bInput = bInput;
        
        // Only for C++17
        /*if constexpr (BSubstractMode == true) {
            invertBInput();
        }*/
        // Kind of static if in C++11
        performSubstractMode<BSubstractMode>();
        
        uint16_t add = _aInput + _bInput + carryIn;
        _adderHold = static_cast<uint8_t>(add);
        
        _overflow = (_aInput ^ _adderHold) & (_bInput ^ _adderHold) & 0x80;
        _carry = add & 0x100;
        //_halfCarry = ((_aInput & 0xF) + (_bInput & 0xF) + carryIn) & 0x10;  // TODO: voir ce que c'est et si c'est utilisé
    }
    
    template <bool BSubstractMode>
    void Alu::performSubstractMode() {
        // Does nothing
    }
    
    
    // Flags Helper
    
    template <FlagsHelper::Flag ...EFlags>
    constexpr uint8_t FlagsHelper::getEnableMask() {
        uint8_t data = 0;
        
        // Fold expression in C++11 (second line is to avoid warning on non used variable)
        int fold[] = { (data |= (1 << static_cast<uint8_t>(EFlags)))... };
        (void) fold;
        
        return data;
        // Fold expression in C++17
        //return ((1 << static_cast<uint8_t>(EFlags)) | ...);
    }
    
    template <FlagsHelper::Flag ...EFlags>
    constexpr uint8_t FlagsHelper::getDisableMask() {
        return static_cast<uint8_t>(~getEnableMask<EFlags...>());
    }
    
    template <FlagsHelper::Flag ...EFlags>
    void FlagsHelper::clear() {
        _value &= getDisableMask<EFlags...>();
    }
    
    template <FlagsHelper::Flag ...EFlags>
    bool FlagsHelper::get() {
        return _value & getEnableMask<EFlags...>();
    }
    
    template <FlagsHelper::Flag EFlag>
    void FlagsHelper::set(bool enable) {
        // Be careful, if enable can be false, we need to call clearStatusFlags before (It's for multiple clear/set flags optimization by avoiding if statement)
        _value |= enable << static_cast<uint8_t>(EFlag);
    }
    
    template <FlagsHelper::Flag EFirstFlag, FlagsHelper::Flag ESecondFlag, FlagsHelper::Flag ...EOtherFlags, typename std::enable_if<(EFirstFlag == FlagsHelper::Flag::Negative) || (EFirstFlag == FlagsHelper::Flag::Zero), int>::type>
    void FlagsHelper::refreshImpl(uint8_t data) {
        // Check first flag
        refreshImpl<EFirstFlag>(data);
        
        // Recursively check remaining flags
        refreshImpl<ESecondFlag, EOtherFlags...>(data);
    }
    
    template <FlagsHelper::Flag EFirstFlag, FlagsHelper::Flag ESecondFlag, FlagsHelper::Flag ...EOtherFlags, typename std::enable_if<(EFirstFlag != FlagsHelper::Flag::Negative) && (EFirstFlag != FlagsHelper::Flag::Zero), int>::type>
    void FlagsHelper::refreshImpl(uint8_t data) {
        // Check first flag
        refreshImpl<EFirstFlag>();
        
        // Recursively check remaining flags
        refreshImpl<ESecondFlag, EOtherFlags...>(data);
    }
    
    template <FlagsHelper::Flag ...EFlags>
    void FlagsHelper::refresh(uint8_t data) {
        // First clear flags bits
        clear<EFlags...>();
        
        // Then check flags
        refreshImpl<EFlags...>(data);
    }
    
}


// Public interface

template <class TBus, class TInternalHardware, bool BDecimalSupported>
Chip<TBus, TInternalHardware, BDecimalSupported>::Chip(TBus &bus) : Chip(bus, 0xFF, 0x0, 0xAA, 0x0, 0x0, 0x20) {
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
Chip<TBus, TInternalHardware, BDecimalSupported>::Chip(TBus &bus, uint16_t programCounter, uint8_t stackPointer, uint8_t accumulator, uint8_t xIndex, uint8_t yIndex, uint8_t statusFlags) : _flagsHelper(_statusFlags, _alu), _bus(bus), _programCounterLow(programCounter & 0xFF), _programCounterHigh(programCounter >> 8), _stackPointer(stackPointer), _accumulator(accumulator), _xIndex(xIndex), _yIndex(yIndex), _statusFlags(statusFlags), _readyLine(true), _readyWaitRequested(false), _setOverflowLine(true), _setOverflowLinePrevious(true),  _programCounterNeedsIncrement(false), _resetRequested(false), _nmiLine(true), _nmiLinePrevious(true), _nmiRequested(false), _irqLine(true), _irqRequested(false), _interruptRequested(false) {
    reset(false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clock() {
    // Perform a clock without forcing execution
    clock(false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ready(bool high) {
    // Save signal
    _readyLine = high;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::reset(bool high) {  // TODO: voir pour les registres si on les laisse ainsi ou si on les reset : normalement on les laisse ainsi puisqu'un reset c'est juste un signal comme une interruption (mais a voir, on ne sait jamais) : peut etre avoir une methode power qui fait un hard reset et ce reset est le soft reset
    // Save signal
    _resetLine = high;
    
    // If reset
    if ((_resetLine == false) && (_resetRequested == false)) {
        _currentInstruction = &Chip::reset0;
        _resetRequested = true;
        _interruptRequested = true; // TODO: voir si ok pour reset aussi
    }
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::nmi(bool high) {    // TODO: a terminer/tester (il parait qu'il faut 2 cycles pour que le signal low soit pris en compte, a voir)
    // Save signal
    _nmiLine = high;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::irq(bool high) {    // TODO: a tester
    // Save signal
    _irqLine = high;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::setOverflow(bool high) {
    // Save signal
    _setOverflowLine = high;
}
/*
template <class TBus, class TInternalHardware, bool BDecimalSupported>
uint16_t Chip<TBus, TInternalHardware, BDecimalSupported>::getProgramCounter() const {
    return (_programCounterHigh << 8) | _programCounterLow;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
uint8_t Chip<TBus, TInternalHardware, BDecimalSupported>::getStackPointer() const {
    return _stackPointer;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
uint8_t Chip<TBus, TInternalHardware, BDecimalSupported>::getAccumulator() const {
    return _accumulator;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
uint8_t Chip<TBus, TInternalHardware, BDecimalSupported>::getXIndex() const {
    return _xIndex;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
uint8_t Chip<TBus, TInternalHardware, BDecimalSupported>::getYIndex() const {
    return _yIndex;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
uint8_t Chip<TBus, TInternalHardware, BDecimalSupported>::getStatusFlags() const {
    return _statusFlags;
}*/

template <class TBus, class TInternalHardware, bool BDecimalSupported>
uint16_t Chip<TBus, TInternalHardware, BDecimalSupported>::getAddressBus() const {
    return (_addressBusHigh << 8) | _addressBusLow;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
uint8_t Chip<TBus, TInternalHardware, BDecimalSupported>::getDataBus() const {
    return ((_readWrite == static_cast<bool>(ReadWrite::Read)) || (_resetRequested == true)) ? _inputDataLatch : _dataOutput;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
bool Chip<TBus, TInternalHardware, BDecimalSupported>::getReadWriteSignal() const {
    return _readWrite;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
bool Chip<TBus, TInternalHardware, BDecimalSupported>::getSyncSignal() const {
    return _sync;
}

// Clock

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clock(bool forceExecute) {
    // ** PHI1 **
    
    // Update PC
    updateProgramCounter(); // TODO: voir si ici ou dans phi2 (et donc plus besoin de ca car simplement le mettre la ou on l'appelle
    
    // Check for overflow flag
    checkOverflowFlag();
    
    // Initialize dataOutput to emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //_dataOutput = 0xFF;
    
    // If rdy is low, wait before perform next read cycle unless we force execute
    if (((_readyWaitRequested == false) || (_readWrite != static_cast<bool>(ReadWrite::Read))) || (forceExecute == true)) {
        // Execute current stage
        (this->*_currentInstruction)();
    }
    
    // ** PHI2 **
    
    // Interrupts are checked each clock (second half of cycle)
    checkNmi();
    checkIrq();
    
    // Check ready line
    checkReady();
    
    // Each clock, there is a memory access
    fetchMemory();
}

// Memory

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::fetchMemory() {
    if ((_readWrite == static_cast<bool>(ReadWrite::Read)) || (_resetRequested == true)) {
        _inputDataLatch = _bus.read((_addressBusHigh << 8) | _addressBusLow);
        _predecode = _inputDataLatch;
        
        return;
    }
    
    _bus.write((_addressBusHigh << 8) | _addressBusLow, _dataOutput);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::readDataBus(uint8_t low, uint8_t high) {
    // Write address bus
    _addressBusLow = low;
    _addressBusHigh = high;
    
    // Set R/W to read
    _readWrite = static_cast<bool>(ReadWrite::Read);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::writeDataBus(uint8_t low, uint8_t high, uint8_t data) {
    // Write address bus
    _addressBusLow = low;
    _addressBusHigh = high;
    
    // Write data bus, emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //_dataOutput &= data;
    _dataOutput = data;
    
    // Set R/W to write
    _readWrite = static_cast<bool>(ReadWrite::Write);
}

// Program flow

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::incrementProgramCounter() {
    // In real 6502, PC is not increment in end of current cycle but in beginning of next cycle
    _programCounterNeedsIncrement = true;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::updateProgramCounter() {
    _programCounterLow += _programCounterNeedsIncrement;
    _programCounterHigh += ((_programCounterLow == 0) && (_programCounterNeedsIncrement == true));
    
    _programCounterNeedsIncrement = false;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::fetchData() {
    readDataBus(_programCounterLow, _programCounterHigh);
    incrementProgramCounter();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::fetchOpcode(OpcodeInstruction nextInstruction) {
    // Set next instruction
    _currentInstruction = nextInstruction;
    
    // If interrupt requested
    if (checkInterrupts() == true) {
        // Read opcode without increment PC
        readDataBus(_programCounterLow, _programCounterHigh);
        
        // Save interrupt flag to know that an interrupt is occur (and not brk opcode)
        _interruptRequested = true;
        
        // Set sync high
        _sync = true;   // TODO: voir si ici ou au debut de la methode, normalement c'est ici (si c'est au debut, pas besoin d'avoir les 2)
        
        return;
    }
    
    fetchData();
    
    // Set sync high
    _sync = true;   // TODO: voir si ici ou au debut de la methode, normalement c'est ici (si c'est au debut, pas besoin d'avoir les 2)
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::fetchOpcode() {
    // Fetch opcode then decode opcode and execute instruction on the next cycle
    fetchOpcode(&Chip::decodeOpcodeAndExecuteInstruction);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::fetchOpcodeAfterRdyLow() {
    // Set next instruction
    _currentInstruction = &Chip::decodeOpcodeAndExecuteInstruction;
    
    // Read opcode located at the address saved in address registers
    readDataBus(_addressBusLow, _addressBusHigh);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::decodeOpcodeAndExecuteInstruction() {
    // We need this because some instructions will perform actions in the decode step of the next instruction, so we need to let them perform actions even if RDY is low, but we can't decode next opcode because opcode is not ready for reading
    if (_readyWaitRequested == true) {
        // We can't use fetchOpcode here because fetch was already performed on the previous cycle, so the interrupts must not be checked again, just the data which is ready for reading
        _currentInstruction = &Chip::fetchOpcodeAfterRdyLow;
        return;
    }
    
    // Get current instruction from opcode (or BRK (0) if interrupt requested)
    _instruction = (_interruptRequested == false) ? _predecode : 0;
    _currentInstruction = _opcodeInstructionFuncs[_instruction];
    
    // Execute current instruction
    (this->*_currentInstruction)();
    
    // Set sync low
    _sync = false;   // TODO: voir si ici ou au debut de la methode, normalement c'est ici
}

// Overflow

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::checkOverflowFlag() {
    // Set overflow is requested if setOverflow line has transition from high to low
    if ((_setOverflowLinePrevious == true) && (_setOverflowLine == false)) {
        // Enable overflow flag if setOverflow is low, otherwise does nothing
        _flagsHelper.set<Flag::Overflow>(_setOverflowLine == false);
    }
    
    // Save previous setOverflow signal
    _setOverflowLinePrevious = _setOverflowLine;
}

// Ready

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::checkReady() {
    // Enable overflow flag if setOverflow is low, otherwise does nothing
    _readyWaitRequested = (_readyLine == false);
}

// Interrupts

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::checkNmi() {
    // Nmi is requested if nmi line has transition from high to low
    if ((_nmiLinePrevious == true) && (_nmiLine == false)) {
        _nmiRequested = true;
    }
    
    // Save previous nmi signal
    _nmiLinePrevious = _nmiLine;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::checkIrq() {
    // Irq is requested if interrupts are not disabled and if irq line goes to low (and stay for one cycle)
    _irqRequested = (_flagsHelper.get<Flag::InterruptDisable>() == false) && (_irqLine == false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
bool Chip<TBus, TInternalHardware, BDecimalSupported>::checkInterrupts() {
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

template <class TBus, class TInternalHardware, bool BDecimalSupported>
int Chip<TBus, TInternalHardware, BDecimalSupported>::getCurrentInterruptVectorsIndex() {
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

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::startStackOperation() {
    _addressBusLow = _stackPointer;
    _addressBusHigh = _stackPageNumber;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::stopStackOperation() {
    _stackPointer = _addressBusLow;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::pushToStack0(uint8_t data) {
    // Write to stack
    writeDataBus(_addressBusLow, _addressBusHigh, data);
    
    // Removing 1 from address low using ALU (Add 0xFF without carry set like true 6502)
    _alu.performSum<BDecimalSupported, false>(0xFF, _addressBusLow, false, false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::pushToStack1() {
    _addressBusLow = _alu.getAdderHold();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::pullFromStack0() {
    // Adding 1 with stackPointer using ALU (Add 0 with carry set like true 6502)
    _alu.performSum<BDecimalSupported, false>(0x0, _addressBusLow, false, true);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::pullFromStack1() {
    readDataBus(_alu.getAdderHold(), _addressBusHigh);
}

#endif /* Cpu6502_Internal_Chip_s_hpp */
