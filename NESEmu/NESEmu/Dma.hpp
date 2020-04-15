//
//  Dma.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 15/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Dma_hpp
#define NESEmu_Dma_hpp

#include <cstdint>


namespace NESEmu { namespace Cpu {
    
    template <class TChip>
    struct Dma {
        
        Dma(TChip &chip);
        
        // Power up
        void powerUp();
        
        // Clock
        void clock();
        
        // Reset
        void reset(bool high);
        
        // Start
        void startSprite(uint8_t address);
        void startDmc(uint16_t address);
        
        // Properties
        bool isIdle() const;
        
    private:
        
        void process();
        bool processDmc();
        void processSprite();
        
        TChip &_chip;
        uint16_t _spriteAddress;
        uint16_t _dmcAddress;
        uint16_t _spriteCycleCount;
        uint8_t _dmcCycleCount;
        uint8_t _spriteWaitCycleCount;
        uint8_t _dmcWaitCycleCount;
        bool _writeCycle;
    };
    
    #include "Dma_s.hpp"
    
} }

#endif /* NESEmu_Dma_hpp */
