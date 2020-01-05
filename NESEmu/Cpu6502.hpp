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
    
private:
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
        Unused,
        Overflow,
        Negative
    };
    
    static constexpr uint8_t getStatusFlagsEnableMask(std::initializer_list<Flags> const &flags);
    static constexpr uint8_t getStatusFlagsDisableMask(std::initializer_list<Flags> const &flags);
    constexpr void clearStatusFlags(std::initializer_list<Flags> const &flags);
    constexpr bool getStatusFlag(Flags flag);
    constexpr void setStatusFlag(Flags flag, bool value);
    
    void incrementProgramCounter();
    
    void readDataBus(uint8_t low, uint8_t high);
    void writeDataBus(uint8_t low, uint8_t high);
    
    void fetchData();
    void fetchOpcode();
    void decodeOpcode();
    
    void checkNmi();
    bool checkInterrupts();
    
    void finishInstruction();
    
    void aluInvertBInput();
    void aluPerformSum(bool decimalEnable, bool carryIn);
    void aluPerformAnd();
    void aluPerformOr();
    void aluPerformEor();
    void aluPerformShiftRight(bool carryIn);
    
    void implied();
    void immediate();
    void absolute0();
    void absolute1();
    void absoluteLoad();
    void absoluteStore();
    void zeroPage0();
    void zeroPageLoad();
    void zeroPageStore();
    void relative();
    void relativeBranch0();
    void relativeBranch1();
    void absoluteIndexed0();
    void absoluteIndexed1(uint8_t index);
    void absoluteIndexedX1();
    void absoluteIndexedY1();
    void absoluteIndexedLoad0();
    void absoluteIndexedLoad1();
    void absoluteIndexedStore0();
    void absoluteIndexedStore1();
    
    void zeroPageIndexed0();
    void zeroPageIndexed1(uint8_t index);
    void zeroPageIndexedX1();
    void zeroPageIndexedY1();
    void zeroPageIndexedLoad();
    void zeroPageIndexedStore();
    
    void zeroPagePreIndexedIndirect0();
    void zeroPagePreIndexedIndirect1();
    void zeroPagePreIndexedIndirect2();
    void zeroPagePreIndexedIndirect3();
    void zeroPagePreIndexedIndirectLoad();
    void zeroPagePreIndexedIndirectStore();
    
    void zeroPageIndirectPostIndexed0();
    void zeroPageIndirectPostIndexed1();
    void zeroPageIndirectPostIndexed2();
    void zeroPageIndirectPostIndexedLoad0();
    void zeroPageIndirectPostIndexedLoad1();
    void zeroPageIndirectPostIndexedStore0();
    void zeroPageIndirectPostIndexedStore1();
    
    void pushToStack(uint8_t data);
    void pullFromStack();
    
    void clv();
    void lda();
    void staAbsolute();
    void staZeroPage();
    void staAbsoluteIndexed();
    void staZeroPageIndexed();
    void sta();
    void bcs();
    void branch(bool condition);
    void brk0();
    void brk1();
    void brk2();
    void brk3();
    void brk4();
    void brk5();
    void brk6();
    void startLow();
    void startHigh0and1();
    void startHigh2();
    void startHigh3();
    void startHigh4();
    void startHigh5();
    void startHigh6();
    void startHigh7();
    
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
    static const InstructionPipeline _instrPipelineFuncs[256];  // TODO: changer 256 par le vrai nombre
    static const int _instrPipelineStartIndexes[256];
    static const uint8_t _interruptVectors[3][2];
    static const uint8_t _stackPageNumber;
    int _instrPipelineStartIndex;
    int _pipelineStep;
    
    uint8_t _predecode;
    uint8_t _instruction;
    uint8_t _addressBusLow;
    uint8_t _addressBusHigh;
    uint8_t _inputDataLatch;
    uint8_t _dataOutput;
    uint8_t _aInput;
    uint8_t _bInput;
    uint8_t _adderHold;
    bool _aluOverflow;
    bool _aluCarry;
    bool _aluHalfCarry;
    
    bool _resetLine;
    bool _nmiLine;
    bool _nmiLinePrevious;
    bool _nmiRequested;
    bool _irqLine;
    bool _irqRequested;
};

#include "Cpu6502_s.hpp"

#endif /* Cpu6502_hpp */
