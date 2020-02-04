//
//  Chip_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 31/12/19.
//  Copyright © 2019 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Chip_s_hpp
#define Cpu6502_Internal_Chip_s_hpp

// TODO: pour une meilleure emulation des opcodes (surtout les undocumented), peut etre faire comme le vrai cpu et decomposer l'opcode en lignes actives/non actives pour activer certains circuits (appeler certaines fonctions) : https://www.pagetable.com/?p=39
// TODO: enlever le flag helper et tout le bordel lié aux flags et n'avoir que les signaux séparés et une methode qui les regroupe et qui prend en parametre le bool Break car on les manipule bien plus souvent séparés que regroupé (seulement php, plp)
// TODO: on peut tester le reset signal avec visual6502 aussi (reset0 et reset1)
// TODO: tester les undocumented avec visual6502 (un seul mode d'adressage suffit, c'est pour voir si les etapes internes sont bonnes) : ok a finir

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
Chip<TBus, TInternalHardware, BDecimalSupported>::Chip(TBus &bus) : _flagsHelper(_statusFlags, _alu), _bus(bus) {
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::powerUp(uint16_t programCounter, uint8_t stackPointer, uint8_t accumulator, uint8_t xIndex, uint8_t yIndex, uint8_t statusFlags) {
    // Registers
    _programCounterLow = programCounter & 0xFF;
    _programCounterHigh = programCounter >> 8;
    _stackPointer = stackPointer;
    _accumulator = accumulator;
    _xIndex = xIndex;
    _yIndex = yIndex;
    _statusFlags = statusFlags;
    
    // Internal
    _addressBusLow = 0x0;
    _addressBusHigh = 0x0;
    _inputDataLatch = 0x0;
    _dataOutput = 0x0;
    
    _readyLine = true;
    _readyWaitRequested = false;
    _sync = false;
    _readWrite = true;
    
    _programCounterNeedsIncrement = false;
    
    _setOverflowLine = false;
    _setOverflowLinePrevious = false;
    _setOverflowRequested = false;
    
    _resetLine = true;
    _resetRequested = false;
    _nmiLine = true;
    _nmiLinePrevious = true;
    _nmiRequested = false;
    _irqLine = true;
    _irqRequested = false;
    _interruptRequested = false;
    
    
    reset(false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clock() {
    // Perform a clock without forcing execution
    clock(false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clockPhi1() {
    // Perform a clock phi1 without forcing execution
    clockPhi1(false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::ready(bool high) {
    // Save signal
    _readyLine = high;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::reset(bool high) {// TODO: tester reset pour etre sur qu'il ne fait que ca et qu'il ne reset pas les registres
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
void Chip<TBus, TInternalHardware, BDecimalSupported>::nmi(bool high) {
    // Save signal
    _nmiLine = high;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::irq(bool high) {
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
    return _dataBus;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
bool Chip<TBus, TInternalHardware, BDecimalSupported>::getReadWriteSignal() const {
    return _readWrite;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
bool Chip<TBus, TInternalHardware, BDecimalSupported>::getSyncSignal() const {
    return _sync;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
bool Chip<TBus, TInternalHardware, BDecimalSupported>::getM1Signal() const {
    return (_phi2 == false);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
bool Chip<TBus, TInternalHardware, BDecimalSupported>::getM2Signal() const {
    return (_phi2 == true);
}

// Clock

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clock(bool forceExecute) {
    // ** PHI1 **
    clockPhi1(forceExecute);
    
    // ** PHI2 **
    clockPhi2();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clockPhi1(bool forceExecute) {
    // Start phi1
    _phi2 = false;
    
    // Update PC
    updateProgramCounter();
    
    // If setOverflow requested
    if (_setOverflowRequested == true) {        // TODO: mettre ca dans une methode
        // Enable overflow flag
        _flagsHelper.set<Flag::Overflow>(true);
        
        // Reset setOverflow requested
        _setOverflowRequested = false;
    }
    
    // Check for overflow flag
    checkOverflowFlag();
    
    // Initialize dataOutput to emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //_dataOutput = 0xFF;
    
    // If rdy is low, wait before perform next read cycle unless we force execute
    if ((_readyWaitRequested == false) || (_readWrite != static_cast<bool>(ReadWrite::Read)) || (forceExecute == true)) {
        // Execute current stage
        (this->*_currentInstruction)();
    }
    
    // Fetch memory for phi1
    fetchMemoryPhi1();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::clockPhi2() {
    // Start phi2
    _phi2 = true;
    
    // Check interrupts line
    checkNmi();
    checkIrq();
    
    // Check ready line
    checkReady();
    
    // Fetch memory for phi2
    fetchMemoryPhi2();
}

// Memory

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::fetchMemoryPhi1() {
    // Read data on dataBus if it is in read mode else set dataBus with last read value
    if ((_readWrite == static_cast<bool>(ReadWrite::Read)) || (_resetRequested == true)) {
        _dataBus = _bus.read((_addressBusHigh << 8) | _addressBusLow);
    } else {
        _dataBus = _inputDataLatch;
    }
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::fetchMemoryPhi2() {
    // dataBus is already filled with date since end of phi1, just put dataBus on internal registers
    if ((_readWrite == static_cast<bool>(ReadWrite::Read)) || (_resetRequested == true)) {
        _inputDataLatch = _dataBus;
        _predecode = _inputDataLatch;
        
        return;
    }
    
    // Write data to dataBus
    _dataBus = _dataOutput;
    _bus.write((_addressBusHigh << 8) | _addressBusLow, _dataBus);
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
    if (_interruptRequested == true) {
        // Read opcode without increment PC
        readDataBus(_programCounterLow, _programCounterHigh);
    } else {
        // Fetch opcode
        fetchData();
    }
    
    // Set sync high
    _sync = true;
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
        // We can't use fetchOpcode here because fetch was already performed on the previous cycle, just read the data which is ready for reading
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
        //_flagsHelper.set<Flag::Overflow>(_setOverflowLine == false);
        _setOverflowRequested = true;
    }
    
    // Save previous setOverflow signal
    _setOverflowLinePrevious = _setOverflowLine;
}

// Ready

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::checkReady() {
    // A ready wait is requested if the ready line is low
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
void Chip<TBus, TInternalHardware, BDecimalSupported>::checkInterrupts() {
    // Set interrupt requested flag if interrupts detected
    if ((_nmiRequested == true) || (_irqRequested == true)) {
        _interruptRequested = true;
    }
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
