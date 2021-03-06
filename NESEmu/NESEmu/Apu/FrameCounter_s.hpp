//
//  FrameCounter_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_FrameCounter_s_hpp
#define NESEmu_Apu_FrameCounter_s_hpp


template <class TChip>
FrameCounter<TChip>::FrameCounter(TChip &chip) : _chip(chip) {
}

template <class TChip>
void FrameCounter<TChip>::powerUp() {
    _counter = 0;
    _interrupt = false;
    _disableInterrupt = false;
    _sequence5StepMode = false;
    _resetDelay = 0;
    _needToReset = false;
}

template <class TChip>
void FrameCounter<TChip>::clock() {
    // Increment counter
    ++_counter;
    
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
    /*
    // If sequencer is set in 5 step mode, reset possible interrupt flag
    if (_sequence5StepMode) {
        _interrupt = false;
     }*/ // TODO: ainsi ou pas ????
}

template <class TChip>
void FrameCounter<TChip>::requestReset() {
    _needToReset = true;
    
    // If the write occurs during an APU cycle, the effects occur 3 CPU cycles after the $4017 write cycle, and if the write occurs between APU cycles, the effects occurs 4 CPU cycles after the write cycle
    // APU cycle has even counter but this is called before the counter is incremented so here a APU cycle has odd counter
    _resetDelay = resetDelayCycle + ((_counter & 0x1) == 0);
}

template <class TChip>
void FrameCounter<TChip>::clock4StepSequence() {
    // Check if last step reached
    if (_counter == sequence4StepClocks[0]) {
        // Set interrupt if not disabled
        _interrupt = !_disableInterrupt;
        
        // Reset counter
        _counter = 0;
    }
    // Step 1
    else if (_counter == sequence4StepClocks[1]) {
        clockQuarterFrame();
    }
    // Step 2
    else if (_counter == sequence4StepClocks[2]) {
        clockQuarterFrame();
        clockHalfFrame();
    }
    // Step 3
    else if (_counter == sequence4StepClocks[3]) {
        clockQuarterFrame();
    }
    // Step 4 (first cycle for interrupt)
    else if (_counter == sequence4StepClocks[4]) {
        // Set interrupt if not disabled
        _interrupt = !_disableInterrupt;
    }
    // Step 4
    else if (_counter == sequence4StepClocks[5]) {
        // Set interrupt if not disabled
        _interrupt = !_disableInterrupt;
        
        clockQuarterFrame();
        clockHalfFrame();
    }
}

template <class TChip>
void FrameCounter<TChip>::clock5StepSequence() {
    // Check if last step reached
    if (_counter == sequence5StepClocks[0]) {
        // Reset counter
        _counter = 0;
    }
    // Step 1
    else if (_counter == sequence5StepClocks[1]) {
        clockQuarterFrame();
    }
    // Step 2
    else if (_counter == sequence5StepClocks[2]) {
        clockQuarterFrame();
        clockHalfFrame();
    }
    // Step 3
    else if (_counter == sequence5StepClocks[3]) {
        clockQuarterFrame();
    }
    // Nothing happen on step 4
    // Step 5
    else if (_counter == sequence5StepClocks[4]) {
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

#endif /* NESEmu_Apu_FrameCounter_s_hpp */
