//
//  Interrupt_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Interrupt_s_hpp
#define Cpu6502_Internal_Interrupt_s_hpp


template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<BResetAccurate, int>::type>
void Chip<TConfiguration>::reset0() {
    // If we are right after power on, add a extra step and exit
    if (_resetSavedInstruction == nullptr) {
        _currentInstruction = &Chip::reset1;
        return;
    }
    
    // Execute saved instruction if it's not a fetchOpcode
    if (_resetSavedInstruction != static_cast<OpcodeInstruction>(&Chip::fetchOpcode)) {
        // Execute saved instruction
        (this->*_resetSavedInstruction)();
    } else {
        // Ensure that we are in read mode
        readDataBus(_addressBusLow, _addressBusHigh);
    }
    
    // Next instruction is fetchOpcode but need to check if resetRequested is still true in case of reset in interrupt
    if (_resetRequested) {
        _currentInstruction = &Chip::fetchOpcode;
    }
}

template <class TConfiguration>
template <bool BResetAccurate, typename std::enable_if<!BResetAccurate, int>::type>
void Chip<TConfiguration>::reset0() {
    // Stay in this step until reset goes high again
    if (_resetLine) {
        _currentInstruction = &Chip::reset1;
    }
}

template <class TConfiguration>
void Chip<TConfiguration>::reset1() {
    // Need this extra-step to sync number of clocks with the ResetAccurate version for exiting the reset state
    _currentInstruction = &Chip::fetchOpcode;
}

template <class TConfiguration>
void Chip<TConfiguration>::brk0() {
    _currentInstruction = &Chip::brk1;
    
    // Read data without increment PC for reset, nmi and irq
    if (_interruptRequested) {
        readDataBus(_programCounterLow, _programCounterHigh);
        
        return;
    }
    
    // Read data and increment PC for brk
    fetchData();
}

template <class TConfiguration>
void Chip<TConfiguration>::brk1() {
    _currentInstruction = &Chip::brk2;
    
    // Push PCH to stack
    startStackOperation();
    pushToStack0(_programCounterHigh);
}

template <class TConfiguration>
void Chip<TConfiguration>::brk2() {
    _currentInstruction = &Chip::brk3;
    
    // Push PCL to stack
    pushToStack1();
    pushToStack0(_programCounterLow);
}

template <class TConfiguration>
void Chip<TConfiguration>::brk3() {
    _currentInstruction = &Chip::brk4;
    
    // Push status flags to stack
    pushToStack1();
    pushToStack0(_statusFlags | ((!_interruptRequested) << static_cast<int>(Flag::Break)));
    
    // Calculate interrupt vectors index
    _interruptVectorsIndex = getCurrentInterruptVectorsIndex(); // TODO: avant c'etait dans brk4 mais ca posait un probleme car ca detectait le nmi meme s'il etait trop tard (a la fin de brk3), alors déplacé ici, normalement c'est ok
}

template <class TConfiguration>
void Chip<TConfiguration>::brk4() {
    _currentInstruction = &Chip::brk5;
    
    // Finish stack operation
    pushToStack1();
    stopStackOperation();
    
    // Calculate interrupt vectors index
    //_interruptVectorsIndex = getCurrentInterruptVectorsIndex();//TODO: déplacé dans brk3, voir ci dessus
    
    // Read low byte of address
    readDataBus(_interruptVectors[_interruptVectorsIndex][0], _interruptVectors[_interruptVectorsIndex][1]);
}

template <class TConfiguration>
void Chip<TConfiguration>::brk5() {
    _currentInstruction = &Chip::brk6;
    
    // Disable interrupts
    _flagsHelper.set<Flag::InterruptDisable>(true);
    
    // 6502 uses the ALU to store temporary low byte of address (by adding 0 to it to keep its value in adderHold)
    _alu.performSum<DecimalSupported, false>(0x0, _inputDataLatch, false, false);
    
    // Possibly correct vector index for reset
    correctInterruptVectorIndexForReset<ResetAccurate>();
    
    // Read high byte of address
    readDataBus(_interruptVectors[_interruptVectorsIndex][0] + 1, _interruptVectors[_interruptVectorsIndex][1]);
    
    // Reset interrupts requested flag, since here it can redetected interrupts but no checkInterrupts because interrupt sequences themselves do not perform interrupt polling
    // See https://wiki.nesdev.com/w/index.php/CPU_interrupts
    _irqRequested = false;
    _nmiRequested = false;
    _resetRequested = false;
    _interruptRequested = false;
}

template <class TConfiguration>
void Chip<TConfiguration>::brk6() {
    // Set PC
    _programCounterLow = _alu.getAdderHold();
    _programCounterHigh = _inputDataLatch;
    
    // Fetch next opcode
    fetchOpcode();
}

template <class TConfiguration>
void Chip<TConfiguration>::rti0() {
    _currentInstruction = &Chip::rti1;
    
    // Even if it's a implied addressing mode, PC is incremented when reading memory
    fetchData();
}

template <class TConfiguration>
void Chip<TConfiguration>::rti1() {
    _currentInstruction = &Chip::rti2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    pullFromStack0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rti2() {
    _currentInstruction = &Chip::rti3;
    
    // Pull status flag from stack
    pullFromStack1();
    pullFromStack0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rti3() {
    _currentInstruction = &Chip::rti4;
    
    // Set status flag
    _statusFlags = (_inputDataLatch & _Detail::FlagsHelper::getDisableMask<Flag::Break>()) | (1 << static_cast<int>(Flag::UnusedHigh));
    
    // Pull PCL from stack
    pullFromStack1();
    pullFromStack0();
}

template <class TConfiguration>
void Chip<TConfiguration>::rti4() {
    _currentInstruction = &Chip::rti5;
    
    // Pull PCH from stack and finish stack operation, need to put before ALU operation because adderHold holds stackPointer
    pullFromStack1();
    stopStackOperation();
    
    // 6502 uses the ALU to store temporary low byte of address (by adding 0 to it to keep its value in adderHold)
    _alu.performSum<DecimalSupported, false>(0x0, _inputDataLatch, false, false);
    
    // Check interrupts
    checkInterrupts();
}

template <class TConfiguration>
void Chip<TConfiguration>::rti5() {
    // Set PC
    _programCounterLow = _alu.getAdderHold();
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

#endif /* Cpu6502_Internal_Interrupt_s_hpp */
