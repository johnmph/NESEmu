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
template <class TBus>
const typename Cpu6502<TBus>::InstructionPipeline Cpu6502<TBus>::_instrPipelineFuncs[256] = {
    &Cpu6502::implied, &Cpu6502::clv,                                                                // Test implied
    &Cpu6502::immediate, &Cpu6502::lda,                                                              // Test immediate read
    &Cpu6502::absolute0, &Cpu6502::absolute1, &Cpu6502::absoluteLoad, &Cpu6502::lda,                 // Test absolute read
    &Cpu6502::absolute0, &Cpu6502::absolute1, &Cpu6502::staAbsolute, &Cpu6502::finishInstruction,    // Test absolute write
    &Cpu6502::zeroPage0, &Cpu6502::zeroPageLoad, &Cpu6502::lda,                                      // Test zero page read
    &Cpu6502::zeroPage0, &Cpu6502::staZeroPage, &Cpu6502::finishInstruction,                         // Test zero page write
    &Cpu6502::relative, &Cpu6502::bcs, &Cpu6502::relativeBranch0, &Cpu6502::relativeBranch1,         // Test relative branch    // TODO: a tester apres quand fini alu
    &Cpu6502::absoluteIndexed0, &Cpu6502::absoluteIndexedX1, &Cpu6502::absoluteIndexedLoad0, &Cpu6502::absoluteIndexedLoad1, &Cpu6502::lda,                 // Test absolute indexed X read
    &Cpu6502::absoluteIndexed0, &Cpu6502::absoluteIndexedY1, &Cpu6502::staAbsoluteIndexed, &Cpu6502::absoluteIndexedStore1, &Cpu6502::finishInstruction,  // Test absolute indexed Y write
    &Cpu6502::zeroPageIndexed0, &Cpu6502::zeroPageIndexedX1, &Cpu6502::zeroPageIndexedLoad, &Cpu6502::lda, // Test zero page indexed X read
    &Cpu6502::zeroPageIndexed0, &Cpu6502::zeroPageIndexedY1, &Cpu6502::staZeroPageIndexed, &Cpu6502::finishInstruction, // Test zero page indexed Y write
    &Cpu6502::brk0, &Cpu6502::brk1, &Cpu6502::brk2, &Cpu6502::brk3, &Cpu6502::brk4, &Cpu6502::brk5, &Cpu6502::brk6,    // Brk
    &Cpu6502::startLow, &Cpu6502::startHigh0and1, &Cpu6502::startHigh0and1, &Cpu6502::startHigh2, &Cpu6502::startHigh3, &Cpu6502::startHigh4, &Cpu6502::startHigh5, &Cpu6502::startHigh6, &Cpu6502::startHigh7,                             // Start sequence
};

template <class TBus>
const int Cpu6502<TBus>::_instrPipelineStartIndexes[256] = { 0, 2, 4, 8, 12, 15, 18, 22, 27, 32, 36, 41, 48, 57 };

template <class TBus>
const uint8_t Cpu6502<TBus>::_interruptVectors[3][2] = {
    0xFA, 0xFF,     // NMI (Prior over IRQ/BRK but not Reset)
    0xFC, 0xFF,     // Reset (Higher priority)
    0xFE, 0xFF,     // IRQ, BRK (Lower priority)
};

template <class TBus>
const uint8_t Cpu6502<TBus>::_stackPageNumber = 0x1;

//TODO: peut etre plutot avoir dans chaque instruction la liste de sous-instruction (avec if (_pipelineStep == 0) ...)
template <class TBus>
void Cpu6502<TBus>::clv() {
    clearStatusFlags({ Flags::Overflow });
    
    finishInstruction();
}

template <class TBus>
void Cpu6502<TBus>::lda() {
    _accumulator = _inputDataLatch;
    
    clearStatusFlags({ Flags::Zero, Flags::Negative }); // TODO: il parait que les flags ne sont pas mis a jour dans le meme cycle!! a voir
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

template <class TBus>
void Cpu6502<TBus>::brk0() {    // TODO: voir si ca fait vraiment ca parce que je vois pas pq
    // Read opcode without increment PC
    readDataBus(_programCounterLow, _programCounterHigh);
}

template <class TBus>
void Cpu6502<TBus>::brk1() {
    pushToStack(_programCounterHigh);
}

template <class TBus>
void Cpu6502<TBus>::brk2() {
    pushToStack(_programCounterLow);
}

template <class TBus>
void Cpu6502<TBus>::brk3() {
    pushToStack(_statusFlags | ((!(_nmiRequested | _irqRequested)) << Flags::Break));      // TODO: voir si c'est bon
}

template <class TBus>
void Cpu6502<TBus>::brk4() {
    // Disable interrupts
    setStatusFlag(Flags::InterruptDisable, true);
    
    checkNmi();
    
    // If Nmi requested
    if (_nmiRequested == true) {
        // Read low byte of address
        readDataBus(_interruptVectors[static_cast<int>(Interrupts::Nmi)][0], _interruptVectors[static_cast<int>(Interrupts::Nmi)][1]);
    }
    // Irq / Brk
    else {
        // Read low byte of address
        readDataBus(_interruptVectors[static_cast<int>(Interrupts::IrqBrk)][0], _interruptVectors[static_cast<int>(Interrupts::IrqBrk)][1]);
    }
}

template <class TBus>
void Cpu6502<TBus>::brk5() {
    // Store low byte of address to programCounterLow
    _programCounterLow = _inputDataLatch;
    
    // If Nmi requested
    if (_nmiRequested == true) {
        // Read high byte of address
        readDataBus(_interruptVectors[static_cast<int>(Interrupts::Nmi)][0] + 1, _interruptVectors[static_cast<int>(Interrupts::Nmi)][1]);
    }
    // Irq / Brk
    else {
        // Read high byte of address
        readDataBus(_interruptVectors[static_cast<int>(Interrupts::IrqBrk)][0] + 1, _interruptVectors[static_cast<int>(Interrupts::IrqBrk)][1]);
    }
}

template <class TBus>
void Cpu6502<TBus>::brk6() {
    // Store high byte of address to programCounterHigh
    _programCounterHigh = _inputDataLatch;
    
    // TODO: voir si ok :
    _nmiRequested = false;
    _irqRequested = false;
    
    // Fetch instruction
    finishInstruction();
}


template <class TBus>
void Cpu6502<TBus>::startLow() {
    readDataBus(_programCounterLow, _programCounterHigh);
}

template <class TBus>
void Cpu6502<TBus>::startHigh0and1() {
    readDataBus(_programCounterLow, _programCounterHigh);
}

template <class TBus>
void Cpu6502<TBus>::startHigh2() {
    pushToStack(_programCounterHigh);   // TODO: durant le start, les write sont desactivé, ca fait donc un read
}

template <class TBus>
void Cpu6502<TBus>::startHigh3() {
    pushToStack(_programCounterLow);    // TODO: durant le start, les write sont desactivé, ca fait donc un read
}

template <class TBus>
void Cpu6502<TBus>::startHigh4() {
    pushToStack(_statusFlags);          // TODO: durant le start, les write sont desactivé, ca fait donc un read
}

template <class TBus>
void Cpu6502<TBus>::startHigh5() {
    // Disable interrupts
    setStatusFlag(Flags::InterruptDisable, true);
    
    // Read low byte of address
    readDataBus(_interruptVectors[static_cast<int>(Interrupts::Reset)][0], _interruptVectors[static_cast<int>(Interrupts::Reset)][1]);
}

template <class TBus>
void Cpu6502<TBus>::startHigh6() {
    // Store low byte of address to programCounterLow
    _programCounterLow = _inputDataLatch;
    
    // Read high byte of address
    readDataBus(_interruptVectors[static_cast<int>(Interrupts::Reset)][0] + 1, _interruptVectors[static_cast<int>(Interrupts::Reset)][1]);
}

template <class TBus>
void Cpu6502<TBus>::startHigh7() {/*
    // Store high byte of address to programCounterHigh
    _programCounterHigh = _inputDataLatch;
    
    // Fetch instruction
    finishInstruction();*/
    brk6();
}




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



template <class TBus>
void Cpu6502<TBus>::branch(bool condition) {
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


template <class TBus>
Cpu6502<TBus>::Cpu6502(TBus &bus) : _bus(bus), _programCounterLow(0xFF), _programCounterHigh(0), _stackPointer(0), _accumulator(0xAA), _xIndex(0), _yIndex(0), _statusFlags(0x2), _resetLine(false), _nmiLine(true), _nmiLinePrevious(true), _nmiRequested(false), _irqLine(true), _irqRequested(false) {
}

template <class TBus>
void Cpu6502<TBus>::clock() {
    // If reset
    if (_resetLine == false) {
        _instrPipelineStartIndex = _instrPipelineStartIndexes[12];  // TODO: mettre l'opcode start index de la routine de reset (un opcode qui n'existe pas dans le 6502)
        _pipelineStep = 0;
    } else {
        // Nmi is checked each clock
        checkNmi();
        
        // Decode current instruction
        if (_pipelineStep == 0) {
            decodeOpcode();
        }
    }
    
    // Execute current stage
    (this->*_instrPipelineFuncs[_instrPipelineStartIndex + _pipelineStep])();
    ++_pipelineStep;
}

template <class TBus>
void Cpu6502<TBus>::reset(bool high) {  // TODO: voir pour le status flag si on le laisse ainsi ou si on le reset : normalement on le laisse ainsi
    // Save signal
    _resetLine = high;
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
    _statusFlags |= value << static_cast<uint8_t>(flag);
}

template <class TBus>
void Cpu6502<TBus>::incrementProgramCounter() {
    ++_programCounterLow;
    _programCounterHigh += (_programCounterLow == 0);
}

template <class TBus>
void Cpu6502<TBus>::readDataBus(uint8_t low, uint8_t high) {
    _inputDataLatch = _bus.read((high << 8) | low);
    _predecode = _inputDataLatch;
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
void Cpu6502<TBus>::checkNmi() {
    // Nmi is requested if nmi line has transition from high to low
    if ((_nmiLinePrevious == true) && (_nmiLine == false)) {
        _nmiRequested = true;
    }
    
    // Save previous nmi signal
    _nmiLinePrevious = _nmiLine;
}

template <class TBus>
bool Cpu6502<TBus>::checkInterrupts() {
    // If Nmi requested
    if (_nmiRequested == true) {
        return true;
    }
    
    // If Irq requested
    if ((getStatusFlag(Flags::InterruptDisable) == false) && (_irqLine == false)) {
        _irqRequested = true;
        return true;
    }
    
    return false;
}

template <class TBus>
void Cpu6502<TBus>::finishInstruction() {
    fetchOpcode();
}


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
void Cpu6502<TBus>::zeroPage0() {
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
    zeroPage0();
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

#endif /* Cpu6502_s_hpp */
