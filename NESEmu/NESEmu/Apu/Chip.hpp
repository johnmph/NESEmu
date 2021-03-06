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
    
    template <class TCpu, class TSoundManager>
    struct Chip {
        
        Chip(TCpu &cpu, TSoundManager &soundManager);
        
        // Power up
        void powerUp();
        
        // Clock
        void clock();
        
        // Reset
        void reset(bool high);
        
        // Channels
        void setChannelRegister(uint16_t address, uint8_t data);
        
        // Status
        uint8_t getStatusRegister();
        void setStatusRegister(uint8_t data);
        
        // Frame counter
        void setFrameCounterRegister(uint8_t data);
        
        // DMC Sample fetched callback
        void dmcSampleFetched(uint8_t data);
        
        // Channels output
        float getPulsesOutput() const;
        float getTriangleNoiseDmcOutput() const;
        
    private:
        
        // Set FrameCounter as friend to keep clockFrameCounterxxx methods private
        friend FrameCounter<Chip>;
        
        // Set DmcChannel as friend to keep requestDmcSample method private
        friend DmcChannel<Chip>;
        
        
        // See https://wiki.nesdev.com/w/index.php/APU_Mixer
        float getMixedOutput() const;
        
        void clockFrameCounterQuarterFrame();
        void clockFrameCounterHalfFrame();
        
        void requestDmcSample(uint16_t address);
        
        void checkInterrupt();
        
        void checkReset();
        
        
        // Channels
        PulseChannel _pulseChannel[2];
        TriangleChannel _triangleChannel;
        NoiseChannel _noiseChannel;
        DmcChannel<Chip> _dmcChannel;
        
        // Frame counter
        FrameCounter<Chip> _frameCounter;
        
        // Internals
        TCpu &_cpu;
        TSoundManager &_soundManager;
        
        bool _resetLine;
    };
    
    #include "Chip_s.hpp"
    
} }

#endif /* NESEmu_Apu_Chip_hpp */
