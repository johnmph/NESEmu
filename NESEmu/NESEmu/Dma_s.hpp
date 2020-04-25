//
//  Dma_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 15/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Dma_s_hpp
#define NESEmu_Dma_s_hpp


template <class TChip>
Dma<TChip>::Dma(TChip &chip) : _chip(chip) {
}

template <class TChip>
void Dma<TChip>::powerUp() {
    _spriteAddress = 0x0;
    _dmcAddress = 0x0;
    _writeCycle = false;
    
    // Reset other members via reset method
    reset(false);
}

template <class TChip>
void Dma<TChip>::clock() {
    // Toggle read / write cycle before process so we have the same type cycle in process method and CPU instruction execution (which is right after this clock method)
    _writeCycle = !_writeCycle;
    
    // Copy read write signal
    // Used _chip.TChip::InternalCpu:: instead _chip.InternalCpu:: because GCC doesn't compile it
    // See https://stackoverflow.com/questions/61419735/different-compiler-behaviour-when-using-alias-as-scope-to-get-parent-member
    _chip._readWrite = _chip.TChip::InternalCpu::_readWrite;//TODO: a voir
    
    // Process
    process();
}

template <class TChip>
void Dma<TChip>::reset(bool high) {
    // Only reset if line is low
    if (high) {
        return;
    }
    
    // Reset possible DMA operation
    _spriteCycleCount = 0;
    _spriteWaitCycleCount = 0;
    _dmcCycleCount = 0;
    _dmcWaitCycleCount = 0;
    
    // Reenable possible disabled CPU
    _chip.ready(true);  // TODO: voir si ok
}

template <class TChip>
void Dma<TChip>::startSprite(uint8_t address) {
    // Calculate address
    _spriteAddress = address << 8;
    
    // Set counters (Sprite cycle count has a extra cycle to restore the CPU at the end)
    _spriteWaitCycleCount = 1;
    _spriteCycleCount = 513;
    
    // Disable CPU
    _chip.ready(false);
}

template <class TChip>
void Dma<TChip>::startDmc(uint16_t address) {
    // Copy address
    _dmcAddress = address;
    
    // Set counters
    _dmcWaitCycleCount = 2;
    _dmcCycleCount = 2;
}

template <class TChip>
bool Dma<TChip>::isIdle() const {
    // DMA is running only if DMA operations are requested and currently performed
    return (((_dmcCycleCount == 0) || (_dmcCycleCount == 2)) && ((_spriteCycleCount == 0) || (_spriteCycleCount == 513)));
}

template <class TChip>
void Dma<TChip>::process() {
    // Process DMC
    bool canProcessSprite = processDmc();
    
    // Process sprite if necessary
    if (canProcessSprite) {
        processSprite();
    }
}

template <class TChip>
bool Dma<TChip>::processDmc() {
    // If no DMC DMA started
    if (_dmcCycleCount == 0) {
        // Exit
        return true;
    }
    
    // Wait for CPU ending with possible writes
    if (_chip.TChip::InternalCpu::_readWrite == Cpu6502::ReadWrite::Write) {
        return true;
    }
    
    // Disable CPU
    _chip.ready(false);
    
    // If need to wait
    if (_dmcWaitCycleCount > 0) {
        // Decrement counter
        --_dmcWaitCycleCount;
        
        // Exit (and process sprite only if not the sprite extra cycle for restoring the CPU)
        return (_spriteCycleCount != 1);
    }
    
    // Synchronize
    if ((_dmcCycleCount & 0x1) != _writeCycle) {
        return true;//(_spriteCycleCount != 1); // TODO: true ou le comment ? a voir
    }
    
    // Check that we are really on correct cycle
    assert((_dmcCycleCount & 0x1) == _writeCycle);
    
    // Decrement counter
    --_dmcCycleCount;
    
    // Read cycle
    if (!_writeCycle) {
        // Read cycle (only inputDataLatch / predecode / RW from 6502 are affected)
        _chip._bus.setAddressBus(_dmcAddress);
        _chip._readWrite = Cpu6502::ReadWrite::Read;//TODO: pas obligé normalement car copié d'internalcpu dans le clock et il est read car les write cycles ont été passé avant ici
    }
    // Write cycle
    else {
        // Notify APU that sample is fetched
        _chip.apuDmcSampleFetched();
        
        // Restore CPU state (no need to restore read/write signal because last cycle was a read)
        //_chip.setAddressBus((_chip._addressBusHigh << 8) | _chip._addressBusLow);
        _chip._bus.setAddressBus(_chip.getAddressBus());
        
        // If we are in last sprite cycle, we don't need it because we already restore the CPU on this DMC cycle (but we can't simply return true here because we are in DMA write cycle and CPU restore cycle on the sprite happens on the DMA read cycle)
        if (_spriteCycleCount == 1) {
            _spriteCycleCount = 0;
        }
        
        // If no sprite DMA pending
        if (_spriteCycleCount == 0) {
            // Reenable CPU
            _chip.ready(true);
        }
    }
    
    // Exit (we can't process sprite because we process already the DMC)
    return false;
}

template <class TChip>
void Dma<TChip>::processSprite() {
    // If no sprite DMA started
    if (_spriteCycleCount == 0) {
        // Exit
        return;
    }
    
    // If need to wait
    if (_spriteWaitCycleCount > 0) {
        // Decrement counter
        --_spriteWaitCycleCount;
        
        // Exit
        return;
    }
    
    // Synchronize
    if ((_spriteCycleCount & 0x1) == _writeCycle) {
        return;
    }
    
    // Check that we are really on correct cycle
    assert((_spriteCycleCount & 0x1) == (!_writeCycle));
    
    // Decrement counter
    --_spriteCycleCount;
    
    // Read cycle
    if (!_writeCycle) {
        // If last cycle
        if (_spriteCycleCount == 0) {
            // Restore CPU state
            //_chip.setAddressBus((_chip._addressBusHigh << 8) | _chip._addressBusLow);
            //_chip._readWrite = Cpu6502::ReadWrite::Read;//TODO: pas obligé normalement car copié d'internalcpu dans le clock et il est read car les write cycles ont été passé avant ici
            _chip._bus.setAddressBus(_chip.getAddressBus());
            
            // Reenable CPU
            _chip.ready(true);
        } else {
            // Read cycle (only inputDataLatch / predecode / RW from 6502 is affected)
            _chip._bus.setAddressBus(_spriteAddress);
            _chip._readWrite = Cpu6502::ReadWrite::Read;//TODO: pas obligé normalement car copié d'internalcpu dans le clock et il est read car les write cycles ont été passé avant ici
            
            // Increment sprite address
            ++_spriteAddress;
        }
    }
    // Write cycle
    else {
        // Write to PPU (only RW from 6502 is affected)
        _chip._bus.setAddressBus(0x2004);
        _chip._readWrite = Cpu6502::ReadWrite::Write;
    }
}

#endif /* NESEmu_Dma_s_hpp */
