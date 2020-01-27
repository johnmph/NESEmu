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
        
        // TODO: exposer tous ce qu'il y a dans les pins
        uint16_t getAddressBus() const;
        uint8_t getDataBus() const;
        bool getReadWriteSignal() const;
        
    private:
        
        using Constants = Constants<EModel>;
        
        // Set Cpu as friend to keep data bus methods private
        friend Cpu6502::Chip<Chip, Chip, false>;
        
        // Memory
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        
        // DMA
        bool checkDma();
        
        // Internal
        TBus &_bus;
        Cpu6502::Chip<Chip, Chip, false> _cpu;
        int _dmaCount;
        uint8_t _dmaAddress;
        bool _dmaToggle;
    };
    
    #include "Cpu_s.hpp"
    
} }

#endif /* NESEmu_Cpu_hpp */
