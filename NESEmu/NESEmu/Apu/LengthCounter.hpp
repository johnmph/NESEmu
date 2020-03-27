//
//  LengthCounter.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_LengthCounter_hpp
#define NESEmu_Apu_LengthCounter_hpp

// See https://wiki.nesdev.com/w/index.php/APU_Length_Counter

#include <cstdint>


namespace NESEmu { namespace Apu {
    
    struct LengthCounter {
        
        void clock();
        
        // Update
        void update();
        
        // Output
        bool getOutput() const;
        
        // Properties
        void setEnabled(bool enabled);
        void setHalt(bool halt);
        void setValueIndex(uint8_t index);
        
    private:
        
        static uint8_t const _lengthValues[32];
        
        uint8_t _counter;
        bool _enabled;
        bool _halt;
        bool _requestHalt;
        uint8_t _reloadValue;
    };
    
} }

#endif /* NESEmu_Apu_LengthCounter_hpp */
