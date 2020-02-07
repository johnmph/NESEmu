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
    struct Chip {
        
        Chip(TBus &bus);
        
        void clock();
        
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
        
    private:
        
        using Constants = Constants<EModel>;
        using InternalCpu = Cpu6502::Chip<Cpu6502::ConfigurationPerformance<Chip, Chip, false>>;
        
        // Set Cpu as friend to keep data bus methods private
        friend InternalCpu;
        
        // Memory
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        
        // DMA
        bool checkDma();
        
        // Internal
        TBus &_bus;
        InternalCpu _cpu;
        int _dmaCount;
        uint8_t _dmaAddress;
        bool _dmaToggle;
    };
    
    #include "Cpu_s.hpp"
    
} }

#endif /* NESEmu_Cpu_hpp */
