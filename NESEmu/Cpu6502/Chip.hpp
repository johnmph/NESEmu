//
//  Cpu6502.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 31/12/19.
//  Copyright © 2019 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Chip_hpp
#define Cpu6502_Chip_hpp

#include <cstdint>
#include <initializer_list>


namespace Cpu6502 {
    
    namespace _Detail {
        
        // We can't put that inside Cpu6502 because Cpu6502 is a templated class and we specialize some methods here (Cannot specialize a member of an unspecialized template)
        struct FlagsHelper {
            
            enum class Flag {
                Carry,
                Zero,
                InterruptDisable,
                DecimalMode,
                Break,
                UnusedHigh,
                Overflow,
                Negative
            };
            
            FlagsHelper(uint8_t &value, bool &carry, bool &overflow);
            
            template <Flag ...flags>
            static constexpr uint8_t getEnableMask();
            
            template <Flag ...flags>
            static constexpr uint8_t getDisableMask();
            
            template <Flag ...flags>
            void clear();
            
            template <Flag ...flags>
            bool get();
            
            template <Flag flag>
            void set(bool enable);
            
            template <Flag flag>
            void refreshImpl();
            
            template <Flag flag>
            void refreshImpl(uint8_t data);
            
            // Specialization for flags which depends on a data
            template <Flag firstFlag, Flag secondFlag, Flag ...otherFlags, typename std::enable_if<(firstFlag == FlagsHelper::Flag::Negative) || (firstFlag == FlagsHelper::Flag::Zero), int>::type = 0>
            void refreshImpl(uint8_t data);
            
            // Specialization for flags which doesn't depend on a data, I don't know if this really optimize
            template <Flag firstFlag, Flag secondFlag, Flag ...otherFlags, typename std::enable_if<(firstFlag != FlagsHelper::Flag::Negative) && (firstFlag != FlagsHelper::Flag::Zero), int>::type = 0>
            void refreshImpl(uint8_t data);
            
            template <Flag ...flags>
            void refresh(uint8_t data);
            
        private:
            uint8_t &_value;
            bool &_carry;
            bool &_overflow;
        };
        
    }
    
    template <class TBus>
    struct Chip {
        using Flag = _Detail::FlagsHelper::Flag;
        
        
        Chip(TBus &bus);
        
        void clock();
        
        void reset(bool high);
        void nmi(bool high);
        void irq(bool high);
        
        uint16_t getProgramCounter();   // TODO: pour debugging, mettre aussi d'autres getter (registers)
        uint16_t getAddressBus();
        uint8_t getDataBus();
        
        //private://TODO: remettre apres les tests
        using OpcodeInstruction = void (Chip::*)();
        
        enum class Interrupts {
            Nmi,
            Reset,
            IrqBrk
        };
        
        enum class ReadWrite : bool {
            Read = true,
            Write = false
        };
        
        // Memory
        void fetchMemory();
        void readDataBus(uint8_t low, uint8_t high);
        void writeDataBus(uint8_t low, uint8_t high, uint8_t data);
        
        // Program flow
        void incrementProgramCounter();
        void updateProgramCounter();
        void fetchData();
        void fetchOpcode(OpcodeInstruction nextInstruction);
        void fetchOpcode();
        void decodeOpcodeAndExecuteInstruction();
        
        // Interrupts
        void checkNmi();
        void checkIrq();
        bool checkInterrupts();
        int getCurrentInterruptVectorsIndex();
        
        // Stack
        void startStackOperation();
        void stopStackOperation();
        void pushToStack0(uint8_t data);
        void pushToStack1();
        void pullFromStack0();
        void pullFromStack1();
        
        // ALU
        void aluInvertBInput(); //TODO: taper ca dans une classe a part dans le namespace _Detail ? avec la possibilité de choisir celui sans Decimal et celui avec Decimal (un bool en template parameter ?)
        void aluPerformSum(bool decimalEnable, bool carryIn);
        void aluPerformAnd();
        void aluPerformOr();
        void aluPerformEor();
        void aluPerformShiftRight(bool carryIn);
        
        // Addressing modes
        #include "Internal/AddressingMode.hpp"
        
        // Instructions
        #include "Internal/Arithmetic.hpp"
        #include "Internal/Branch.hpp"
        #include "Internal/Flag.hpp"
        #include "Internal/Flow.hpp"
        #include "Internal/Interrupt.hpp"
        #include "Internal/Logic.hpp"
        #include "Internal/Memory.hpp"
        #include "Internal/Register.hpp"
        #include "Internal/Stack.hpp"
        #include "Internal/Undocumented.hpp"
        
        // Registers
        uint8_t _programCounterLow;
        uint8_t _programCounterHigh;
        uint8_t _stackPointer;
        uint8_t _accumulator;
        uint8_t _xIndex;
        uint8_t _yIndex;
        uint8_t _statusFlags;
        
        // Internal
        _Detail::FlagsHelper _flagsHelper;
        TBus &_bus;
        static const OpcodeInstruction _opcodeInstructionFuncs[256];
        static const uint8_t _interruptVectors[3][2];
        static const uint8_t _stackPageNumber;
        
        OpcodeInstruction _currentInstruction;
        int _interruptVectorsIndex;
        
        uint8_t _addressBusLow;
        uint8_t _addressBusHigh;
        uint8_t _inputDataLatch;
        uint8_t _dataOutput;
        bool _readWrite;
        
        bool _programCounterNeedsIncrement;
        
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
    
    #include "Internal/Chip_s.hpp"
    
    #include "Internal/AddressingMode_s.hpp"
    
    #include "Internal/Arithmetic_s.hpp"
    #include "Internal/Branch_s.hpp"
    #include "Internal/Flag_s.hpp"
    #include "Internal/Flow_s.hpp"
    #include "Internal/Interrupt_s.hpp"
    #include "Internal/Logic_s.hpp"
    #include "Internal/Memory_s.hpp"
    #include "Internal/Register_s.hpp"
    #include "Internal/Stack_s.hpp"
    #include "Internal/Undocumented_s.hpp"
    
}

#endif /* Cpu6502_Chip_hpp */
