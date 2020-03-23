//
//  Chip.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 18/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_Chip_hpp
#define NESEmu_Apu_Chip_hpp

// See https://wiki.nesdev.com/w/index.php/APU
// See http://nesdev.com/apu_ref.txt

#include "FrameCounter.hpp"
#include "PulseChannel.hpp"
#include "TriangleChannel.hpp"
#include "NoiseChannel.hpp"
#include "DmcChannel.hpp"


namespace NESEmu { namespace Apu {
    
    template <class TInterruptHardware, class TSoundHardware>
    struct Chip {
        
        Chip(TInterruptHardware &interruptHardware, TSoundHardware &soundHardware);
        
        // Power up
        void powerUp();
        
        // Reset
        void reset(bool high);
        
        // Clock
        void clock();
        
        // Channels
        void setChannelRegister(uint16_t address, uint8_t data);
        
        // Status
        uint8_t getStatusRegister();
        void setStatusRegister(uint8_t data);
        
        // Frame counter
        void setFrameCounterRegister(uint8_t data);
        
    private:
        
        // Set FrameCounter as friend to keep clockFrameCounterxxx methods private
        friend FrameCounter<Chip>;
        
        
        // See https://wiki.nesdev.com/w/index.php/APU_Mixer
        float getMixedOutput() const;
        
        void clockFrameCounterQuarterFrame();
        void clockFrameCounterHalfFrame();
        
        void checkInterrupt();
        
        
        // Channels
        PulseChannel _pulseChannel[2];
        TriangleChannel _triangleChannel;
        NoiseChannel _noiseChannel;
        DmcChannel _dmcChannel;
        
        // Status
        bool _statusEnableDmc;//TODO: a voir
        
        // Frame counter
        FrameCounter<Chip> _frameCounter;
        
        // Internals
        TInterruptHardware &_interruptHardware;
        TSoundHardware &_soundHardware;
        bool _oddCycle;
        
    };
    
    #include "Chip_s.hpp"
    
} }

#endif /* NESEmu_Apu_Chip_hpp */
