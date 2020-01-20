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
    
    // Flags Helper
    
    template <FlagsHelper::Flag ...flags>
    constexpr uint8_t FlagsHelper::getEnableMask() {
        uint8_t data = 0;
        
        // Fold expression in C++11 (second line is to avoid warning on non used variable)
        int fold[] = { (data |= (1 << static_cast<uint8_t>(flags)))... };
        (void) fold;
        
        return data;
        //return ((1 << static_cast<uint8_t>(flags)) | ...);    // Fold expression in C++17
    }
    
    template <FlagsHelper::Flag ...flags>
    constexpr uint8_t FlagsHelper::getDisableMask() {
        return static_cast<uint8_t>(~getEnableMask<flags...>());
    }
    
    template <FlagsHelper::Flag ...flags>
    void FlagsHelper::clear() {
        _value &= getDisableMask<flags...>();
    }
    
    template <FlagsHelper::Flag ...flags>
    bool FlagsHelper::get() {
        return _value & getEnableMask<flags...>();
    }
    
    template <FlagsHelper::Flag flag>
    void FlagsHelper::set(bool enable) {
        // Be careful, if enable can be false, we need to call clearStatusFlags before (It's for multiple clear/set flags optimization by avoiding if statement)
        _value |= enable << static_cast<uint8_t>(flag);
    }
    
    template <FlagsHelper::Flag firstFlag, FlagsHelper::Flag secondFlag, FlagsHelper::Flag ...otherFlags, typename std::enable_if<(firstFlag == FlagsHelper::Flag::Negative) || (firstFlag == FlagsHelper::Flag::Zero), int>::type>
    void FlagsHelper::refreshImpl(uint8_t data) {
        // Check first flag
        refreshImpl<firstFlag>(data);
        
        // Recursively check remaining flags
        refreshImpl<secondFlag, otherFlags...>(data);
    }
    
    template <FlagsHelper::Flag firstFlag, FlagsHelper::Flag secondFlag, FlagsHelper::Flag ...otherFlags, typename std::enable_if<(firstFlag != FlagsHelper::Flag::Negative) && (firstFlag != FlagsHelper::Flag::Zero), int>::type>
    void FlagsHelper::refreshImpl(uint8_t data) {
        // Check first flag
        refreshImpl<firstFlag>();
        
        // Recursively check remaining flags
        refreshImpl<secondFlag, otherFlags...>(data);
    }
    
    template <FlagsHelper::Flag ...flags>
    void FlagsHelper::refresh(uint8_t data) {
        // First clear flags bits
        clear<flags...>();
        
        // Then check flags
        refreshImpl<flags...>(data);
    }
    
}


// Public interface

template <class TBus>
Chip<TBus>::Chip(TBus &bus) : _flagsHelper(_statusFlags, _aluCarry, _aluOverflow), _bus(bus), _programCounterLow(0xFF), _programCounterHigh(0), _stackPointer(0), _accumulator(0xAA), _xIndex(0), _yIndex(0), _statusFlags(0x20), _programCounterNeedsIncrement(false), _resetRequested(false), _nmiLine(true), _nmiLinePrevious(true), _nmiRequested(false), _irqLine(true), _irqRequested(false), _interruptRequested(false) {
    reset(false);
}

template <class TBus>
void Chip<TBus>::clock() {
    // Update PC
    updateProgramCounter();
    
    // Initialize dataOutput to emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //_dataOutput = 0xFF;
    
    // Execute current stage
    (this->*_currentInstruction)();
    
    // Interrupts are checked each clock (second half of cycle)
    checkNmi();
    checkIrq();
    
    // Each clock, there is a memory access
    fetchMemory();
}

template <class TBus>
void Chip<TBus>::reset(bool high) {  // TODO: voir pour les registres si on les laisse ainsi ou si on les reset : normalement on les laisse ainsi puisqu'un reset c'est juste un signal comme une interruption (mais a voir, on ne sait jamais) : peut etre avoir une methode power qui fait un hard reset et ce reset est le soft reset
    // Save signal
    _resetLine = high;
    
    // If reset
    if ((_resetLine == false) && (_resetRequested == false)) {
        _currentInstruction = &Chip::reset0;
        _resetRequested = true;
        _interruptRequested = true; // TODO: voir si ok pour reset aussi
    }
}

template <class TBus>
void Chip<TBus>::nmi(bool high) {    // TODO: a terminer/tester (il parait qu'il faut 2 cycles pour que le signal low soit pris en compte, a voir)
    // Save signal
    _nmiLine = high;
}

template <class TBus>
void Chip<TBus>::irq(bool high) {    // TODO: a tester
    // Save signal
    _irqLine = high;
}

template <class TBus>
uint16_t Chip<TBus>::getProgramCounter() {
    return (_programCounterHigh << 8) | _programCounterLow;
}

template <class TBus>
uint16_t Chip<TBus>::getAddressBus() {
    return (_addressBusHigh << 8) | _addressBusLow;
}

template <class TBus>
uint8_t Chip<TBus>::getDataBus() {
    return ((_readWrite == static_cast<bool>(ReadWrite::Read)) || (_resetRequested == true)) ? _inputDataLatch : _dataOutput;
}

// Memory

template <class TBus>
void Chip<TBus>::fetchMemory() {
    if ((_readWrite == static_cast<bool>(ReadWrite::Read)) || (_resetRequested == true)) {
        _inputDataLatch = _bus.read((_addressBusHigh << 8) | _addressBusLow);
        _predecode = _inputDataLatch;
        
        return;
    }
    
    _bus.write((_addressBusHigh << 8) | _addressBusLow, _dataOutput);
}

template <class TBus>
void Chip<TBus>::readDataBus(uint8_t low, uint8_t high) {
    // Write address bus
    _addressBusLow = low;
    _addressBusHigh = high;
    
    // Set R/W to read
    _readWrite = static_cast<bool>(ReadWrite::Read);
}

template <class TBus>
void Chip<TBus>::writeDataBus(uint8_t low, uint8_t high, uint8_t data) {
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

template <class TBus>
void Chip<TBus>::incrementProgramCounter() {
    // In real 6502, PC is not increment in end of current cycle but in beginning of next cycle
    _programCounterNeedsIncrement = true;
}

template <class TBus>
void Chip<TBus>::updateProgramCounter() {
    _programCounterLow += _programCounterNeedsIncrement;
    _programCounterHigh += ((_programCounterLow == 0) && (_programCounterNeedsIncrement == true));
    
    _programCounterNeedsIncrement = false;
}

template <class TBus>
void Chip<TBus>::fetchData() {
    readDataBus(_programCounterLow, _programCounterHigh);
    incrementProgramCounter();
}

template <class TBus>
void Chip<TBus>::fetchOpcode(OpcodeInstruction nextInstruction) {
    // Set next instruction
    _currentInstruction = nextInstruction;
    
    // If interrupt requested
    if (checkInterrupts() == true) {
        // Read opcode without increment PC
        readDataBus(_programCounterLow, _programCounterHigh);   // TODO: voir si ok
        
        // Save interrupt flag to know that an interrupt is occur (and not brk opcode)
        _interruptRequested = true;
        
        return;
    }
    
    fetchData();
}

template <class TBus>
void Chip<TBus>::fetchOpcode() {
    // Fetch opcode then decode opcode and execute instruction on the next cycle
    fetchOpcode(&Chip::decodeOpcodeAndExecuteInstruction);
}

template <class TBus>
void Chip<TBus>::decodeOpcodeAndExecuteInstruction() {
    // Get current instruction from opcode (or BRK (0) if interrupt requested)
    _instruction = (_interruptRequested == false) ? _predecode : 0;
    _currentInstruction = _opcodeInstructionFuncs[_instruction];
    
    // Execute current instruction
    (this->*_currentInstruction)();
}

// Interrupts

template <class TBus>
void Chip<TBus>::checkNmi() {
    // Nmi is requested if nmi line has transition from high to low
    if ((_nmiLinePrevious == true) && (_nmiLine == false)) {
        _nmiRequested = true;
    }
    
    // Save previous nmi signal
    _nmiLinePrevious = _nmiLine;
}

template <class TBus>
void Chip<TBus>::checkIrq() {
    // Irq is requested if interrupts are not disabled and if irq line goes to low (and stay for one cycle)
    _irqRequested = (_flagsHelper.get<Flag::InterruptDisable>() == false) && (_irqLine == false);
}

template <class TBus>
bool Chip<TBus>::checkInterrupts() {
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
int Chip<TBus>::getCurrentInterruptVectorsIndex() {
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
void Chip<TBus>::startStackOperation() {
    _addressBusLow = _stackPointer;
    _addressBusHigh = _stackPageNumber;
}

template <class TBus>
void Chip<TBus>::stopStackOperation() {
    _stackPointer = _addressBusLow;
}

template <class TBus>
void Chip<TBus>::pushToStack0(uint8_t data) {
    // Write to stack
    writeDataBus(_addressBusLow, _addressBusHigh, data);
    
    // Removing 1 from address low using ALU (Add 0xFF without carry set like true 6502)
    _aInput = 0xFF;
    _bInput = _addressBusLow;
    aluPerformSum(false, false);
}

template <class TBus>
void Chip<TBus>::pushToStack1() {
    _addressBusLow = _adderHold;
}

template <class TBus>
void Chip<TBus>::pullFromStack0() {
    // Adding 1 with stackPointer using ALU (Add 0 with carry set like true 6502)
    _aInput = 0x0;
    _bInput = _addressBusLow;
    aluPerformSum(false, true);
}

template <class TBus>
void Chip<TBus>::pullFromStack1() {
    readDataBus(_adderHold, _addressBusHigh);
}

// ALU

template <class TBus>
void Chip<TBus>::aluInvertBInput() {
    _bInput = ~_bInput;
}

template <class TBus>
void Chip<TBus>::aluPerformSum(bool decimalEnable, bool carryIn) {//TODO: terminer (halfCarry, decimalEnable)
    uint16_t add = _aInput + _bInput + carryIn;
    _adderHold = static_cast<uint8_t>(add);
    
    _aluOverflow = (_aInput ^ _adderHold) & (_bInput ^ _adderHold) & 0x80;
    _aluCarry = add & 0x100;
    //_aluHalfCarry = ((_aInput & 0xF) + (_bInput & 0xF) + carryIn) & 0x10;  // TODO: voir ce que c'est et si c'est utilisé
}

template <class TBus>
void Chip<TBus>::aluPerformAnd() {
    _adderHold = _aInput & _bInput;
}

template <class TBus>
void Chip<TBus>::aluPerformOr() {
    _adderHold = _aInput | _bInput;
}

template <class TBus>
void Chip<TBus>::aluPerformEor() {
    _adderHold = _aInput ^ _bInput;
}

template <class TBus>
void Chip<TBus>::aluPerformShiftRight(bool carryIn) {
    _adderHold = (_aInput >> 1) | (carryIn << 7);
    
    // In real 6502, it's not _aluCarry signal which is set but it's the first bit of DB bus which is used
    _aluCarry = _aInput & 0x1;
}

#endif /* Cpu6502_Internal_Chip_s_hpp */
