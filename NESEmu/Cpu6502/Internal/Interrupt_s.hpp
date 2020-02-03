//
//  Interrupt_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Interrupt_s_hpp
#define Cpu6502_Internal_Interrupt_s_hpp


template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::reset0() {
    // If reset line stays low, continue this step, else go to next step
    if (_resetLine == true) {
        _currentInstruction = &Chip::reset1;
    }
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::reset1() {
    _currentInstruction = &Chip::brk0;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::brk0() {
    _currentInstruction = &Chip::brk1;
    
    // Read data without increment PC for reset, nmi and irq
    if (_interruptRequested == true) {
        readDataBus(_programCounterLow, _programCounterHigh);
        
        return;
    }
    
    // Read data and increment PC for brk
    fetchData();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::brk1() {
    _currentInstruction = &Chip::brk2;
    
    // Push PCH to stack
    startStackOperation();
    pushToStack0(_programCounterHigh);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::brk2() {
    _currentInstruction = &Chip::brk3;
    
    // Push PCL to stack
    pushToStack1();
    pushToStack0(_programCounterLow);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::brk3() {
    _currentInstruction = &Chip::brk4;
    
    // Push status flags to stack
    pushToStack1();
    pushToStack0(_statusFlags | ((_interruptRequested == false) << static_cast<int>(Flag::Break))); // TODO: voir si ok
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::brk4() {
    _currentInstruction = &Chip::brk5;
    
    // Finish stack operation
    pushToStack1();
    stopStackOperation();
    
    // Calculate interrupt vectors index
    _interruptVectorsIndex = getCurrentInterruptVectorsIndex(); // TODO: peut etre plus besoin de ca car maintenant on a addressBusLow/High et on peut l'incrementer dans brk5
    
    // Read low byte of address
    readDataBus(_interruptVectors[_interruptVectorsIndex][0], _interruptVectors[_interruptVectorsIndex][1]);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::brk5() {
    _currentInstruction = &Chip::brk6;
    
    // Disable interrupts
    _flagsHelper.set<Flag::InterruptDisable>(true);
    
    // 6502 uses the ALU to store temporary low byte of address (by adding 0 to it to keep its value in adderHold)
    _alu.performSum<BDecimalSupported, false>(0x0, _inputDataLatch, false, false);
    
    // Read high byte of address
    readDataBus(_interruptVectors[_interruptVectorsIndex][0] + 1, _interruptVectors[_interruptVectorsIndex][1]);
    
    // Reset interrupts requested flag, since here it can redetected interrupts but no checkInterrupts because interrupt sequences themselves do not perform interrupt polling
    // See https://wiki.nesdev.com/w/index.php/CPU_interrupts
    _irqRequested = false;
    _nmiRequested = false;
    _resetRequested = false;
    _interruptRequested = false;
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::brk6() {
    // Set PC
    _programCounterLow = _alu.getAdderHold();
    _programCounterHigh = _inputDataLatch;
    
    // Fetch next opcode
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rti0() {
    _currentInstruction = &Chip::rti1;
    
    // Even if it's a implied addressing mode, PC is incremented when reading memory
    fetchData();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rti1() {
    _currentInstruction = &Chip::rti2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    pullFromStack0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rti2() {
    _currentInstruction = &Chip::rti3;
    
    // Pull status flag from stack
    pullFromStack1();
    pullFromStack0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rti3() {
    _currentInstruction = &Chip::rti4;
    
    // Set status flag
    _statusFlags = (_inputDataLatch & _Detail::FlagsHelper::getDisableMask<Flag::Break>()) | (1 << static_cast<int>(Flag::UnusedHigh));
    
    // Pull PCL from stack
    pullFromStack1();
    pullFromStack0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rti4() {
    _currentInstruction = &Chip::rti5;
    
    // Pull PCH from stack and finish stack operation, need to put before ALU operation because adderHold holds stackPointer
    pullFromStack1();
    stopStackOperation();
    
    // 6502 uses the ALU to store temporary low byte of address (by adding 0 to it to keep its value in adderHold)
    _alu.performSum<BDecimalSupported, false>(0x0, _inputDataLatch, false, false);
    
    // Check interrupts
    checkInterrupts();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::rti5() {
    // Set PC
    _programCounterLow = _alu.getAdderHold();
    _programCounterHigh = _inputDataLatch;
    
    fetchOpcode();
}

#endif /* Cpu6502_Internal_Interrupt_s_hpp */
