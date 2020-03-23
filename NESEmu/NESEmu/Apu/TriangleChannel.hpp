//
//  TriangleChannel.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_Internal_TriangleChannel_hpp
#define NESEmu_Apu_Internal_TriangleChannel_hpp

// See https://wiki.nesdev.com/w/index.php/APU_Triangle

#include "LengthCounter.hpp"


namespace NESEmu { namespace Apu {
    
    struct TriangleChannel {
        
        void powerUp();
        void reset();
        
        void clock();
        
        void clockFrameCounterQuarterFrame();
        void clockFrameCounterHalfFrame();
        
        // Output
        uint8_t getOutput() const;
        
        // Register
        void setRegister(uint8_t registerNumber, uint8_t data);
        
        bool getLengthCounterOutput() const;
        void setLengthCounterEnabled(bool enabled);
        
    private:
        
        LengthCounter _lengthCounter;
        uint16_t _timer;
        uint16_t _counter;
        uint8_t _sequencerCurrentStep;
        uint8_t _linearCounter;
        uint8_t _linearCounterReloadValue;
        bool _linearCounterReloadFlag;
        bool _linearCounterControl;
    };
    
} }

#endif /* NESEmu_Apu_Internal_TriangleChannel_hpp */
