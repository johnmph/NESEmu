//
//  SweepUnit.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_SweepUnit_hpp
#define NESEmu_Apu_SweepUnit_hpp

// See https://wiki.nesdev.com/w/index.php/APU_Sweep

#include <cstdint>


namespace NESEmu { namespace Apu {
    
    //template <bool BNegateOneComplement>
    struct SweepUnit {
        
        SweepUnit(bool oneComplementMode, uint16_t &channelPeriod);
        
        void clock();
        
        void reload();
        
        // Output
        bool getOutput() const;
        uint16_t getTargetPeriod() const;
        
        // Properties
        void setPeriod(uint8_t period);
        void setShiftCount(uint8_t shiftCount);
        void setEnabled(bool enabled);
        void setNegate(bool negate);
        
    private:
        
        static constexpr uint16_t targetPeriodOverflow = 0x800;
        static constexpr uint8_t channelMinimumPeriod = 8;
        
        uint16_t &_channelPeriod;
        uint8_t _counter;
        uint8_t _period;
        uint8_t _shiftCount;
        bool const _oneComplementMode;
        bool _enabled;
        bool _reloadFlag;
        bool _negate;
    };
    
} }

#endif /* NESEmu_Apu_SweepUnit_hpp */
