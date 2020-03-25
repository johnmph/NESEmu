//
//  EnvelopeUnit.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_EnvelopeUnit_hpp
#define NESEmu_Apu_EnvelopeUnit_hpp

// See https://wiki.nesdev.com/w/index.php/APU_Envelope

#include <cstdint>


namespace NESEmu { namespace Apu {
    
    struct EnvelopeUnit {
        
        void clock();
        
        void start();
        
        // Output
        uint8_t getVolume() const;
        
        // Parameters
        void setDividerPeriodOrConstantVolume(uint8_t value);
        void setLoopMode(bool loopMode);
        void setConstantVolumeMode(bool constantVolumeMode);
        
    private:
        
        static constexpr uint8_t decayCounterReloadValue = 15;
        
        uint8_t _dividerPeriodOrConstantVolume;
        uint8_t _counter;
        uint8_t _decayCounter;
        bool _startFlag;
        bool _loopMode;
        bool _constantVolumeMode;
    };
    
} }

#endif /* NESEmu_Apu_EnvelopeUnit_hpp */
