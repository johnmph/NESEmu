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
        
        struct Alu {
            
            uint8_t getAInput() const;
            uint8_t getBInput() const;
            uint8_t getAdderHold() const;
            bool getOverflow() const;
            bool getCarry() const;
            bool getHalfCarry() const;
            
            template <bool BDecimalSupported, bool BSubstractMode>
            void performSum(uint8_t aInput, uint8_t bInput, bool decimalEnable, bool carryIn);
            void performAnd(uint8_t aInput, uint8_t bInput);
            void performOr(uint8_t aInput, uint8_t bInput);
            void performEor(uint8_t aInput, uint8_t bInput);
            void performShiftRight(uint8_t aInput, bool carryIn);
            
        private:
            void invertBInput();
            
            template <bool BSubstractMode>
            void performSubstractMode();
            
            uint8_t _aInput;
            uint8_t _bInput;
            uint8_t _adderHold;
            bool _overflow;
            bool _carry;
            bool _halfCarry;
        };
        
        // We can't put that inside Chip because Chip is a templated class and we specialize some methods here (Cannot specialize a member of an unspecialized template)
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
            
            FlagsHelper(uint8_t &value, Alu &alu);
            
            template <Flag ...EFlags>
            static constexpr uint8_t getEnableMask();
            
            template <Flag ...EFlags>
            static constexpr uint8_t getDisableMask();
            
            template <Flag ...EFlags>
            void clear();
            
            template <Flag ...EFlags>
            bool get();
            
            template <Flag EFlag>
            void set(bool enable);
            
            template <Flag EFlag>
            void refreshImpl();
            
            template <Flag EFlag>
            void refreshImpl(uint8_t data);
            
            // Specialization for flags which depends on a data
            template <Flag EFirstFlag, Flag ESecondFlag, Flag ...EOtherFlags, typename std::enable_if<(EFirstFlag == FlagsHelper::Flag::Negative) || (EFirstFlag == FlagsHelper::Flag::Zero), int>::type = 0>
            void refreshImpl(uint8_t data);
            
            // Specialization for flags which doesn't depend on a data, I don't know if this really optimize
            template <Flag EFirstFlag, Flag ESecondFlag, Flag ...EOtherFlags, typename std::enable_if<(EFirstFlag != FlagsHelper::Flag::Negative) && (EFirstFlag != FlagsHelper::Flag::Zero), int>::type = 0>
            void refreshImpl(uint8_t data);
            
            template <Flag ...EFlags>
            void refresh(uint8_t data);
            
        private:
            uint8_t &_value;
            Alu &_alu;
        };
        
    }
    
    template <bool BSetOverflowEnabled, bool BResetAccurate, class TBus, bool BDecimalSupported = false>
    struct Configuration {
        using Bus = TBus;
        
        static constexpr bool DecimalSupported = BDecimalSupported;
        static constexpr bool SetOverflowEnabled = BSetOverflowEnabled;
        static constexpr bool ResetAccurate = BResetAccurate;
    };
    
    template <class TBus, bool BDecimalSupported = false>
    using ConfigurationAccurate = Configuration<true, true, TBus, BDecimalSupported>;
    
    template <class TBus, bool BDecimalSupported = false>
    using ConfigurationPerformance = Configuration<false, false, TBus, BDecimalSupported>;
    
    template <class TConfiguration>
    struct Chip {
        using Bus = typename TConfiguration::Bus;
        
        using Flag = _Detail::FlagsHelper::Flag;
        
        enum class ReadWrite : bool {
            Read = true,
            Write = false
        };
        
        Chip(Bus &bus);
        
        void powerUp(uint16_t programCounter = 0x00FF, uint8_t stackPointer = 0x0, uint8_t accumulator = 0xAA, uint8_t xIndex = 0x0, uint8_t yIndex = 0x0, uint8_t statusFlags = 0x22);
        
        void clock();
        void clockPhi1();
        void clockPhi2();
        void ready(bool high);
        
        void reset(bool high);
        void nmi(bool high);
        void irq(bool high);
        
        void setOverflow(bool high);
        
        uint16_t getAddressBus() const;
        uint8_t getDataBus() const;
        bool getReadWriteSignal() const;
        bool getSyncSignal() const;
        bool getM1Signal() const;
        bool getM2Signal() const;
        
    protected:
        
        static constexpr bool DecimalSupported = TConfiguration::DecimalSupported;
        static constexpr bool SetOverflowEnabled = TConfiguration::SetOverflowEnabled;
        static constexpr bool ResetAccurate = TConfiguration::ResetAccurate;
        
        using OpcodeInstruction = void (Chip::*)();
        
        enum class Interrupts {
            Nmi,
            Reset,
            IrqBrk
        };
        
        // Memory
        void fetchMemoryPhi1();
        void fetchMemoryPhi2();
        void readDataBus(uint8_t low, uint8_t high);
        void writeDataBus(uint8_t low, uint8_t high, uint8_t data);
        
        // Program flow
        void incrementProgramCounter();
        void updateProgramCounter();
        void fetchData();
        void fetchOpcode(OpcodeInstruction nextInstruction);
        void fetchOpcode();
        //void fetchOpcodeAfterRdyLow();
        void decodeOpcodeAndExecuteInstruction();
        
        // Overflow
        template <bool BSetOverflowEnabled, typename std::enable_if<BSetOverflowEnabled == true, int>::type = 0>
        void setOverflow(bool high);
        
        template <bool BSetOverflowEnabled, typename std::enable_if<BSetOverflowEnabled == false, int>::type = 0>
        void setOverflow(bool high);
        
        template <bool BSetOverflowEnabled, typename std::enable_if<BSetOverflowEnabled == true, int>::type = 0>
        void checkSetOverflow();
        
        template <bool BSetOverflowEnabled, typename std::enable_if<BSetOverflowEnabled == false, int>::type = 0>
        void checkSetOverflow();
        
        // Ready
        void checkReady();
        
        // Reset
        template <bool BResetAccurate, typename std::enable_if<BResetAccurate == true, int>::type = 0>
        void resetAfterPowerUp();
        
        template <bool BResetAccurate, typename std::enable_if<BResetAccurate == false, int>::type = 0>
        void resetAfterPowerUp();
        
        template <bool BResetAccurate, typename std::enable_if<BResetAccurate == true, int>::type = 0>
        void reset(bool high);
        
        template <bool BResetAccurate, typename std::enable_if<BResetAccurate == false, int>::type = 0>
        void reset(bool high);
        
        template <bool BResetAccurate, typename std::enable_if<BResetAccurate == true, int>::type = 0>
        void checkReset();
        
        template <bool BResetAccurate, typename std::enable_if<BResetAccurate == false, int>::type = 0>
        void checkReset();
        
        // Interrupts
        void checkNmi();
        void checkIrq();
        void checkInterrupts();
        int getCurrentInterruptVectorsIndex();
        
        template <bool BResetAccurate, typename std::enable_if<BResetAccurate == true, int>::type = 0>
        void correctInterruptVectorIndexForReset();
        
        template <bool BResetAccurate, typename std::enable_if<BResetAccurate == false, int>::type = 0>
        void correctInterruptVectorIndexForReset();
        
        // Stack
        void startStackOperation();
        void stopStackOperation();
        void pushToStack0(uint8_t data);
        void pushToStack1();
        void pullFromStack0();
        void pullFromStack1();
        
        // Addressing modes
        #include "Internal/AddressingMode.hpp"
        
        // Instructions
        #include "Internal/Common.hpp"
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
        Bus &_bus;
        static const OpcodeInstruction _opcodeInstructionFuncs[256];
        static const uint8_t _interruptVectors[3][2];
        static const uint8_t _stackPageNumber;
        
        OpcodeInstruction _currentInstruction;
        int _interruptVectorsIndex;
        
        uint16_t _addressBus;   // TODO: peut etre sortir addressBus et dataBus d'ici et le mettre dans le bus
        uint8_t _dataBus;
        uint8_t _addressBusLow;
        uint8_t _addressBusHigh;
        uint8_t _inputDataLatch;
        uint8_t _dataOutput;
        bool _readyLine;
        bool _readyWaitRequested;
        bool _sync;
        bool _readWrite;
        bool _phi2;
        
        bool _programCounterNeedsIncrement;
        
        uint8_t _predecode;
        uint8_t _instruction;
        
        bool _setOverflowLine;
        bool _setOverflowLinePrevious;
        bool _setOverflowRequested;
        
        _Detail::Alu _alu;
        
        bool _resetLine;
        bool _resetLineLatch;
        bool _resetRequested;
        OpcodeInstruction _resetSavedInstruction;
        bool _nmiLine;
        bool _nmiLinePrevious;
        bool _nmiRequested;
        bool _irqLine;
        bool _irqRequested;
        bool _interruptRequested;
    };
    
    #include "Internal/Chip_s.hpp"
    
    #include "Internal/AddressingMode_s.hpp"
    
    #include "Internal/Common_s.hpp"
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
