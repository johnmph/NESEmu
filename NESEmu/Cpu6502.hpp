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
    
    void fetchOpcode();
    
    void immediate_0();
    void absolute_0();
    void absolute_1();
    void absolute_load();
    void absolute_store();
    
    uint8_t zeroPage();
    uint16_t relative();
    uint16_t absoluteIndexed(uint8_t index);
    uint16_t absoluteIndexedX();
    uint16_t absoluteIndexedY();
    uint8_t zeroPageIndexed(uint8_t index);
    uint8_t zeroPageIndexedX();
    uint8_t zeroPageIndexedY();
    uint16_t zeroPagePreIndexedIndirect();
    uint16_t zeroPageIndirectPostIndexed();
    
    // Internal
    static const InstructionPipeline _instrPipelineFuncs[256];  // TODO: changer 256 par le vrai nombre
    static const uint8_t _instrPipelineStartIndexes[256];
    int _instrPipelineStartIndex;
    uint8_t _pipelineStep;
    uint8_t _opcode;
    uint8_t _adl;
    uint8_t _adh;
    uint8_t _bal;
    uint8_t _bah;
    uint8_t _data;
    uint8_t _alu;   // TODO: alu register ?
    
    // Registers
    uint16_t _programCounter;   // TODO: séparer en 2 pour les relative ?
    uint8_t _stackPointer;
    uint8_t _accumulator;
    uint8_t _xIndex;
    uint8_t _yIndex;
    uint8_t _statusFlags;
    
    bool _nmiLine;
    bool _irqLine;
    bool _resetLine;
};

#endif /* Cpu6502_hpp */
