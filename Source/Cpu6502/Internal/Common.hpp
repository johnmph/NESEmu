//
//  Common.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 6/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Common_hpp
#define Cpu6502_Internal_Common_hpp


template <_Detail::FlagsHelper::Flag ...EFlags>
void finishAlu();

template <_Detail::FlagsHelper::Flag ...EFlags>
void finishAluForAccumulator();

template <_Detail::FlagsHelper::Flag ...EFlags>
void finishAluForMemory(OpcodeInstruction nextInstruction);

template <_Detail::FlagsHelper::Flag ...EFlags>
void finishAluForMemory();

#endif /* Cpu6502_Internal_Common_hpp */
