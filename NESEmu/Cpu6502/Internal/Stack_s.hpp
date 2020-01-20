//
//  Stack_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Stack_s_hpp
#define Cpu6502_Internal_Stack_s_hpp


template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::pha0() {
    _currentInstruction = &Chip::pha1;
    
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::pha1() {
    _currentInstruction = &Chip::pha2;
    
    startStackOperation();
    pushToStack0(_accumulator);
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::pha2() {
    pushToStack1();
    stopStackOperation();
    
    fetchOpcode();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::php0() {
    _currentInstruction = &Chip::php1;
    
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::php1() {
    _currentInstruction = &Chip::php2;
    
    startStackOperation();
    pushToStack0(_statusFlags | (1 << static_cast<int>(Flag::Break)));
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::php2() {
    pushToStack1();
    stopStackOperation();
    
    fetchOpcode();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::pla0() {
    _currentInstruction = &Chip::pla1;
    
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::pla1() {
    _currentInstruction = &Chip::pla2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    pullFromStack0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::pla2() {
    _currentInstruction = &Chip::pla3;
    
    pullFromStack1();
    stopStackOperation();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::pla3() {
    _accumulator = _inputDataLatch;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_accumulator);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::plp0() {
    _currentInstruction = &Chip::plp1;
    
    implied();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::plp1() {
    _currentInstruction = &Chip::plp2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    pullFromStack0();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::plp2() {
    _currentInstruction = &Chip::plp3;
    
    pullFromStack1();
    stopStackOperation();
}

template <class TBus, bool DecimalSupported>
void Chip<TBus, DecimalSupported>::plp3() {
    _statusFlags = (_inputDataLatch & _Detail::FlagsHelper::getDisableMask<Flag::Break>()) | (1 << static_cast<int>(Flag::UnusedHigh));
    
    fetchOpcode();
}

#endif /* Cpu6502_Internal_Stack_s_hpp */
