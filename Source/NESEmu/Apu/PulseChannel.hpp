//
//  PulseChannel.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_PulseChannel_hpp
#define NESEmu_Apu_PulseChannel_hpp

// See https://wiki.nesdev.com/w/index.php/APU_Pulse

#include "EnvelopeUnit.hpp"
#include "SweepUnit.hpp"
#include "LengthCounter.hpp"


namespace NESEmu { namespace Apu {
    
    struct PulseChannel {
        
        explicit PulseChannel(bool sweepOneComplementMode);
        
        // Power up
        void powerUp();
        
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
        
        static constexpr uint8_t sequencerStepsMask = 0x7;
        static uint8_t const _dutyWaveforms[4];
        
        EnvelopeUnit _envelopeUnit;
        SweepUnit _sweepUnit;
        LengthCounter _lengthCounter;
        uint16_t _timer;
        uint16_t _counter;
        uint8_t _sequencerCurrentStep;  // TODO: voir par apres pour l'implementer avec un shift register plutot (a chaque sequence on shift a droite et on recupere le bit sorti comme etant la sequence courante)
        uint8_t _waveform;
    };
    
} }

#endif /* NESEmu_Apu_PulseChannel_hpp */
