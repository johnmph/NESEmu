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
// TODO: enlever le flag helper et tout le bordel lié aux flags et n'avoir que les signaux séparés et une methode qui les regroupe et qui prend en parametre le bool Break car on les manipule bien plus souvent séparés que regroupé (seulement php, plp) : NON car plus lent : de 4.6sec ainsi a 5.4sec avec les signaux séparés !!!
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

template <class TConfiguration>
Chip<TConfiguration>::Chip(Bus &bus) : _flagsHelper(_statusFlags, _alu), _bus(bus) {
}

template <class TConfiguration>
void Chip<TConfiguration>::powerUp(uint16_t programCounter, uint8_t stackPointer, uint8_t accumulator, uint8_t xIndex, uint8_t yIndex, uint8_t statusFlags) {
    // Registers
    _programCounterLow = programCounter & 0xFF;
    _programCounterHigh = programCounter >> 8;
    _stackPointer = stackPointer;
    _accumulator = accumulator;
    _xIndex = xIndex;
    _yIndex = yIndex;
    _statusFlags = statusFlags;
    
    // Internal
    _addressBus = 0x0;
    _dataBus = 0x0;
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
    _resetLineLatch = true;
    _resetRequested = false;
    _nmiLine = true;
    _nmiLinePrevious = true;
    _nmiRequested = false;
    _irqLine = true;
    _irqRequested = false;
    _interruptRequested = false;
    
    // Begin reset
    resetAfterPowerUp<ResetAccurate>();
}

template <class TConfiguration>
void Chip<TConfiguration>::clock() {
    // Execute phi1
    clockPhi1();
    
    // Execute phi2
    clockPhi2();
}

template <class TConfiguration>
void Chip<TConfiguration>::clockPhi1() {
    // Start phi1
    _phi2 = false;
    
    // Update PC
    updateProgramCounter();
    
    // Check for overflow flag
    checkSetOverflow<SetOverflowEnabled>();
    
    // Initialize dataOutput to emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //_dataOutput = 0xFF;
    
    // If rdy is low, wait before perform next read cycle TODO: j'ai mis sync en plus pour arreter sur le fetchOpcode
    if ((_readyWaitRequested == false) || ((_readWrite != static_cast<bool>(ReadWrite::Read)) && (/*_sync == false*/true))) {
        // Execute current stage
        (this->*_currentInstruction)();
    }
    
    // Fetch memory for phi1
    fetchMemoryPhi1();
}

template <class TConfiguration>
void Chip<TConfiguration>::clockPhi2() {
    // Start phi2
    _phi2 = true;
    
    // Check reset
    checkReset<ResetAccurate>();
    
    // Check interrupts line
    checkNmi();
    checkIrq();
    
    // Check ready line
    checkReady();
    
    // Fetch memory for phi2
    fetchMemoryPhi2();
}

template <class TConfiguration>
void Chip<TConfiguration>::ready(bool high) {
    // Save signal
    _readyLine = high;
}

template <class TConfiguration>
void Chip<TConfiguration>::reset(bool high) {
    reset<ResetAccurate>(high);
}

template <class TConfiguration>
void Chip<TConfiguration>::nmi(bool high) {
    // Save signal
    _nmiLine = high;
}

template <class TConfiguration>
void Chip<TConfiguration>::irq(bool high) {
    // Save signal
    _irqLine = high;
}

template <class TConfiguration>
void Chip<TConfiguration>::setOverflow(bool high) {
    setOverflow<SetOverflowEnabled>(high);
}

template <class TConfiguration>
uint16_t Chip<TConfiguration>::getAddressBus() const {
    //return (_addressBusHigh << 8) | _addressBusLow; // TODO: _addressBus plutot (et avoir getAddressBusLow et getAddressBusHigh ?)
    return _addressBus;
}

template <class TConfiguration>
uint8_t Chip<TConfiguration>::getDataBus() const {
    return _dataBus;
}

template <class TConfiguration>
bool Chip<TConfiguration>::getReadWriteSignal() const {
    return _readWrite;
}

template <class TConfiguration>
bool Chip<TConfiguration>::getSyncSignal() const {
    return _sync;
}

template <class TConfiguration>
bool Chip<TConfiguration>::getM1Signal() const {
    return (_phi2 == false);
}

template <class TConfiguration>
bool Chip<TConfiguration>::getM2Signal() const {
    return (_phi2 == true);
}

// Memory

template <class TConfiguration>
void Chip<TConfiguration>::fetchMemoryPhi1() {
    // Read data on dataBus if it is in read mode else set dataBus with last read value
    if (_readWrite == static_cast<bool>(ReadWrite::Read)) {
        _dataBus = _bus.read(_addressBus);
    } else {
        _dataBus = _inputDataLatch;
    }
}

template <class TConfiguration>
void Chip<TConfiguration>::fetchMemoryPhi2() {
    // dataBus is already filled with date since end of phi1, just put dataBus on internal registers
    if (_readWrite == static_cast<bool>(ReadWrite::Read)) {
        _inputDataLatch = _dataBus;
        _predecode = _inputDataLatch;
        
        return;
    }
    
    // Write data to dataBus
    _dataBus = _dataOutput;
    _bus.write(_addressBus, _dataBus);
}

template <class TConfiguration>
void Chip<TConfiguration>::readDataBus(uint8_t low, uint8_t high) {
    // Write address bus registers
    _addressBusLow = low;
    _addressBusHigh = high;
    
    // Set address bus
    _addressBus = (_addressBusHigh << 8) | _addressBusLow;
    
    // Set R/W to read
    _readWrite = static_cast<bool>(ReadWrite::Read);
}

template <class TConfiguration>
void Chip<TConfiguration>::writeDataBus(uint8_t low, uint8_t high, uint8_t data) {
    // Write address bus registers
    _addressBusLow = low;
    _addressBusHigh = high;
    
    // Set address bus
    _addressBus = (_addressBusHigh << 8) | _addressBusLow;
    
    // Write data bus, emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //_dataOutput &= data;
    _dataOutput = data;
    
    // Set R/W to write (unless it's in reset)
    _readWrite = static_cast<bool>((_resetRequested == false) ? ReadWrite::Write : ReadWrite::Read);
}

// Program flow

template <class TConfiguration>
void Chip<TConfiguration>::incrementProgramCounter() {
    // In real 6502, PC is not increment in end of current cycle but in beginning of next cycle
    _programCounterNeedsIncrement = true;
}

template <class TConfiguration>
void Chip<TConfiguration>::updateProgramCounter() {
    _programCounterLow += _programCounterNeedsIncrement;
    _programCounterHigh += ((_programCounterLow == 0) && (_programCounterNeedsIncrement == true));
    
    _programCounterNeedsIncrement = false;
}

template <class TConfiguration>
void Chip<TConfiguration>::fetchData() {
    readDataBus(_programCounterLow, _programCounterHigh);
    incrementProgramCounter();
}

template <class TConfiguration>
void Chip<TConfiguration>::fetchOpcode(OpcodeInstruction nextInstruction) {
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

template <class TConfiguration>
void Chip<TConfiguration>::fetchOpcode() {
    // Fetch opcode then decode opcode and execute instruction on the next cycle
    fetchOpcode(&Chip::decodeOpcodeAndExecuteInstruction);
}
/*
template <class TConfiguration>
void Chip<TConfiguration>::fetchOpcodeAfterRdyLow() {
    // Set next instruction
    _currentInstruction = &Chip::decodeOpcodeAndExecuteInstruction;
    
    // Read opcode located at the address saved in address registers
    readDataBus(_addressBusLow, _addressBusHigh);
}*/

template <class TConfiguration>
void Chip<TConfiguration>::decodeOpcodeAndExecuteInstruction() {
    // We need this because some instructions will perform actions in the decode step of the next instruction, so we need to let them perform actions even if RDY is low, but we can't decode next opcode because opcode is not ready for reading
    if (_readyWaitRequested == true) {
        // We can't use fetchOpcode here because fetch was already performed on the previous cycle, just read the data which is ready for reading
        //_currentInstruction = &Chip::fetchOpcodeAfterRdyLow;
        return;
    }
    
    // Get current instruction from opcode (or BRK (0) if interrupt requested)
    _instruction = (_interruptRequested == false) ? _predecode : 0;
    _currentInstruction = _opcodeInstructionFuncs[_instruction];
    
    // Execute current instruction
    (this->*_currentInstruction)();
    
    // Set sync low
    _sync = false;
}

// Overflow

template <class TConfiguration>
template <bool BSetOverflowEnabled, typename std::enable_if<BSetOverflowEnabled == true, int>::type>
void Chip<TConfiguration>::setOverflow(bool high) {
    // Save signal
    _setOverflowLine = high;
}

template <class TConfiguration>
template <bool BSetOverflowEnabled, typename std::enable_if<BSetOverflowEnabled == false, int>::type>
void Chip<TConfiguration>::setOverflow(bool high) {
    static_assert(BSetOverflowEnabled == true, "Can't call setOverflow on a non-supported setOverflow CPU");
}

template <class TConfiguration>
template <bool BSetOverflowEnabled, typename std::enable_if<BSetOverflowEnabled == true, int>::type>
void Chip<TConfiguration>::checkSetOverflow() {
    // If setOverflow requested
    if (_setOverflowRequested == true) {
        // Enable overflow flag
        _flagsHelper.set<Flag::Overflow>(true);
        
        // Reset setOverflow requested
        _setOverflowRequested = false;
    }
    
    // Set overflow is requested if setOverflow line has transition from high to low
    if ((_setOverflowLinePrevious == true) && (_setOverflowLine == false)) {
        _setOverflowRequested = true;
    }
    
    // Save previous setOverflow signal
    _setOverflowLinePrevious = _setOverflowLine;
}

template <class TConfiguration>
template <bool BSetOverflowEnabled, typename std::enable_if<BSetOverflowEnabled == false, int>::type>
void Chip<TConfiguration>::checkSetOverflow() {
    // Does nothing
}

// Ready

template <class TConfiguration>
void Chip<TConfiguration>::checkReady() {
    // A ready wait is requested if the ready line is low
    _readyWaitRequested = (_readyLine == false);
}

// Reset

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate == true, int>::type>
void Chip<TConfiguration>::resetAfterPowerUp() {
    // Set current instruction because reset will save it in _resetSavedInstruction to execute it later
    _currentInstruction = &Chip::fetchOpcode;
    
    // Reset by pulling line down and calling checkReset two times because detection is delayed
    reset(false);
    checkReset<ResetAccurate>();
    checkReset<ResetAccurate>();
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate == false, int>::type>
void Chip<TConfiguration>::resetAfterPowerUp() {
    reset(false);
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate == true, int>::type>
void Chip<TConfiguration>::reset(bool high) {
    // Save signal
    _resetLine = high;
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate == false, int>::type>
void Chip<TConfiguration>::reset(bool high) {
    // If line is low and reset is not alreay requested
    if ((high == false) && (_resetRequested == false)) {
        // Set reset requested and execute reset0 for the next instruction
        _currentInstruction = &Chip::reset0<ResetAccurate>;
        _resetRequested = true;
        _interruptRequested = true;
    }
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate == true, int>::type>
void Chip<TConfiguration>::checkReset() {
    // If reset signal
    if (_resetLineLatch == false) {
        // If not already requested, save next instruction
        if (_resetRequested == false) {
            _resetSavedInstruction = _currentInstruction;
        }
        
        // Set reset requested and execute reset0 for the next instruction
        _currentInstruction = &Chip::reset0<ResetAccurate>;
        _resetRequested = true;
        _interruptRequested = true;
    }
    
    // Save signal
    _resetLineLatch = _resetLine;
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate == false, int>::type>
void Chip<TConfiguration>::checkReset() {
    // Does nothing
}

// Interrupts

template <class TConfiguration>
void Chip<TConfiguration>::checkNmi() {
    // Nmi is requested if nmi line has transition from high to low
    if ((_nmiLinePrevious == true) && (_nmiLine == false)) {
        _nmiRequested = true;
    }
    
    // Save previous nmi signal
    _nmiLinePrevious = _nmiLine;
}

template <class TConfiguration>
void Chip<TConfiguration>::checkIrq() {
    // Irq is requested if interrupts are not disabled and if irq line goes to low (and stay for one cycle)
    _irqRequested = (_flagsHelper.get<Flag::InterruptDisable>() == false) && (_irqLine == false);
}

template <class TConfiguration>
void Chip<TConfiguration>::checkInterrupts() {
    // Set interrupt requested flag if interrupts detected
    if ((_nmiRequested == true) || (_irqRequested == true)) {
        _interruptRequested = true;
    }
}

template <class TConfiguration>
int Chip<TConfiguration>::getCurrentInterruptVectorsIndex() {
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

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate == true, int>::type>
void Chip<TConfiguration>::correctInterruptVectorIndexForReset() {
    // Get reset vector if we are in reset
    if (_resetRequested == true) {
        _interruptVectorsIndex = static_cast<int>(Interrupts::Reset);
    }
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate == false, int>::type>
void Chip<TConfiguration>::correctInterruptVectorIndexForReset() {
    // Does nothing
}

// Stack

template <class TConfiguration>
void Chip<TConfiguration>::startStackOperation() {
    //_addressBusLow = _stackPointer;
    //_addressBusHigh = _stackPageNumber;
    readDataBus(_stackPointer, _stackPageNumber);
}

template <class TConfiguration>
void Chip<TConfiguration>::stopStackOperation() {
    _stackPointer = _addressBusLow;
}

template <class TConfiguration>
void Chip<TConfiguration>::pushToStack0(uint8_t data) {
    // Write to stack
    writeDataBus(_addressBusLow, _addressBusHigh, data);
    
    // Removing 1 from address low using ALU (Add 0xFF without carry set like true 6502)
    _alu.performSum<DecimalSupported, false>(0xFF, _addressBusLow, false, false);
}

template <class TConfiguration>
void Chip<TConfiguration>::pushToStack1() {
    _addressBusLow = _alu.getAdderHold();
}

template <class TConfiguration>
void Chip<TConfiguration>::pullFromStack0() {
    // Adding 1 with stackPointer using ALU (Add 0 with carry set like true 6502)
    _alu.performSum<DecimalSupported, false>(0x0, _addressBusLow, false, true);
}

template <class TConfiguration>
void Chip<TConfiguration>::pullFromStack1() {
    readDataBus(_alu.getAdderHold(), _addressBusHigh);
}

#endif /* Cpu6502_Internal_Chip_s_hpp */
