//
//  FrameCounter.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_Internal_FrameCounter_hpp
#define NESEmu_Apu_Internal_FrameCounter_hpp

// See https://wiki.nesdev.com/w/index.php/APU_Frame_Counter

#include <cstdint>


namespace NESEmu { namespace Apu {
    
    template <class TChip>
    struct FrameCounter {
        // TODO: voir pour reset et power up
        FrameCounter(TChip &chip);
        
        void clock();
        
        void requestReset();
        
        // Properties
        bool getInterrupt() const;
        void resetInterrupt();
        
        void setDisableInterrupt(bool disableInterrupt);
        void setSequence5StepMode(bool sequence5StepMode);
        
    private:
        
        static constexpr uint16_t sequence4StepClocks[] = { 29830, 7457, 14913, 22371, 29828, 29829 };
        static constexpr uint16_t sequence5StepClocks[] = { 37282, 7457, 14913, 22371, 37281 };
        static constexpr uint8_t resetDelayCycle = 3;
        
        void clock4StepSequence();
        void clock5StepSequence();
        
        void clockQuarterFrame();
        void clockHalfFrame();
        
        
        TChip &_chip;
        uint16_t _counter;
        bool _interrupt;
        bool _disableInterrupt;
        bool _sequence5StepMode;
        uint8_t _resetDelay;
        bool _needToReset;
    };
    
    #include "FrameCounter_s.hpp"
    
} }

#endif /* NESEmu_Apu_Internal_FrameCounter_hpp */
