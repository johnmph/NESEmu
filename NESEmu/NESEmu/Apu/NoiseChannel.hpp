//
//  NoiseChannel.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_NoiseChannel_s_hpp
#define NESEmu_Apu_NoiseChannel_s_hpp

// See https://wiki.nesdev.com/w/index.php/APU_Noise

#include "EnvelopeUnit.hpp"
#include "LengthCounter.hpp"


namespace NESEmu { namespace Apu {
    
    struct NoiseChannel {
        
        // Power up
        void powerUp();//TODO: retirer ca et reset si pas besoin
        
        // Clock
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
        
        static uint16_t const _timerPeriods[16];//TODO: ca depend de NTSC/PAL !!!
        
        EnvelopeUnit _envelopeUnit;
        LengthCounter _lengthCounter;
        uint16_t _counter;
        uint16_t _timer;
        uint16_t _shiftRegister;
        bool _mode;
    };
    
} }

#endif /* NESEmu_Apu_NoiseChannel_s_hpp */
