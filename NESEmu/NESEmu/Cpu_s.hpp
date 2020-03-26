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
struct Constants<Model::Ricoh2A03> {//TODO: taper ca dans apu et voir a quoi ca correspond
    // APU
    static constexpr int apuFrameCounterRateInHz = 60;
};

template <>
struct Constants<Model::Ricoh2A07> {
    // APU
    static constexpr int apuFrameCounterRateInHz = 50;
};


template <Model EModel, class TBus, class TSoundHardware>
Chip<EModel, TBus, TSoundHardware>::Chip(TBus &bus, TSoundHardware &soundHardware) : _apu(*this, soundHardware), _bus(bus), InternalCpu(*this), _dmaStarted(false) {
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::powerUp(uint16_t programCounter, uint8_t stackPointer, uint8_t accumulator, uint8_t xIndex, uint8_t yIndex, uint8_t statusFlags) {
    InternalCpu::powerUp(programCounter, stackPointer, accumulator, xIndex, yIndex, statusFlags);
    
    // Power up APU
    _apu.powerUp();
    
    // Reset irq line
    _irqLine = true;
    _apuIrqLine = true;
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::clock() {
    // Execute phi1
    clockPhi1();
    
    // Execute phi2
    clockPhi2();
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::clockPhi1() {//TODO: peut etre a la place d'avoir tout ce bordel pour le dma, avoir un bool isInDma et si true alors on effectue le dma (read ou write) sinon on appelle InternalCpu::clockPhi1(); : a voir car il faut prendre en compte les phi1 phi2 a avoir, les check interrupts, ...
    // End phi2
    endPhi2();
    
    // Start phi1
    startPhi1();
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::clockPhi2() {
    // End phi1
    endPhi1();
    
    // Start phi2
    startPhi2();
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::startPhi1() {
    // Start phi1
    this->_phi2 = false;
    
    // Update PC
    InternalCpu::updateProgramCounter();
    
    // Initialize dataOutput to emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //_dataOutput = 0xFF;
    
    // Check for DMA
    bool needToForceExecute = checkDmaPhi1();
    
    // If rdy is low, wait before perform next read cycle (or write cycle if sync) unless we force execute
    if ((!this->_readyWaitRequested) || ((!this->_sync) && (this->_readWrite != Cpu6502::ReadWrite::Read)) || needToForceExecute) {
        // Execute current stage
        (this->*(this->_currentInstruction))();
    }
    
    // Clock APU
    _apu.clock();//TODO: voir si ok
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::endPhi1() {
    // Read data on dataBus if it is in read mode
    if (this->_readWrite == Cpu6502::ReadWrite::Read) {
        performRead();
    }
    // Set dataBus with last read value in it is in write mode
    else {
        setDataBus(this->_inputDataLatch);//TODO: a voir pq c'est nécessaire pour les tests
    }
    
    // Check for overflow flag
    //InternalCpu::template checkSetOverflow<InternalCpu::SetOverflowEnabled>();
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::startPhi2() {
    // Start phi2
    this->_phi2 = true;
    
    // Check for DMA
    bool needToWrite = checkDmaPhi2();
    
    // Fetch memory for phi2
    if (needToWrite) {
        _bus.performWrite();//TODO: optimisation en mettant _bus devant pour eviter le decodage de l'adresse ici car le dma ecrit tjs en $2004
    } else {
        if (this->_readWrite == Cpu6502::ReadWrite::Write) {
            // Write data to dataBus
            setDataBus(this->_dataOutput);
            performWrite();
        }
    }
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::endPhi2() {
    // Copy data from dataBus on internal registers
    if (this->_readWrite == Cpu6502::ReadWrite::Read) {
        this->_inputDataLatch = getDataBus();
        this->_predecode = this->_inputDataLatch;
    }
    
    // Check reset
    //InternalCpu::template checkReset<InternalCpu::ResetAccurate>();   // TODO: normalement pas besoin car initialisé avec une config performance
    
    // Check interrupts line
    InternalCpu::checkNmi();
    InternalCpu::checkIrq();
    
    // Check ready line
    InternalCpu::checkReady();
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::reset(bool high) {
    // If reset
    if (!high) {
        // Stop possible DMA
        stopDma();
    }
    
    // Reset CPU
    InternalCpu::reset(high);
    
    // Reset APU
    _apu.reset(high);
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::nmi(bool high) {
    InternalCpu::nmi(high);
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::irq(bool high) {
    // Save signal
    _irqLine = high;
    
    // Irq must be low if any of its input is low
    InternalCpu::irq(_irqLine && _apuIrqLine);
}

template <Model EModel, class TBus, class TSoundHardware>
Cpu6502::ReadWrite Chip<EModel, TBus, TSoundHardware>::getReadWriteSignal() const {
    return InternalCpu::getReadWriteSignal();
}

template <Model EModel, class TBus, class TSoundHardware>
uint8_t Chip<EModel, TBus, TSoundHardware>::getOutSignal() const {
    return _outLatch;
}

template <Model EModel, class TBus, class TSoundHardware>
bool Chip<EModel, TBus, TSoundHardware>::getOe1Signal() const { // TODO: sert a activer/desactiver (signal inversé) le port manette 1 (qui active ce signal ???) (peut etre grace a ca pouvoir eviter d'appeler a chaque clock CPU l'update des controllers ?)
}

template <Model EModel, class TBus, class TSoundHardware>
bool Chip<EModel, TBus, TSoundHardware>::getOe2Signal() const { // TODO: pareil qu'au dessus
}

template <Model EModel, class TBus, class TSoundHardware>
bool Chip<EModel, TBus, TSoundHardware>::getM2Signal() const {
    return InternalCpu::getM2Signal();
}

template <Model EModel, class TBus, class TSoundHardware>
uint16_t Chip<EModel, TBus, TSoundHardware>::getAddressBus() const {
    return _bus.getAddressBus();
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::setAddressBus(uint16_t address) {
    _bus.setAddressBus(address);
}

template <Model EModel, class TBus, class TSoundHardware>
uint8_t Chip<EModel, TBus, TSoundHardware>::getDataBus() const {
    return _bus.getDataBus();
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::setDataBus(uint8_t data) {
    _bus.setDataBus(data);
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::setDataBus(uint8_t data, uint8_t mask) {
    _bus.setDataBus(data, mask);
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::performRead() {
    // The CPU itself decodes reads of $4015 (APU status) and $4016-$4017 (controller read). But no circuit in the Control Deck decodes reads from $4000-$4014 or $4018-$7FFF
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    
    uint16_t address = getAddressBus();
    
    // APU status
    if (address == 0x4015) {
        setDataBus(_apu.getStatusRegister(), 0xDF); // TODO: voir si ok
    }
    // Controller 1
    // See https://wiki.nesdev.com/w/index.php/Controller_reading
    // See https://wiki.nesdev.com/w/index.php/Controller_reading_code
    else if (address == 0x4016) {
        _bus.readControllerPort(0); // TODO: voir si pas meilleure conception que ca sinon renommer TBus en THardware car ca ne fait pas que le bus
    }
    // Controller 2
    // Same as controller 1
    else if (address == 0x4017) {
        _bus.readControllerPort(1);
    }
    // External bus
    else {
        _bus.performRead();
    }
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::performWrite() {
    uint16_t address = getAddressBus();
    uint8_t data = getDataBus();
    
    // APU channels
    if ((address >= 0x4000) && (address < 0x4014)) {
        _apu.setChannelRegister(address, data);
    }
    // DMA
    else if (address == 0x4014) {
        startDma(data);
    }
    // APU status
    else if (address == 0x4015) {
        _apu.setStatusRegister(data);
    }
    // Controller 1 / 2
    // See https://wiki.nesdev.com/w/index.php/Controller_reading
    // See https://wiki.nesdev.com/w/index.php/Controller_reading_code
    else if (address == 0x4016) {
        _outLatch = data & 0x7;
    }
    // APU frame counter
    else if (address == 0x4017) {
        _apu.setFrameCounterRegister(data);
    }
    // Test mode
    else if ((address >= 0x4018) && (address < 0x4020)) {
        //TODO: ajouter
    }
    // External bus
    else {
        _bus.performWrite();
    }
}

template <Model EModel, class TBus, class TSoundHardware>
bool Chip<EModel, TBus, TSoundHardware>::checkDmaPhi1() {
    // If DMC started
    if (_dmcStarted) {
        // Decrement DMC counter
        --_dmcCount;
        
        if (_dmcCount == 1) {
            _bus.setAddressBus(_dmcSampleAddress);
            this->_readWrite = Cpu6502::ReadWrite::Read;
        }
        else if (_dmcCount == 0) {
            // Notify APU that sample is fetched
            _apu.dmcSampleFetched(_bus.getDataBus());
            
            // Restore CPU state
            _bus.setAddressBus((this->_addressBusHigh << 8) | this->_addressBusLow);
            
            // Reenable CPU if necessary
            if (!_dmaStarted) {
                InternalCpu::ready(true);
            }
            
            _dmcStarted = false;
        }
        
        return false;
    }
    
    // Don't execute DMA if not asked
    if (!_dmaStarted) {
        return false;
    }
    
    // If DMA is completed
    if (_dmaCount == 0) {
        stopDma();
        
        return false;
    }
    
    // Wait for DMA begin that CPU has terminated current instruction
    if (!InternalCpu::getSyncSignal()) {
        return false;
    }
    
    // If DMA is in read phase
    if (!_dmaToggle) {
        // If DMA begin, wait a last clock to ensure that all operations are finished (some operations does other things in the method which call decode instruction)
        if (_dmaCount > 256) {
            --_dmaCount;
            
            // Force execution because method which call decode instruction is called after the fetch cycle and so RW is read and RDY is low, so if it's not forced, it will not be executed
            return true;
        }
        
        // Read from dmaAddress (only inputDataLatch / predecode / RW from 6502 is affected)
        _bus.setAddressBus((_dmaAddress << 8) | (256 - _dmaCount));
        this->_readWrite = Cpu6502::ReadWrite::Read;
    }
    // If DMA is in write phase
    else {
        // Write to PPU (only RW from 6502 is affected)
        _bus.setAddressBus(0x2004);
        this->_readWrite = Cpu6502::ReadWrite::Write;
        
        // One byte copied
        --_dmaCount;
    }
    
    _dmaToggle = !_dmaToggle;
    
    return false;
}

template <Model EModel, class TBus, class TSoundHardware>
bool Chip<EModel, TBus, TSoundHardware>::checkDmaPhi2() {
    // Need to write if DMA started and begin to read/write and in write mode
    return (_dmaStarted && (_dmaCount <= 256) && (this->_readWrite == Cpu6502::ReadWrite::Write));
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::startDma(uint8_t address) {
    // Set address
    _dmaAddress = address;
    
    // 257 for 1 extra cycle + 256 read-write cycles
    _dmaCount = 257;
    
    // Set DMA flags
    _dmaToggle = false;
    _dmaStarted = true;
    
    // Disable CPU
    InternalCpu::ready(false);
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::stopDma() {
    // Restore CPU state
    _bus.setAddressBus((this->_addressBusHigh << 8) | this->_addressBusLow);
    this->_readWrite = Cpu6502::ReadWrite::Read;
    
    // Reenable CPU
    InternalCpu::ready(true);
    
    // Reset DMA flag
    _dmaStarted = false;
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::apuIrq(bool high) {
    // Save signal
    _apuIrqLine = high;
    
    // Irq must be low if any of its input is low
    InternalCpu::irq(_irqLine && _apuIrqLine);
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::apuRequestDmcSample(uint16_t address) {
    /*if (_dmcStarted) {
        return;
    }
    // Save address
    _dmcSampleAddress = address;
    
    // 4 cycles delay
    _dmcCount = (_dmaStarted) ? 2 : 4;
    
    // Set DMC flag
    _dmcStarted = true;
    
    // Disable CPU
    InternalCpu::ready(false);
    */
    
    uint16_t currentAddress = getAddressBus();
    setAddressBus(address);  //TODO: pas bon, a changer, juste pour tests, il faut faire comme un dma !
    performRead();
    setAddressBus(currentAddress);
    _apu.dmcSampleFetched(getDataBus());
}

#endif /* NESEmu_Cpu_s_hpp */
