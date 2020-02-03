//
//  Stack_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Stack_s_hpp
#define Cpu6502_Internal_Stack_s_hpp


template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::push2() {
    pushToStack1();
    stopStackOperation();
    
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::pha0() {
    _currentInstruction = &Chip::pha1;
    
    // In implied addressing mode, there is a unused read which doesn't increment PC
    readDataBus(_programCounterLow, _programCounterHigh);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::pha1() {
    _currentInstruction = &Chip::push2;
    
    startStackOperation();
    pushToStack0(_accumulator);
    
    // Check interrupts
    checkInterrupts();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::php0() {
    _currentInstruction = &Chip::php1;
    
    // In implied addressing mode, there is a unused read which doesn't increment PC
    readDataBus(_programCounterLow, _programCounterHigh);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::php1() {
    _currentInstruction = &Chip::push2;
    
    startStackOperation();
    pushToStack0(_statusFlags | (1 << static_cast<int>(Flag::Break)));
    
    // Check interrupts
    checkInterrupts();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::pla0() {
    _currentInstruction = &Chip::pla1;
    
    // In implied addressing mode, there is a unused read which doesn't increment PC
    readDataBus(_programCounterLow, _programCounterHigh);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::pla1() {
    _currentInstruction = &Chip::pla2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    pullFromStack0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::pla2() {
    _currentInstruction = &Chip::pla3;
    
    pullFromStack1();
    stopStackOperation();
    
    // Check interrupts
    checkInterrupts();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::pla3() {
    _accumulator = _inputDataLatch;
    
    // Update status
    _flagsHelper.refresh<Flag::Zero, Flag::Negative>(_accumulator);
    
    // Fetch opcode
    fetchOpcode();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::plp0() {
    _currentInstruction = &Chip::plp1;
    
    // In implied addressing mode, there is a unused read which doesn't increment PC
    readDataBus(_programCounterLow, _programCounterHigh);
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::plp1() {
    _currentInstruction = &Chip::plp2;
    
    // Start stack operation (read of current cycle will read stack memory)
    startStackOperation();
    pullFromStack0();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::plp2() {
    _currentInstruction = &Chip::plp3;
    
    pullFromStack1();
    stopStackOperation();
    
    // Check interrupts
    checkInterrupts();
}

template <class TBus, class TInternalHardware, bool BDecimalSupported>
void Chip<TBus, TInternalHardware, BDecimalSupported>::plp3() {
    _statusFlags = (_inputDataLatch & _Detail::FlagsHelper::getDisableMask<Flag::Break>()) | (1 << static_cast<int>(Flag::UnusedHigh));
    
    fetchOpcode();
}

#endif /* Cpu6502_Internal_Stack_s_hpp */
