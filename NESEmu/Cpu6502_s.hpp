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
void Cpu6502<TBus>::clock() {
    // Start a new instruction
    if (_pipelineStep == 0) {
        _instrPipelineStartIndex = _instrPipelineStartIndexes[_opcode];
    }
    
    _instrPipelineFuncs[_instrPipelineStartIndex + _pipelineStep]();
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
void Cpu6502<TBus>::fetchOpcode() {
    _pipelineStep = -1;//TODO: voir si ok
    
    _opcode = TBus::read(_programCounter);
    
    ++_programCounter;
}

template <class TBus>
void Cpu6502<TBus>::immediate_0() {
    _data = TBus::read(_programCounter);
    ++_programCounter;
}

template <class TBus>
void Cpu6502<TBus>::absolute_0() {
    immediate_0();
    _adl = _data;
}

template <class TBus>
void Cpu6502<TBus>::absolute_1() {
    immediate_0();
    _adh = _data;
}

template <class TBus>
void Cpu6502<TBus>::absolute_load() {
    _data = TBus::read((_adh << 8) | _adl);
}

template <class TBus>
void Cpu6502<TBus>::absolute_store() {
    TBus::write((_adh << 8) | _adl, _data);
}

template <class TBus>
void Cpu6502<TBus>::zeroPage_0() {
    absolute_0();
    _adh = 0;   // TODO: voir si ok
}

template <class TBus>
void Cpu6502<TBus>::zeroPage_load() {
    _data = TBus::read(_adl);
}

template <class TBus>
void Cpu6502<TBus>::zeroPage_store() {
    TBus::write(_adl, _data);
}


template <class TBus>
void Cpu6502<TBus>::relative_0() {
    immediate_0();
}

template <class TBus>
void Cpu6502<TBus>::relative_1() {    // TODO: a voir, il faut finir avec les pcl et pch
    _programCounter += static_cast<int8_t>(_data);//TODO: pas bon, il faut juste ajouter a pcl
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
