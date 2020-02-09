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
Chip<EModel, TBus>::Chip(TBus &bus) : _bus(bus), InternalCpu(*this), _dmaStarted(false), _dmaCount(0), _dmaToggle(false) {//TODO: dmaToggle dans reset plutot ?
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::powerUp(uint16_t programCounter, uint8_t stackPointer, uint8_t accumulator, uint8_t xIndex, uint8_t yIndex, uint8_t statusFlags) {
    InternalCpu::powerUp(programCounter, stackPointer, accumulator, xIndex, yIndex, statusFlags);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::clock() {
    // Execute phi1
    clockPhi1();
    
    // Execute phi2
    clockPhi2();
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::clockPhi1() {
    // Start phi1
    this->_phi2 = false;
    
    // Update PC
    InternalCpu::updateProgramCounter();
    
    // Check for overflow flag
    //InternalCpu::template checkSetOverflow<InternalCpu::SetOverflowEnabled>();
    
    // Initialize dataOutput to emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //this->_dataOutput = 0xFF;
    
    // Check for DMA
    bool needToForceExecute = checkDmaPhi1();
    
    // If rdy is low, wait before perform next read cycle unless we force execute TODO: j'ai mis sync en plus pour arreter sur le fetchOpcode
    if ((this->_readyWaitRequested == false) || ((this->_readWrite != static_cast<bool>(InternalCpu::ReadWrite::Read)) && (/*this->_sync == false*/true)) || (needToForceExecute == true)) {
        // Execute current stage
        (this->*(this->_currentInstruction))();
    }
    
    // Fetch memory for phi1
    InternalCpu::fetchMemoryPhi1();
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::clockPhi2() {
    // Start phi2
    this->_phi2 = true;
    
    // Check reset
    //InternalCpu::template checkReset<InternalCpu::ResetAccurate>();   // TODO: normalement pas besoin car initialisé avec une config performance
    
    // Check interrupts line
    InternalCpu::checkNmi();
    InternalCpu::checkIrq();
    
    // Check ready line
    InternalCpu::checkReady();
    
    // Check for DMA
    bool needToWrite = checkDmaPhi2();
    
    // Fetch memory for phi2
    if (needToWrite == true) {
        write(this->_addressBus, this->_dataBus);
    } else {
        InternalCpu::fetchMemoryPhi2();
    }
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::reset(bool high) {
    InternalCpu::reset(high);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::nmi(bool high) {
    InternalCpu::nmi(high);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::irq(bool high) {
    InternalCpu::irq(high);
}

template <Model EModel, class TBus>
uint16_t Chip<EModel, TBus>::getAddressBus() const {
    return InternalCpu::getAddressBus();
}

template <Model EModel, class TBus>
uint8_t Chip<EModel, TBus>::getDataBus() const {
    return InternalCpu::getDataBus();
}

template <Model EModel, class TBus>
bool Chip<EModel, TBus>::getReadWriteSignal() const {
    return InternalCpu::getReadWriteSignal();
}

template <Model EModel, class TBus>
bool Chip<EModel, TBus>::getM2Signal() const {
    return InternalCpu::getM2Signal();
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
        _dmaStarted = true;
        InternalCpu::ready(false);
        
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
bool Chip<EModel, TBus>::checkDmaPhi1() {
    // Don't execute DMA if not asked
    if (_dmaStarted == false) {
        return false;
    }
    
    // If DMA is completed
    if (_dmaCount == 0) {
        // Restore CPU state
        this->_readWrite = static_cast<bool>(InternalCpu::ReadWrite::Read);
        this->_addressBus = (this->_addressBusHigh << 8) | this->_addressBusLow;
        
        // Reenable CPU
        InternalCpu::ready(true);
        
        // Reset DMA flag
        _dmaStarted = false;
        
        return false;
    }
    
    // Wait for DMA begin that CPU has terminated current instruction
    if (InternalCpu::getSyncSignal() == false) {
        return false;
    }
    
    // If DMA is in read phase
    if (_dmaToggle == false) {
        // If DMA begin, wait a last clock to ensure that all operations are finished (some operations does other things in the method which call decode instruction)
        if (_dmaCount > 256) {
            --_dmaCount;
            
            // Force execution because method which call decode instruction is called after the fetch cycle and so RW is read and RDY is low, so if it's not forced, it will not be executed
            return true;
        }
        
        // Read from dmaAddress (only inputDataLatch / predecode / RW from 6502 is affected)
        this->_readWrite = static_cast<bool>(InternalCpu::ReadWrite::Read);
        this->_addressBus = (_dmaAddress << 8) | (256 - _dmaCount);
    }
    // If DMA is in write phase
    else {
        // Write to PPU (only RW from 6502 is affected)
        this->_readWrite = static_cast<bool>(InternalCpu::ReadWrite::Write);     // TODO: voir si un reset signal pendant le dma, si write devient read
        this->_addressBus = 0x2004;
        
        // One byte copied
        --_dmaCount;
    }
    
    _dmaToggle = !_dmaToggle;
    
    return false;
}

template <Model EModel, class TBus>
bool Chip<EModel, TBus>::checkDmaPhi2() {
    // Need to write if DMA started and begin to read/write and in write mode
    return ((_dmaStarted == true) && (_dmaCount <= 256) && (this->_readWrite == static_cast<bool>(InternalCpu::ReadWrite::Write)));
}

#endif /* NESEmu_Cpu_s_hpp */
