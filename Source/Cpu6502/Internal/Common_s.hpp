//
//  Common_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 6/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Common_s_hpp
#define Cpu6502_Internal_Common_s_hpp


template <class TConfiguration>
template <_Detail::FlagsHelper::Flag ...EFlags>
void Chip<TConfiguration>::finishAlu() {
    // Update status
    _flagsHelper.refresh<EFlags...>(_alu.getAdderHold());
    
    // Execute instruction
    decodeOpcodeAndExecuteInstruction();
}

template <class TConfiguration>
template <_Detail::FlagsHelper::Flag ...EFlags>
void Chip<TConfiguration>::finishAluForAccumulator() {
    // Store ALU result in accumulator
    _accumulator = _alu.getAdderHold();
    
    // Finish ALU
    finishAlu<EFlags...>();
}

template <class TConfiguration>
template <_Detail::FlagsHelper::Flag ...EFlags>
void Chip<TConfiguration>::finishAluForMemory(OpcodeInstruction nextInstruction) {
    // Set next instruction
    _currentInstruction = nextInstruction;
    
    // Write result back
    anyRMWWrite(_alu.getAdderHold());
    
    // Update status
    _flagsHelper.refresh<EFlags...>(_alu.getAdderHold());
}

template <class TConfiguration>
template <_Detail::FlagsHelper::Flag ...EFlags>
void Chip<TConfiguration>::finishAluForMemory() {
    finishAluForMemory<EFlags...>(&Chip::fetchOpcode);
}

#endif /* Cpu6502_Internal_Common_s_hpp */
