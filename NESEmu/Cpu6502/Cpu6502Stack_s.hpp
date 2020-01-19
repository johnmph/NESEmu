//
//  Cpu6502Stack_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Stack_s_hpp
#define Cpu6502Stack_s_hpp


template <class TBus>
void Cpu6502<TBus>::pha0() {
    _currentInstruction = &Cpu6502::pha1;
    
    implied();
}

template <class TBus>
void Cpu6502<TBus>::pha1() {
    _currentInstruction = &Cpu6502::pha2;
    
    startStackOperation();
    pushToStack0(_accumulator);
}

template <class TBus>
void Cpu6502<TBus>::pha2() {
    pushToStack1();
    stopStackOperation();
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::php0() {
    _currentInstruction = &Cpu6502::php1;
    
    implied();
}

template <class TBus>
void Cpu6502<TBus>::php1() {
    _currentInstruction = &Cpu6502::php2;
    
    startStackOperation();
    pushToStack0(_statusFlags | (1 << static_cast<int>(Flag::Break)));
}

template <class TBus>
void Cpu6502<TBus>::php2() {
    pushToStack1();
    stopStackOperation();
    
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::pla0() {
    _currentInstruction = &Cpu6502::pla1;
    
    implied();
}

template <class TBus>
void Cpu6502<TBus>::pla1() {
    _currentInstruction = &Cpu6502::pla2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    pullFromStack0();
}

template <class TBus>
void Cpu6502<TBus>::pla2() {
    _currentInstruction = &Cpu6502::pla3;
    
    pullFromStack1();
    stopStackOperation();
}

template <class TBus>
void Cpu6502<TBus>::pla3() {
    _accumulator = _inputDataLatch;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_accumulator);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::plp0() {
    _currentInstruction = &Cpu6502::plp1;
    
    implied();
}

template <class TBus>
void Cpu6502<TBus>::plp1() {
    _currentInstruction = &Cpu6502::plp2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    pullFromStack0();
}

template <class TBus>
void Cpu6502<TBus>::plp2() {
    _currentInstruction = &Cpu6502::plp3;
    
    pullFromStack1();
    stopStackOperation();
}

template <class TBus>
void Cpu6502<TBus>::plp3() {
    _statusFlags = (_inputDataLatch & FlagsHelper::getDisableMask<Flag::Break>()) | (1 << static_cast<int>(Flag::UnusedHigh));
    
    fetchOpcode();
}

#endif /* Cpu6502Stack_s_hpp */
