//
//  FrameCounter_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_Internal_FrameCounter_s_hpp
#define NESEmu_Apu_Internal_FrameCounter_s_hpp


template <class TChip>
FrameCounter<TChip>::FrameCounter(TChip &chip) : _chip(chip) {
}

template <class TChip>
void FrameCounter<TChip>::clock() {
    // Check if need to reset
    if (_needToReset) {
        // Decrement delay
        --_resetDelay;
        
        // If it's time to reset
        if (_resetDelay == 0) {
            // Reset frame counter
            _counter = 0;
            _needToReset = false;
            
            // Clock quarter and half frame if 5-step
            if (_sequence5StepMode) {
                clockQuarterFrame();
                clockHalfFrame();
            }
        }
    }
    
    // Check sequence step
    if (_sequence5StepMode) {
        clock5StepSequence();
    } else {
        clock4StepSequence();
    }
    
    // Increment counter
    ++_counter;
}

template <class TChip>
bool FrameCounter<TChip>::getInterrupt() const {
    return _interrupt;
}

template <class TChip>
void FrameCounter<TChip>::resetInterrupt() {
    _interrupt = false;
}

template <class TChip>
void FrameCounter<TChip>::setDisableInterrupt(bool disableInterrupt) {
    _disableInterrupt = disableInterrupt;
    
    // If interrupt disabled, reset possible interrupt flag
    if (_disableInterrupt) {
        _interrupt = false;
    }
}

template <class TChip>
void FrameCounter<TChip>::setSequence5StepMode(bool sequence5StepMode) {
    _sequence5StepMode = sequence5StepMode;
}

template <class TChip>
void FrameCounter<TChip>::requestReset() {
    _needToReset = true;
    _resetDelay = (_counter & 0x1) ? 4 : 3;
}

template <class TChip>
void FrameCounter<TChip>::clock4StepSequence() {
    // Check if last step reached
    if (_counter == 29830) {//TODO: mettre les valeurs dans des constantes
        // Set interrupt if not disabled
        _interrupt = !_disableInterrupt;
        
        // Reset counter
        _counter = 0;
    }
    // Step 1
    else if (_counter == 7457) {
        clockQuarterFrame();
    }
    // Step 2
    else if (_counter == 14913) {
        clockQuarterFrame();
        clockHalfFrame();
    }
    // Step 3
    else if (_counter == 22371) {
        clockQuarterFrame();
    }
    // Step 4 (first cycle for interrupt)
    else if (_counter == 29828) {
        // Set interrupt if not disabled
        _interrupt = !_disableInterrupt;
    }
    // Step 4
    else if (_counter == 29829) {
        // Set interrupt if not disabled
        _interrupt = !_disableInterrupt;
        
        clockQuarterFrame();
        clockHalfFrame();
    }
}

template <class TChip>
void FrameCounter<TChip>::clock5StepSequence() {
    // Check if last step reached
    if (_counter == 37282) {//TODO: mettre les valeurs dans des constantes
        // Reset counter
        _counter = 0;
    }
    // Step 1
    else if (_counter == 7457) {
        clockQuarterFrame();
    }
    // Step 2
    else if (_counter == 14913) {
        clockQuarterFrame();
        clockHalfFrame();
    }
    // Step 3
    else if (_counter == 22371) {
        clockQuarterFrame();
    }
    // Nothing happen on step 4
    // Step 5
    else if (_counter == 37281) {
        clockQuarterFrame();
        clockHalfFrame();
    }
}

template <class TChip>
void FrameCounter<TChip>::clockQuarterFrame() {
    _chip.clockFrameCounterQuarterFrame();
}

template <class TChip>
void FrameCounter<TChip>::clockHalfFrame() {
    _chip.clockFrameCounterHalfFrame();
}

#endif /* NESEmu_Apu_Internal_FrameCounter_s_hpp */
