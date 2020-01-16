//
//  Cpu6502Interrupt_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Interrupt_s_hpp
#define Cpu6502Interrupt_s_hpp


template <class TBus>
void Cpu6502<TBus>::reset0() {
    // If reset line stays low, continue this step, else go to next step
    if (_resetLine == true) {
        _currentInstruction = &Cpu6502::reset1;
    }
}

template <class TBus>
void Cpu6502<TBus>::reset1() {
    _currentInstruction = &Cpu6502::brk0;
}

template <class TBus>
void Cpu6502<TBus>::brk0() {    // TODO: a voir
    _currentInstruction = &Cpu6502::brk1;
    
    // Read data without increment PC for reset, nmi and irq
    if (_interruptRequested == true) {
        readDataBus(_programCounterLow, _programCounterHigh);   // TODO: voir si vraiment nécessaire avec le nouveau systeme
        
        return;
    }
    
    // Read data and increment PC for brk
    fetchData();
}

template <class TBus>
void Cpu6502<TBus>::brk1() {
    _currentInstruction = &Cpu6502::brk2;
    
    // Push PCH to stack
    pushToStack0(_programCounterHigh);
}

template <class TBus>
void Cpu6502<TBus>::brk2() {
    _currentInstruction = &Cpu6502::brk3;
    
    // Finish stack operation and push PCL to stack
    pushToStack1();
    pushToStack0(_programCounterLow);
}

template <class TBus>
void Cpu6502<TBus>::brk3() {
    _currentInstruction = &Cpu6502::brk4;
    
    // Finish stack operation and push Status to stack
    pushToStack1();
    pushToStack0(_statusFlags | ((_interruptRequested == false) << static_cast<int>(Flag::Break))); // TODO: voir si ok
}

template <class TBus>
void Cpu6502<TBus>::brk4() {
    _currentInstruction = &Cpu6502::brk5;
    
    // Finish stack operation
    pushToStack1();
    
    // Disable interrupts
    _flagsHelper.set<Flag::InterruptDisable>(true);
    
    // Calculate interrupt vectors index
    _interruptVectorsIndex = getCurrentInterruptVectorsIndex(); // TODO: peut etre plus besoin de ca car maintenant on a addressBusLow/High et on peut l'incrementer dans brk5
    
    // Read low byte of address
    readDataBus(_interruptVectors[_interruptVectorsIndex][0], _interruptVectors[_interruptVectorsIndex][1]);
}

template <class TBus>
void Cpu6502<TBus>::brk5() {
    _currentInstruction = &Cpu6502::brk6;
    
    // Store low byte of address to programCounterLow
    _programCounterLow = _inputDataLatch;
    
    // Read high byte of address
    readDataBus(_interruptVectors[_interruptVectorsIndex][0] + 1, _interruptVectors[_interruptVectorsIndex][1]);
}

template <class TBus>
void Cpu6502<TBus>::brk6() {
    // Store high byte of address to programCounterHigh
    _programCounterHigh = _inputDataLatch;
    
    // TODO: voir si ok : (car il faut voir si c'est possible d'avoir la detection d'une interruption dans le stage du brk)
    _nmiRequested = false;
    _resetRequested = false;
    _interruptRequested = false;
    
    // Fetch next opcode
    fetchOpcode();
}

template <class TBus>
void Cpu6502<TBus>::rti0() {    // TODO: a voir
    _currentInstruction = &Cpu6502::rti1;
    
    implied();
}

template <class TBus>
void Cpu6502<TBus>::rti1() {
    _currentInstruction = &Cpu6502::rti2;
    
    pullFromStack0();
}

template <class TBus>
void Cpu6502<TBus>::rti2() {
    _currentInstruction = &Cpu6502::rti3;
    
    pullFromStack1();
    pullFromStack0();
}

template <class TBus>
void Cpu6502<TBus>::rti3() {
    _currentInstruction = &Cpu6502::rti4;
    
    _statusFlags = (_inputDataLatch & FlagsHelper::getDisableMask<Flag::Break>()) | (1 << static_cast<int>(Flag::UnusedHigh));
    
    pullFromStack1();
    pullFromStack0();
}

template <class TBus>
void Cpu6502<TBus>::rti4() {
    _currentInstruction = &Cpu6502::rti5;
    
    _programCounterLow = _inputDataLatch;
    
    pullFromStack1();
}

template <class TBus>
void Cpu6502<TBus>::rti5() {
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

#endif /* Cpu6502Interrupt_s_hpp */
