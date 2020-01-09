//
//  Cpu6502.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 31/12/19.
//  Copyright © 2019 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_hpp
#define Cpu6502_hpp

#include <cstdint>
#include <initializer_list>


template <class TBus>
struct Cpu6502 {
    Cpu6502(TBus &bus);
    
    void clock();
    
    void reset(bool high);
    void nmi(bool high);
    void irq(bool high);
    
    uint16_t getProgramCounter();   // TODO: pour debugging, mettre aussi d'autres getter (registers)
    uint16_t getAddressBus();
    uint8_t getDataBus();
    
    //private://TODO: remettre apres les tests
    using InstructionPipeline = void (Cpu6502::*)();
    
    enum class Interrupts {
        Nmi,
        Reset,
        IrqBrk
    };
    
    enum class Flags {
        Carry,
        Zero,
        InterruptDisable,
        DecimalMode,
        Break,
        UnusedHigh,
        Overflow,
        Negative
    };
    
    enum class ReadWrite : bool {
        Read = true,
        Write = false
    };
    
    // Status
    static constexpr uint8_t getStatusFlagsEnableMask(std::initializer_list<Flags> const &flags);
    static constexpr uint8_t getStatusFlagsDisableMask(std::initializer_list<Flags> const &flags);
    constexpr void clearStatusFlags(std::initializer_list<Flags> const &flags);
    constexpr bool getStatusFlag(Flags flag);
    constexpr void setStatusFlag(Flags flag, bool value);
    
    // Memory
    void fetchMemory();
    void readDataBus(uint8_t low, uint8_t high);
    void writeDataBus(uint8_t low, uint8_t high, uint8_t data);
    
    // Program flow
    void incrementProgramCounter();
    void fetchData();
    void fetchOpcode(InstructionPipeline nextInstruction);
    void fetchOpcode();
    void decodeOpcode();
    
    // Interrupts
    void checkNmi();
    void checkIrq();
    bool checkInterrupts();
    int getCurrentInterruptVectorsIndex();
    
    // Stack
    void pushToStack0(uint8_t data);
    void pushToStack1();
    void pullFromStack0();
    void pullFromStack1();
    
    // ALU
    void aluInvertBInput();
    void aluPerformSum(bool decimalEnable, bool carryIn);
    void aluPerformAnd();
    void aluPerformOr();
    void aluPerformEor();
    void aluPerformShiftRight(bool carryIn);
    
    // Addressing modes
    #include "Cpu6502AddressingMode.hpp"
    
    // Instructions
    #include "Cpu6502Arithmetic.hpp"
    #include "Cpu6502Branch.hpp"
    #include "Cpu6502Flag.hpp"
    #include "Cpu6502Flow.hpp"
    #include "Cpu6502Interrupt.hpp"
    #include "Cpu6502Logic.hpp"
    #include "Cpu6502Memory.hpp"
    #include "Cpu6502Register.hpp"
    #include "Cpu6502Stack.hpp"
    
    // Registers
    uint8_t _programCounterLow;
    uint8_t _programCounterHigh;
    uint8_t _stackPointer;
    uint8_t _accumulator;
    uint8_t _xIndex;
    uint8_t _yIndex;
    uint8_t _statusFlags;
    
    // Internal
    TBus &_bus;
    static const InstructionPipeline _instrPipelineFuncs[256];
    static const uint8_t _interruptVectors[3][2];
    static const uint8_t _stackPageNumber;
    
    InstructionPipeline _currentInstruction;
    int _interruptVectorsIndex;
    
    uint8_t _addressBusLow;
    uint8_t _addressBusHigh;
    uint8_t _inputDataLatch;
    uint8_t _dataOutput;
    bool _readWrite;
    
    uint8_t _addressBusLowRegister;
    uint8_t _addressBusHighRegister;
    uint8_t _predecode;
    uint8_t _instruction;
    
    uint8_t _aInput;
    uint8_t _bInput;
    uint8_t _adderHold;
    bool _aluOverflow;
    bool _aluCarry;
    bool _aluHalfCarry;
    
    bool _resetLine;
    bool _resetRequested;
    bool _nmiLine;
    bool _nmiLinePrevious;
    bool _nmiRequested;
    bool _irqLine;
    bool _irqRequested;
    bool _interruptRequested;
};

#include "Cpu6502_s.hpp"

#include "Cpu6502AddressingMode_s.hpp"

#include "Cpu6502Arithmetic_s.hpp"
#include "Cpu6502Branch_s.hpp"
#include "Cpu6502Flag_s.hpp"
#include "Cpu6502Flow_s.hpp"
#include "Cpu6502Interrupt_s.hpp"
#include "Cpu6502Logic_s.hpp"
#include "Cpu6502Memory_s.hpp"
#include "Cpu6502Register_s.hpp"
#include "Cpu6502Stack_s.hpp"


#endif /* Cpu6502_hpp */
