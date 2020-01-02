//
//  Cpu6502_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 31/12/19.
//  Copyright © 2019 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_s_hpp
#define Cpu6502_s_hpp

//TODO: peut etre plutot avoir dans chaque instruction la liste de sous-instruction (avec if (_pipelineStep == 0) ...)
template <class TBus>
void Cpu6502<TBus>::clv() {
    clearStatusFlags({ Flags::Overflow });
    
    implied();
}

template <class TBus>
void Cpu6502<TBus>::lda() {
    _accumulator = _inputDataLatch;
    
    clearStatusFlags({ Flags::Zero, Flags::Negative });
    setStatusFlag(Flags::Zero, _accumulator == 0);
    setStatusFlag(Flags::Negative, _accumulator & 0x80);
    
    notImplied();
}

template <class TBus>
void Cpu6502<TBus>::staAbsolute() {
    sta();
    absolute_store();
    _pipelineStep = 0;  //TODO: ici ou dans absolute_store?? (si tous apres absolute_store, le mettre dedans)
}

template <class TBus>
void Cpu6502<TBus>::sta() {
    _dataOutput = _accumulator;
}

template <class TBus>
void Cpu6502<TBus>::bcs() {
    relative_0();
    
    if (getStatusFlag(Flags::Carry) == true) {
        relative_branch_0();
        return;
    }
    
    implied();
}



template <class TBus>
void Cpu6502<TBus>::clock() {
    // Fetch a new instruction
    if (_pipelineStep == 0) {
        fetchOpcode();
    }
    // Decode current instruction
    else if (_pipelineStep == 1) {
        decodeOpcode();
        _instrPipelineStartIndex = _instrPipelineStartIndexes[_instruction];
    }
    // Execute instruction current step
    else {
        _instrPipelineFuncs[_instrPipelineStartIndex + (_pipelineStep - 2)]();
    }
    
    ++_pipelineStep;
}

template <class TBus>
void Cpu6502<TBus>::nmi(bool high) {
}

template <class TBus>
void Cpu6502<TBus>::irq(bool high) {
}

template <class TBus>
void Cpu6502<TBus>::reset(bool high) {
    //TBus::read();
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
    _inputDataLatch = TBus::read((high << 8) | low);
    _predecode = _inputDataLatch;
}

template <class TBus>
void Cpu6502<TBus>::writeDataBus(uint8_t low, uint8_t high) {
    TBus::write((high << 8) | low, _dataOutput);
}

template <class TBus>
void Cpu6502<TBus>::fetchOpcode() {
    readDataBus(_programCounterLow, _programCounterHigh);
    incrementProgramCounter();
}

template <class TBus>
void Cpu6502<TBus>::decodeOpcode() {
    _instruction = _predecode;  // TODO: voir avec interruption ici (BRK)
    
    fetchOpcode();
}


template <class TBus>
void Cpu6502<TBus>::implied() {
    _pipelineStep = 1;
    clock();
}

template <class TBus>
void Cpu6502<TBus>::notImplied() {
    _pipelineStep = 0;
    clock();
}

template <class TBus>
void Cpu6502<TBus>::absolute_0() {
    _addressBusLow = _inputDataLatch;
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::absolute_load() {
    _addressBusHigh = _inputDataLatch;
    
    readDataBus(_addressBusLow, _addressBusHigh);
}

template <class TBus>
void Cpu6502<TBus>::absolute_store() {
    _addressBusHigh = _inputDataLatch;
    
    writeDataBus(_addressBusLow, _addressBusHigh);
}


template <class TBus>
void Cpu6502<TBus>::zeroPage_load() {
    _addressBusLow = _inputDataLatch;
    
    readDataBus(_addressBusLow, 0);
}

template <class TBus>
void Cpu6502<TBus>::zeroPage_store() {
    _addressBusLow = _inputDataLatch;
    
    writeDataBus(_addressBusLow, 0);
}


template <class TBus>
void Cpu6502<TBus>::relative_0() {
    _bInput = _inputDataLatch;
    _adderHold = _programCounterLow + static_cast<int8_t>(_bInput); //TODO: avoir un carry ici en utilisant l'alu pour faire cette addition
    
    //fetchOpcode();  // TODO: si branch non pris, il faudra appeler implied dans l'instruction de branchement pour passer directement au decodeOpcode
}

template <class TBus>
void Cpu6502<TBus>::relative_branch_0() {//TODO: a voir
    _programCounterLow = _adderHold;
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::relative_branch_1() {    // TODO: a voir
    if () { // Tester le carry ici
        if () {// Tester le sign ici
            --_programCounterHigh;
        }
        else {
            ++_programCounterHigh;
        }
        
        fetchOpcode();
        return;
    }
    
    decodeOpcode();
}


template <class TBus>
void Cpu6502<TBus>::absoluteIndexed_0() {
    immediate_0();
    _bal = _data;
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexed_1(uint8_t index) {
    immediate_0();
    _bah = _data;
    
    _bal += index;  // TODO: a la place d'avoir ca et la ligne d'en dessous, appeler un adc generique (ainsi je simule vraiment la réalité et j'ai tous les flags)
    setStatusFlag(Flags::Carry, _bal < index);
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedX_1() {
    absoluteIndexed_1(_xIndex);
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexedY_1() {
    absoluteIndexed_1(_yIndex);
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexed_load_0() {
    _data = TBus::read((_bah << 8) | _bal);
    
    // If we need to correct bah
    if (getStatusFlag(Flags::Carry) == true) {
        ++_bah;
        return;
    }
    
    // No need of extra step because we already have the correct data (bah was correct)
    ++_pipelineStep;
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexed_load_1() {
    _data = TBus::read((_bah << 8) | _bal);
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexed_store_0() {
    _data = TBus::read((_bah << 8) | _bal);
    
    // If we need to correct bah
    if (getStatusFlag(Flags::Carry) == true) {
        ++_bah;
    }
}

template <class TBus>
void Cpu6502<TBus>::absoluteIndexed_store_1() {
    TBus::write((_bah << 8) | _bal, _data);
}


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
}



#endif /* Cpu6502_s_hpp */
