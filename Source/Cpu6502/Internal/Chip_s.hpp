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
// TODO: http://visual6502.org/wiki/index.php?title=6502_all_256_Opcodes
// TODO: si on fait ca il faut avoir un tablaeau d'instructions courantes a executer avec un identifiant pour savoir si l'instruction depend d'un read (pour le rdy low qui peut continuer des alu operations)
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
        /*if constexpr (BSubstractMode) {
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
    
    // Function template explicit instantiation of the specialization
    template <>
    void Alu::performSubstractMode<true>();
    
    
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
    
    // Function template explicit instantiation of the specialization
    template <>
    void FlagsHelper::refreshImpl<FlagsHelper::Flag::Negative>(uint8_t data);
    
    template <>
    void FlagsHelper::refreshImpl<FlagsHelper::Flag::Zero>(uint8_t data);
    
    template <>
    void FlagsHelper::refreshImpl<FlagsHelper::Flag::Carry>();
    
    template <>
    void FlagsHelper::refreshImpl<FlagsHelper::Flag::Overflow>();
    
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
    _addressBusLow = 0x0;
    _addressBusHigh = 0x0;
    _inputDataLatch = 0x0;
    _dataOutput = 0x0;
    
    _readyLine = true;
    _readyWaitRequested = false;
    _sync = false;
    _readWrite = ReadWrite::Read;
    
    _programCounterNeedsIncrement = false;
    
    _setOverflowLine = true;
    _setOverflowLinePrevious = true;
    _setOverflowRequested = false;
    
    _resetLine = true;
    _resetLineLatch = true;
    _resetRequested = false;
    _nmiLine = true;
    _nmiLatched = true;
    _nmiLatchedPrevious = true;
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
    // End phi2
    endPhi2();
    
    // Start phi1
    startPhi1();
}

template <class TConfiguration>
void Chip<TConfiguration>::clockPhi2() {
    // End phi1
    endPhi1();
    
    // Start phi2
    startPhi2();
}

template <class TConfiguration>
void Chip<TConfiguration>::startPhi1() {
    // Start phi1
    _phi2 = false;
    
    // Update PC
    updateProgramCounter();
    
    // Initialize dataOutput to emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //_dataOutput = 0xFF;
    
    // If rdy is low, wait before perform next read cycle
    if ((!_readyWaitRequested) || (_readWrite != ReadWrite::Read)) {//TODO: voir car ready n'affecte pas le reset normalement
        // Execute current stage
        (this->*_currentInstruction)();
    }
}

template <class TConfiguration>
void Chip<TConfiguration>::endPhi1() {
    // Read data on dataBus if it is in read mode
    if (_readWrite == ReadWrite::Read) {
        _bus.performRead();
    }
    // Set dataBus with last read value in it is in write mode
    /*else {
        _bus.setDataBus(_inputDataLatch);//TODO: a voir pq c'est nécessaire pour les tests
    }*/
    
    // Check for overflow flag
    checkSetOverflow<SetOverflowEnabled>();
}

template <class TConfiguration>
void Chip<TConfiguration>::startPhi2() {
    // Start phi2
    _phi2 = true;
    
    // Write data on dataBus if it is in write mode
    if (_readWrite == ReadWrite::Write) {
        // Write data to dataBus
        _bus.setDataBus(_dataOutput);
        _bus.performWrite();
    }
}

template <class TConfiguration>
void Chip<TConfiguration>::endPhi2() {
    // Copy data from dataBus on internal registers
    //if (_readWrite == ReadWrite::Read) {
        _inputDataLatch = _bus.getDataBus();
        _predecode = _inputDataLatch;
    //}
    
    // Check reset
    checkReset<ResetAccurate>();
    
    // Check interrupts line
    checkNmi();
    checkIrq();
    
    // Check ready line
    checkReady();
}

template <class TConfiguration>
void Chip<TConfiguration>::ready(bool high) {
    // Save signal
    _readyLine = high;
}

template <class TConfiguration>
void Chip<TConfiguration>::reset(bool high) {//TODO: voir pour avoir la meme logique que nmi pour irq et reset (par rapport a la phase complete de phi2)
    reset<ResetAccurate>(high);
}

template <class TConfiguration>
void Chip<TConfiguration>::nmi(bool high) {
    // Save signal
    _nmiLine = high;
    
    // Process if we are in phi2
    if (!_phi2) {
        return;
    }
    
    // Latch signal
    _nmiLatched = _nmiLine;
    
    // Nmi is requested if nmi line has transition from high to low
    if (_nmiLatchedPrevious && (!_nmiLatched)) {
        _nmiRequested = true;
    }
}

template <class TConfiguration>
void Chip<TConfiguration>::irq(bool high) {//TODO: voir pour avoir la meme logique que nmi pour irq et reset (par rapport a la phase complete de phi2)
    // Save signal
    _irqLine = high;
    
    // Irq is requested if interrupts are not disabled and if irq line goes to low in phi2
    _irqRequested = (!_flagsHelper.get<Flag::InterruptDisable>()) && (!_irqLine) && _phi2;//TODO: voir si ce n'est pas un probleme que si on appelle irq meme avec high = false en phi1 on reset _irqRequested (normalement non car on fait le checkIrq apres mais bon voir pour etre sur et mettre un comment pour notifier ca)
}

template <class TConfiguration>
void Chip<TConfiguration>::setOverflow(bool high) {
    setOverflow<SetOverflowEnabled>(high);
}

template <class TConfiguration>
ReadWrite Chip<TConfiguration>::getReadWriteSignal() const {
    return _readWrite;
}

template <class TConfiguration>
bool Chip<TConfiguration>::getSyncSignal() const {
    return _sync;
}

template <class TConfiguration>
bool Chip<TConfiguration>::getM1Signal() const {
    return !_phi2;
}

template <class TConfiguration>
bool Chip<TConfiguration>::getM2Signal() const {
    return _phi2;
}

// Memory

template <class TConfiguration>
void Chip<TConfiguration>::readDataBus(uint8_t low, uint8_t high) {
    // Write address bus registers
    _addressBusLow = low;
    _addressBusHigh = high;
    
    // Set address bus
    _bus.setAddressBus((_addressBusHigh << 8) | _addressBusLow);
    
    // Set R/W to read
    _readWrite = ReadWrite::Read;
}

template <class TConfiguration>
void Chip<TConfiguration>::writeDataBus(uint8_t low, uint8_t high, uint8_t data) {
    // Write address bus registers
    _addressBusLow = low;
    _addressBusHigh = high;
    
    // Set address bus
    _bus.setAddressBus((_addressBusHigh << 8) | _addressBusLow);
    
    // Write data bus, emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //_dataOutput &= data;
    _dataOutput = data;
    
    // Set R/W to write (unless it's in reset)
    _readWrite = (_resetRequested) ? ReadWrite::Read : ReadWrite::Write;
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
    _programCounterHigh += ((_programCounterLow == 0) && _programCounterNeedsIncrement);
    
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
    if (_interruptRequested) {
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

template <class TConfiguration>
void Chip<TConfiguration>::decodeOpcodeAndExecuteInstruction() {
    // We need this because some instructions will perform actions in the decode step of the next instruction, so we need to let them perform actions even if RDY is low, but we can't decode next opcode because opcode is not ready for reading
    /*if (_readyWaitRequested) {//TODO: remettre si execution pendant rdy low !!!
        return;
    }*/
    
    // Get current instruction from opcode (or BRK (0) if interrupt requested)
    _instruction = (_interruptRequested) ? 0 : _predecode;
    _currentInstruction = _opcodeInstructionFuncs[_instruction];
    
    // Execute current instruction
    (this->*_currentInstruction)();
    
    // Set sync low
    _sync = false;
}

// Overflow

template <class TConfiguration>
template <bool BSetOverflowEnabled, typename std::enable_if<BSetOverflowEnabled, int>::type>
void Chip<TConfiguration>::setOverflow(bool high) {
    // Save signal
    _setOverflowLine = high;
}

template <class TConfiguration>
template <bool BSetOverflowEnabled, typename std::enable_if<!BSetOverflowEnabled, int>::type>
void Chip<TConfiguration>::setOverflow(bool high) {
    static_assert(BSetOverflowEnabled, "Can't call setOverflow on a non-supported setOverflow CPU");
}

template <class TConfiguration>
template <bool BSetOverflowEnabled, typename std::enable_if<BSetOverflowEnabled, int>::type>
void Chip<TConfiguration>::checkSetOverflow() {
    // If setOverflow requested
    if (_setOverflowRequested) {
        // Enable overflow flag
        _flagsHelper.set<Flag::Overflow>(true);
        
        // Reset setOverflow requested
        _setOverflowRequested = false;
    }
    
    // Set overflow is requested if setOverflow line has transition from high to low
    if (_setOverflowLinePrevious && (!_setOverflowLine)) {
        _setOverflowRequested = true;
    }
    
    // Save previous setOverflow signal
    _setOverflowLinePrevious = _setOverflowLine;
}

template <class TConfiguration>
template <bool BSetOverflowEnabled, typename std::enable_if<!BSetOverflowEnabled, int>::type>
void Chip<TConfiguration>::checkSetOverflow() {
    // Does nothing
}

// Ready

template <class TConfiguration>
void Chip<TConfiguration>::checkReady() {
    // A ready wait is requested if the ready line is low
    _readyWaitRequested = !_readyLine && _resetLine;// TODO: voir si ok avec le reset
}

// Reset

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate, int>::type>
void Chip<TConfiguration>::resetAfterPowerUp() {
    // Reset current instruction
    _currentInstruction = nullptr;
    
    // Reset by pulling line down and calling checkReset to detect reset
    reset(false);
    checkReset<ResetAccurate>();
    
    // Release reset
    reset(true);
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<!BResetAccurate, int>::type>
void Chip<TConfiguration>::resetAfterPowerUp() {
    // Reset by pulling line down
    reset(false);
    
    // Release reset
    reset(true);
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate, int>::type>
void Chip<TConfiguration>::reset(bool high) {
    // Save signal
    _resetLine = high;
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<!BResetAccurate, int>::type>
void Chip<TConfiguration>::reset(bool high) {
    // If line is low and reset is not already requested
    if ((!high) && (!_resetRequested)) {
        // Set reset requested and execute reset0 for the next instruction
        _currentInstruction = &Chip::reset0<ResetAccurate>;
        _resetRequested = true;
        _interruptRequested = true;
    }
    
    // Save signal
    _resetLine = high;
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate, int>::type>
void Chip<TConfiguration>::checkReset() {
    // If reset signal is low
    if (!_resetLineLatch) {
        // If not already requested, save next instruction
        if (!_resetRequested) {
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
template <bool BResetAccurate, typename std::enable_if<!BResetAccurate, int>::type>
void Chip<TConfiguration>::checkReset() {
    // Does nothing
}

// Interrupts

template <class TConfiguration>
void Chip<TConfiguration>::checkNmi() {
    // Save previous nmi latched
    _nmiLatchedPrevious = _nmiLatched;
    
    // Because code which call nmi() can set the line low in phi1 and not call it in phi2, we need to check nmi line in phi2 with its current line value
    nmi(_nmiLine);
}

template <class TConfiguration>
void Chip<TConfiguration>::checkIrq() {
    // Because code which call irq() can set the line low in phi1 and not call it in phi2, we need to check irq line in phi2 with its current line value
    irq(_irqLine);
}

template <class TConfiguration>
void Chip<TConfiguration>::checkInterrupts() {
    // Set interrupt requested flag if interrupts detected
    if (_nmiRequested || _irqRequested) {
        _interruptRequested = true;
    }
}

template <class TConfiguration>
int Chip<TConfiguration>::getCurrentInterruptVectorsIndex() {
    // If reset
    if (_resetRequested) {
        return static_cast<int>(Interrupts::Reset);
    }
    
    // If nmi
    if (_nmiRequested) {
        return static_cast<int>(Interrupts::Nmi);
    }
    
    // Irq/Brk
    return static_cast<int>(Interrupts::IrqBrk);
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate, int>::type>
void Chip<TConfiguration>::correctInterruptVectorIndexForReset() {
    // Get reset vector if we are in reset
    if (_resetRequested) {
        _interruptVectorsIndex = static_cast<int>(Interrupts::Reset);
    }
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<!BResetAccurate, int>::type>
void Chip<TConfiguration>::correctInterruptVectorIndexForReset() {
    // Does nothing
}

// Stack

template <class TConfiguration>
void Chip<TConfiguration>::startStackOperation() {
    // Copy stack address in address bus registers
    readDataBus(_stackPointer, _stackPageNumber);
}

template <class TConfiguration>
void Chip<TConfiguration>::stopStackOperation() {
    // Save stack pointer
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
    // Save calculated address
    _addressBusLow = _alu.getAdderHold();
}

template <class TConfiguration>
void Chip<TConfiguration>::pullFromStack0() {
    // Adding 1 with stackPointer using ALU (Add 0 with carry set like true 6502)
    _alu.performSum<DecimalSupported, false>(0x0, _addressBusLow, false, true);
}

template <class TConfiguration>
void Chip<TConfiguration>::pullFromStack1() {
    // Read in stack
    readDataBus(_alu.getAdderHold(), _addressBusHigh);
}

#endif /* Cpu6502_Internal_Chip_s_hpp */
