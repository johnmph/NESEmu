//
//  DmcChannel.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_DmcChannel_hpp
#define NESEmu_Apu_DmcChannel_hpp

// See https://wiki.nesdev.com/w/index.php/APU_DMC

#include <cstdint>


namespace NESEmu { namespace Apu {
    
    template <class TChip>
    struct DmcChannel {
        
        DmcChannel(TChip &chip);
        
        void powerUp();
        
        // Clock
        void clock();
        
        // Properties
        bool isEnabled() const;
        void setEnabled(bool enabled);
        bool getInterrupt() const;
        void resetInterrupt();
        
        // Output
        uint8_t getOutput() const;
        
        // Register
        void setRegister(uint8_t registerNumber, uint8_t data);
        
        // Sample fetched callback
        void sampleFetched(uint8_t data);
        
    private:
        
        static uint16_t const _rates[16];//TODO: ca depend de NTSC/PAL !!!
        
        
        TChip &_chip;
        uint8_t _shiftRegister;
        uint8_t _shiftRegisterShiftedBitsCounter;
        bool _silenceFlag;
        uint16_t _counter;
        uint16_t _timer;
        uint8_t _outputLevel;
        uint16_t _sampleAddress;
        uint16_t _sampleLength;
        uint16_t _currentSampleAddress;
        uint16_t _sampleRemainingBytes;
        uint8_t _sampleBuffer;
        bool _sampleBufferFilled;
        bool _loopFlag;
        bool _interrupt;
        bool _enableInterrupt;
        
        bool _needRequestSample;
    };
    
    #include "DmcChannel_s.hpp"
    
} }

#endif /* NESEmu_Apu_DmcChannel_hpp */
