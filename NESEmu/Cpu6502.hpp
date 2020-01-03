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
    void nmi(bool high);
    void irq(bool high);
    void reset(bool high);
    
private:
    using InstructionPipeline = void (Cpu6502::*)();
    
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
    
    //uint16_t getProgramCounter();
    void incrementProgramCounter();
    
    void readDataBus(uint8_t low, uint8_t high);
    void writeDataBus(uint8_t low, uint8_t high);
    
    void fetchData();
    void fetchOpcode();
    void decodeOpcode();
    void finishInstruction();
    
    void aluPerformSum(bool decimalEnable, bool carryIn);
    
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
    
    uint8_t zeroPageIndexed(uint8_t index);
    uint8_t zeroPageIndexedX();
    uint8_t zeroPageIndexedY();
    uint16_t zeroPagePreIndexedIndirect();
    uint16_t zeroPageIndirectPostIndexed();
    
    void clv();
    void lda();
    void staAbsolute();
    void staZeroPage();
    void staAbsoluteIndexed();
    void sta();
    void bcs();
    void branch(bool condition);
    
    // Internal
    TBus &_bus;
    static const InstructionPipeline _instrPipelineFuncs[256];  // TODO: changer 256 par le vrai nombre
    static const uint8_t _instrPipelineStartIndexes[256];
    int _instrPipelineStartIndex;
    uint8_t _pipelineStep;
    
    uint8_t _inputDataLatch;
    uint8_t _predecode;
    uint8_t _instruction;
    uint8_t _dataOutput;
    uint8_t _aInput;
    uint8_t _bInput;
    uint8_t _adderHold;
    bool _aluOverflow;
    bool _aluCarry;
    bool _aluHalfCarry;
    uint8_t _addressBusLow;
    uint8_t _addressBusHigh;
    //uint8_t _bal;
    //uint8_t _bah;
    
    // Registers
    uint8_t _programCounterLow;
    uint8_t _programCounterHigh;
    uint8_t _stackPointer;
    uint8_t _accumulator;
    uint8_t _xIndex;
    uint8_t _yIndex;
    uint8_t _statusFlags;
    
    bool _nmiLine;
    bool _irqLine;
    bool _resetLine;
};

#include "Cpu6502_s.hpp"

#endif /* Cpu6502_hpp */
