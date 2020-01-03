//
//  Cpu6502_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 31/12/19.
//  Copyright © 2019 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_s_hpp
#define Cpu6502_s_hpp


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
};

template <class TBus>
const uint8_t Cpu6502<TBus>::_instrPipelineStartIndexes[256] = { 0, 2, 4, 8, 12, 15, 18, 22, 27, 32 };

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
void Cpu6502<TBus>::sta() {
    _dataOutput = _accumulator;
}

template <class TBus>
void Cpu6502<TBus>::bcs() {
    branch(getStatusFlag(Flags::Carry) == true);
}

template <class TBus>
void Cpu6502<TBus>::aluPerformSum(bool decimalEnable, bool carryIn) {//TODO: terminer
    _adderHold = _aInput + _bInput + carryIn;
    
    _aluOverflow = _adderHold & 0x80;
    _aluCarry = false;
    _aluHalfCarry = false;
}

template <class TBus>
void Cpu6502<TBus>::branch(bool condition) {
    // Calculate addition of programCounterLow and offset
    _aInput = _programCounterLow;
    _bInput = _inputDataLatch;
    aluPerformSum(false, false);
    //_adderHold = _programCounterLow + static_cast<int8_t>(_bInput); //TODO: avoir un carry ici en utilisant l'alu pour faire cette addition
    
    if (condition == true) {
        // Branch is taken, this data will not be used
        fetchData();
        return;
    }
    
    // Branch is not taken, read next instruction
    finishInstruction();
}


template <class TBus>
Cpu6502<TBus>::Cpu6502(TBus &bus) : _bus(bus) {
}

template <class TBus>
void Cpu6502<TBus>::clock() {
    // Decode current instruction
    if (_pipelineStep == 0) {
        decodeOpcode();
    }
    
    // Execute current stage
    (*this.*_instrPipelineFuncs[_instrPipelineStartIndex + _pipelineStep])();
    ++_pipelineStep;
}

template <class TBus>
void Cpu6502<TBus>::nmi(bool high) {
}

template <class TBus>
void Cpu6502<TBus>::irq(bool high) {
}

template <class TBus>
void Cpu6502<TBus>::reset(bool high) {  // TODO: a changer, pour tests
    _programCounterLow = 0;
    _programCounterHigh = 0;
    
    _stackPointer = 0;
    _accumulator = 0;
    _xIndex = 1;
    _yIndex = 2;
    _statusFlags = 0;
    
    fetchOpcode();
    _pipelineStep = 0;
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
/*
template <class TBus>
uint16_t Cpu6502<TBus>::getProgramCounter() {
    return (_programCounterHigh << 8) | _programCounterLow;
}*/

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
    fetchData();
    _pipelineStep = -1;
}

template <class TBus>
void Cpu6502<TBus>::decodeOpcode() {
    _instruction = _predecode;  // TODO: voir avec interruption ici (BRK)
    _instrPipelineStartIndex = _instrPipelineStartIndexes[_instruction];
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
    _addressBusHigh = _inputDataLatch;
    
    readDataBus(_addressBusLow, _addressBusHigh);
}

template <class TBus>
void Cpu6502<TBus>::absoluteStore() {
    _addressBusHigh = _inputDataLatch;
    
    writeDataBus(_addressBusLow, _addressBusHigh);
}


template <class TBus>
void Cpu6502<TBus>::zeroPage0() {
    // Read low byte of address and increment PC
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageLoad() {
    _addressBusLow = _inputDataLatch;
    
    readDataBus(_addressBusLow, 0);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageStore() {
    _addressBusLow = _inputDataLatch;
    
    writeDataBus(_addressBusLow, 0);
}


template <class TBus>
void Cpu6502<TBus>::relative() {
    // Read offset and increment PC
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::relativeBranch0() {    // TODO: a voir
    _programCounterLow = _adderHold;
    
    if (_aluCarry == true) {
        if (false) {// TODO: Tester le sign ici
            --_programCounterHigh;
        }
        else {
            ++_programCounterHigh;
        }
        
        // There is a page boundary cross, this data will not be used (because its not the good address), we don't increment PC
        readDataBus(_programCounterLow, _programCounterHigh);
        return;
    }
    
    // There is no page boundary cross, so it's the correct data, we can fetch the instruction
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::relativeBranch1() {    // TODO: a voir
    fetchOpcode();
}



template <class TBus>
void Cpu6502<TBus>::absoluteIndexed0() {
    // Read low byte of address and increment PC
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexed1(uint8_t index) {
    // Store low byte of address in addressBusLow
    _addressBusLow = _inputDataLatch;
    
    // Read high byte of address and increment PC
    fetchData();
    
    _aInput = _addressBusLow;
    _bInput = index;
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
    _addressBusHigh = _inputDataLatch;
    
    readDataBus(_addressBusLow, _addressBusHigh);
    
    // If we need to correct addressBusHigh
    if (_aluCarry == true) {
        ++_addressBusHigh;
        return;
    }
    
    // No need of extra step because we already have the correct data (addressBusHigh was correct)
    ++_pipelineStep;
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedLoad1() {
    readDataBus(_addressBusLow, _addressBusHigh);
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedStore0() {
    _addressBusLow = _adderHold;
    _addressBusHigh = _inputDataLatch;
    
    readDataBus(_addressBusLow, _addressBusHigh);
    
    // If we need to correct addressBusHigh
    if (_aluCarry == true) {
        ++_addressBusHigh;
    }
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedStore1() {
    writeDataBus(_addressBusLow, _addressBusHigh);
}

/*
template <class TBus>
void Cpu6502<TBus>::zeroPageIndexed_0() {
    absoluteIndexed_0();
    _bah = 0;   // TODO: voir si ok
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndexed_1(uint8_t index) {
    _data = TBus::read(_bal);
    _bal += index;  // TODO: via le adc pour setter les flags carry, ... ?
}

template <class TBus>
uint8_t Cpu6502<TBus>::zeroPageIndexedX_1() {
    return zeroPageIndexed_1(_xIndex);
}

template <class TBus>
uint8_t Cpu6502<TBus>::zeroPageIndexedY_1() {
    return zeroPageIndexed_1(_yIndex);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndexed_load() {
    _data = TBus::read(_bal);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndexed_store() {
    TBus::write(_bal, _data);
}


template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirect_0() {
    zeroPageIndexed_0();
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirect_1() {
    zeroPageIndexedX_1();
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirect_2() {
    zeroPageIndexed_load();
    _adl = _data;
    ++_bal;
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirect_3() {
    zeroPageIndexed_load();
    _adh = _data;
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirect_load() {
    absolute_load();
}

template <class TBus>
void Cpu6502<TBus>::zeroPagePreIndexedIndirect_store() {
    absolute_store();
}



template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed_0() {
    zeroPageIndexed_0();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed_1() {
    zeroPagePreIndexedIndirect_2();
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed_2() {
    zeroPagePreIndexedIndirect_3();
    
    _adl += _yIndex;  // TODO: a la place d'avoir ca et la ligne d'en dessous, appeler un adc generique (ainsi je simule vraiment la réalité et j'ai tous les flags)
    setStatusFlag(Flags::Carry, _adl < _yIndex);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed_load_0() {
    _data = TBus::read((_adh << 8) | _adl);
    
    // If we need to correct adh
    if (getStatusFlag(Flags::Carry) == true) {
        ++_adh;
        return;
    }
    
    // No need of extra step because we already have the correct data (adh was correct)
    ++_pipelineStep;
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed_load_1() {
    _data = TBus::read((_adh << 8) | _adl);
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed_store_0() {
    _data = TBus::read((_adh << 8) | _adl);
    
    // If we need to correct adh
    if (getStatusFlag(Flags::Carry) == true) {
        ++_adh;
    }
}

template <class TBus>
void Cpu6502<TBus>::zeroPageIndirectPostIndexed_store_1() {
    TBus::write((_adh << 8) | _adl, _data);
}*/



#endif /* Cpu6502_s_hpp */
