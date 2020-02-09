//
//  Cpu.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 23/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cpu_hpp
#define NESEmu_Cpu_hpp

#include <cstdint>
#include "Cpu6502/Chip.hpp"

// TODO: rajouter des tests Visual2A03 (surtout pour le DMA)
namespace NESEmu { namespace Cpu {
    
    enum class Model {
        Ricoh2A03,
        Ricoh2A07
    };
    
    template <Model EModel>
    struct Constants;
    
    template <Model EModel, class TBus>
    struct Chip : protected Cpu6502::Chip<Cpu6502::ConfigurationPerformance<Chip<EModel, TBus>, false>> {
        
        Chip(TBus &bus);
        
        void powerUp(uint16_t programCounter = 0x100F, uint8_t stackPointer = 0xC0, uint8_t accumulator = 0x00, uint8_t xIndex = 0xC0, uint8_t yIndex = 0x0, uint8_t statusFlags = 0x22);
        
        void clock();
        void clockPhi1();
        void clockPhi2();
        
        void reset(bool high);
        void nmi(bool high);
        void irq(bool high);
        //void test(bool high);
        
        // TODO: exposer tous ce qu'il y a dans les pins
        uint16_t getAddressBus() const;
        uint8_t getDataBus() const;
        //bool getAd1Signal() const;
        //bool getAd2Signal() const;
        bool getReadWriteSignal() const;
        //uint8_t getOutSignal() const;
        //bool getOe1Signal() const;
        //bool getOe2Signal() const;
        bool getM2Signal() const;
        
    private:
        
        using Constants = Constants<EModel>;
        using InternalCpu = Cpu6502::Chip<Cpu6502::ConfigurationPerformance<Chip, false>>;
        
        // Set Cpu as friend to keep data bus methods private
        friend InternalCpu;
        
        // Memory
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        
        // DMA
        bool checkDmaPhi1();
        bool checkDmaPhi2();
        
        // Internal
        TBus &_bus;
        bool _dmaStarted;
        int _dmaCount;
        uint8_t _dmaAddress;
        bool _dmaToggle;
    };
    
    #include "Cpu_s.hpp"
    
} }

#endif /* NESEmu_Cpu_hpp */
