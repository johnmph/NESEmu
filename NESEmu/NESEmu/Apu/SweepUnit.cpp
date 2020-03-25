//
//  SweepUnit.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "SweepUnit.hpp"


namespace NESEmu { namespace Apu {
    
    SweepUnit::SweepUnit(bool oneComplementMode, uint16_t &channelPeriod) : _oneComplementMode(oneComplementMode), _channelPeriod(channelPeriod) {
    }
    
    void SweepUnit::clock() {
        // Update channel period if allowed
        if ((_counter == 0) && _enabled && getOutput()) {
            _channelPeriod = getTargetPeriod();
        }
        
        // If need to reload counter
        if ((_counter == 0) || (_reloadFlag)) {
            // Reload counter and reset reload flag
            _counter = _period;
            _reloadFlag = false;
            
            return;
        }
        
        // Decrement counter
        --_counter;
    }
    
    uint16_t SweepUnit::getTargetPeriod() const {
        // Shift right
        uint16_t amount = _channelPeriod >> _shiftCount;
        
        // Negate if necessary
        if (_negate) {
            amount = (_oneComplementMode) ? ~amount : -amount;
        }
        
        // Add to channel period
        return _channelPeriod + amount;
    }
    
    bool SweepUnit::getOutput() const {
        // Output only if not overflow (11 bits timer) and channel period is not less than the minimum period
        return (getTargetPeriod() < targetPeriodOverflow) && (_channelPeriod >= channelMinimumPeriod);
    }
    
    void SweepUnit::setPeriod(uint8_t period) {
        _period = period;
    }
    
    void SweepUnit::setShiftCount(uint8_t shiftCount) {
        _shiftCount = shiftCount;
    }
    
    void SweepUnit::setEnabled(bool enabled) {
        _enabled = enabled;
    }
    
    void SweepUnit::setNegate(bool negate) {
        _negate = negate;
    }
    
    void SweepUnit::reload() {
        _reloadFlag = true;
    }
    
} }
