//
//  Cpu_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 23/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cpu_s_hpp
#define NESEmu_Cpu_s_hpp


template <>
struct Constants<Model::Ricoh2A03> {
    // APU
    static constexpr int apuFrameCounterRateInHz = 60;
};

template <>
struct Constants<Model::Ricoh2A07> {
    // APU
    static constexpr int apuFrameCounterRateInHz = 50;
};


template <Model EModel, class TBus>
Chip<EModel, TBus>::Chip(TBus &bus) : _bus(bus), _cpu(*this), _dmaToggle(false) {//TODO: dmaToggle dans reset plutot ?
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::clock() {
    // Check for DMA
    bool needToForceExecute = checkDma();
    
    // Cpu clock (force execution if first cycle of DMA)
    _cpu.clock(needToForceExecute == true);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::reset(bool high) {
    _cpu.reset(high);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::nmi(bool high) {
    _cpu.nmi(high);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::irq(bool high) {
    _cpu.irq(high);
}

template <Model EModel, class TBus>
uint16_t Chip<EModel, TBus>::getAddressBus() const {
    return _cpu.getAddressBus();
}

template <Model EModel, class TBus>
uint8_t Chip<EModel, TBus>::getDataBus() const {
    return _cpu.getDataBus();
}

template <Model EModel, class TBus>
bool Chip<EModel, TBus>::getReadWriteSignal() const {
    return _cpu.getReadWriteSignal();
}

template <Model EModel, class TBus>
uint8_t Chip<EModel, TBus>::read(uint16_t address) {
    // The CPU itself decodes reads of $4015 (APU status) and $4016-$4017 (controller read). But no circuit in the Control Deck decodes reads from $4000-$4014 or $4018-$7FFF
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    
    // Sound
    if (address == 0x4015) {
        return 0x0; // TODO: changer
    }
    // Joystick 1
    else if (address == 0x4016) {
        return 0x0; // TODO: changer
    }
    // Joystick 2
    else if (address == 0x4017) {
        return 0x0; // TODO: changer
    }
    
    // External bus
    return _bus.read(address);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::write(uint16_t address, uint8_t data) {
    // Sound
    if (((address >= 0x4000) && (address < 0x4014)) || (address == 0x4015)) {
        //TODO: ajouter
        return;
    }
    // DMA
    else if (address == 0x4014) {
        // Start a DMA transfert (257 for 1 extra cycle + 256 read-write cycles)
        _dmaAddress = data;
        _dmaCount = 257;
        _cpu.ready(false);
        
        return;
    }
    // Joystick 1
    else if (address == 0x4016) {
        //TODO: ajouter
        return;
    }
    // Joystick 2
    else if (address == 0x4017) {
        //TODO: ajouter
        return;
    }
    // Test mode
    else if ((address >= 0x4018) && (address < 0x4020)) {
        //TODO: ajouter
        return;
    }
    
    // External bus
    _bus.write(address, data);
}

template <Model EModel, class TBus>
bool Chip<EModel, TBus>::checkDma() {   // TODO: j'ai fait cette fonction qui retourne un bool pour pouvoir executer le decodeOpcode quand rdy est low car avant le decode il y a le fetch et donc le rw sera en read et donc dans clock du 6502 il n'executera plus d'instructions, mais j'ai besoin qu'il execute decode dans le 1er step du dma, mais voir si pas moyen de faire mieux que ca (plus optimisé)
    // Don't execute DMA if not asked
    if (_dmaCount == 0) {
        return false;
    }
    
    // Wait for DMA begin that CPU has terminated current instruction
    if (_cpu.getSyncSignal() == false) {
        return false;
    }
    
    // If DMA is in read phase
    if (_dmaToggle == false) {
        // If DMA begin, wait a last clock to ensure that all operations are finished (some operations does other things in the method which call decode instruction)
        if (_dmaCount == 257) {
            --_dmaCount;
            
            // Force execution because method which call decode instruction is called after the fetch cycle and so RW is read and RDY is low, so if it's not forced, it will not be executed
            return true;
        }
        
        // Read from dmaAddress (only inputDataLatch / predecode / RW from 6502 is affected)
        _cpu._readWrite = static_cast<bool>(InternalCpu::ReadWrite::Read);
        _cpu._inputDataLatch = read((_dmaAddress << 8) | (256 - _dmaCount));
        _cpu._predecode = _cpu._inputDataLatch;
    }
    // If DMA is in write phase
    else {
        // Write to PPU (only RW from 6502 is affected)
        _cpu._readWrite = static_cast<bool>(InternalCpu::ReadWrite::Write);     // TODO: voir si un reset signal pendant le dma, si write devient read
        write(0x2004, _cpu._inputDataLatch);
        
        // One byte copied
        --_dmaCount;
        
        // Reenable CPU if DMA is completed
        if (_dmaCount == 0) {
            _cpu.ready(true);
        }
    }
    
    _dmaToggle = !_dmaToggle;
    
    return false;
}

#endif /* NESEmu_Cpu_s_hpp */
