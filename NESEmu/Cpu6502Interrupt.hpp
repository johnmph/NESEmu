//
//  Cpu6502Interrupt.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Interrupt_hpp
#define Cpu6502Interrupt_hpp


// BRK (+ IRQ/NMI/RESET), RTI

void startLow() {
    // If reset line stays low, continue this step
    if (_resetLine == false) {
        --_pipelineStep;
    }
}

void startHigh() {
}

void brk0() {    // TODO: a voir
    // Read data without increment PC for reset, nmi and irq
    if (_interruptRequested == true) {
        readDataBus(_programCounterLow, _programCounterHigh);
        return;
    }
    
    // Read data and increment PC for brk
    fetchData();
}

void brk1() {
    pushToStack0(_programCounterHigh);
}

void brk2() {
    pushToStack1();
    pushToStack0(_programCounterLow);
}

void brk3() {
    pushToStack1();
    pushToStack0(_statusFlags | ((!_interruptRequested) << static_cast<int>(Flags::Break))); // TODO: voir si ok
}

void brk4() {
    pushToStack1();
    
    // Disable interrupts
    setStatusFlag(Flags::InterruptDisable, true);
    
    // Calculate interrupt vectors index
    _interruptVectorsIndex = getCurrentInterruptVectorsIndex(); // TODO: peut etre plus besoin de ca car maintenant on a addressBusLow/High et on peut l'incrementer dans brk5
    
    // Read low byte of address
    readDataBus(_interruptVectors[_interruptVectorsIndex][0], _interruptVectors[_interruptVectorsIndex][1]);
}

void brk5() {
    // Store low byte of address to programCounterLow
    _programCounterLow = _inputDataLatch;
    
    // Read high byte of address
    readDataBus(_interruptVectors[_interruptVectorsIndex][0] + 1, _interruptVectors[_interruptVectorsIndex][1]);
}

void brk6() {
    // Store high byte of address to programCounterHigh
    _programCounterHigh = _inputDataLatch;
    
    // TODO: voir si ok : (car il faut voir si c'est possible d'avoir la detection d'une interruption dans le stage du brk)
    _nmiRequested = false;
    _resetRequested = false;
    _interruptRequested = false;
    
    // Fetch instruction
    finishInstruction();
}

#endif /* Cpu6502Interrupt_hpp */
