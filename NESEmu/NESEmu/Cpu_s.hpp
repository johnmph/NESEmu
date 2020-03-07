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
Chip<EModel, TBus>::Chip(TBus &bus) : _bus(bus), InternalCpu(*this), _dmaStarted(false) {
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
void Chip<EModel, TBus>::clockPhi1() {//TODO: peut etre a la place d'avoir tout ce bordel pour le dma, avoir un bool isInDma et si true alors on effectue le dma (read ou write) sinon on appelle InternalCpu::clockPhi1(); : a voir car il faut prendre en compte les phi1 phi2 a avoir, les check interrupts, ...
    // Fetch memory for phi1
    fetchMemoryPhi1();
    
    // Check interrupts line (before phi1 begins)
    InternalCpu::checkNmi();
    InternalCpu::checkIrq();
    
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
    
    // If rdy is low, wait before perform next read cycle (or write cycle if sync) unless we force execute
    if ((!this->_readyWaitRequested) || ((!this->_sync) && (this->_readWrite != Cpu6502::ReadWrite::Read)) || needToForceExecute) {
        // Execute current stage
        (this->*(this->_currentInstruction))();
    }
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::clockPhi2() {
    // Start phi2
    this->_phi2 = true;
    
    // Check reset
    //InternalCpu::template checkReset<InternalCpu::ResetAccurate>();   // TODO: normalement pas besoin car initialisé avec une config performance
    
    // Check interrupts line
    //InternalCpu::checkNmi();
    //InternalCpu::checkIrq();
    
    // Check ready line
    InternalCpu::checkReady();
    
    // Check for DMA
    bool needToWrite = checkDmaPhi2();
    
    // Fetch memory for phi2
    if (needToWrite) {
        _bus.performWrite();//TODO: optimisation en mettant _bus devant pour eviter le decodage de l'adresse ici car le dma ecrit tjs en $2004
    } else {
        fetchMemoryPhi2();
    }
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::reset(bool high) {
    // If reset
    if (!high) {
        // Stop possible DMA
        stopDma();
    }
    
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
bool Chip<EModel, TBus>::getReadWriteSignal() const {
    return InternalCpu::getReadWriteSignal();
}

template <Model EModel, class TBus>
uint8_t Chip<EModel, TBus>::getOutSignal() const {
    return _outLatch;
}

template <Model EModel, class TBus>
bool Chip<EModel, TBus>::getOe1Signal() const { // TODO: sert a activer/desactiver (signal inversé) le port manette 1 (qui active ce signal ???) (peut etre grace a ca pouvoir eviter d'appeler a chaque clock CPU l'update des controllers ?)
}

template <Model EModel, class TBus>
bool Chip<EModel, TBus>::getOe2Signal() const { // TODO: pareil qu'au dessus
}

template <Model EModel, class TBus>
bool Chip<EModel, TBus>::getM2Signal() const {
    return InternalCpu::getM2Signal();
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::fetchMemoryPhi1() {
    // dataBus is already filled with data since end of previous phi2, just put dataBus on internal registers
    if (this->_readWrite == Cpu6502::ReadWrite::Read) {
        this->_inputDataLatch = getDataBus();
        this->_predecode = this->_inputDataLatch;
        
        return;
    }
    
    //setDataBus(this->_inputDataLatch);   // TODO: voir si ca n'a pas d'influence sur le open bus behaviour !!! (j'ai besoin de ca sinon les tests foirent)
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::fetchMemoryPhi2() {
    // Read data on dataBus if it is in read mode
    if (this->_readWrite == Cpu6502::ReadWrite::Read) {
        performRead();
        
        return;
    }
    
    // Write data to dataBus if it is in write mode
    setDataBus(this->_dataOutput);
    performWrite();
}

template <Model EModel, class TBus>
uint16_t Chip<EModel, TBus>::getAddressBus() const {
    return _bus.getAddressBus();
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::setAddressBus(uint16_t address) {
    _bus.setAddressBus(address);
}

template <Model EModel, class TBus>
uint8_t Chip<EModel, TBus>::getDataBus() const {
    return _bus.getDataBus();
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::setDataBus(uint8_t data) {
    _bus.setDataBus(data);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::setDataBus(uint8_t data, uint8_t mask) {
    _bus.setDataBus(data, mask);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::performRead() {
    // The CPU itself decodes reads of $4015 (APU status) and $4016-$4017 (controller read). But no circuit in the Control Deck decodes reads from $4000-$4014 or $4018-$7FFF
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    
    uint16_t address = getAddressBus();
    
    // APU
    if (address == 0x4015) {
        //setDataBus(0x0); // TODO: changer
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

template <Model EModel, class TBus>
void Chip<EModel, TBus>::performWrite() {
    uint16_t address = getAddressBus();
    uint8_t data = getDataBus();
    
    // APU
    if (((address >= 0x4000) && (address < 0x4014)) || (address == 0x4015)) {
        //TODO: ajouter
    }
    // DMA
    else if (address == 0x4014) {
        startDma(data);
    }
    // Controller 1 / 2
    // See https://wiki.nesdev.com/w/index.php/Controller_reading
    // See https://wiki.nesdev.com/w/index.php/Controller_reading_code
    else if (address == 0x4016) {   // TODO: il faut aussi notifier les manettes qu'on a ecrit ca en appelant leur out(_outLatch & 0x1) !!! : je l'ai mis dans le clock nes cpu, voir si pas moyen de faire mieux optimisé que ca !!
        _outLatch = data & 0x7;
    }
    // APU frame counter
    else if (address == 0x4017) {
        //TODO: ajouter
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

template <Model EModel, class TBus>
bool Chip<EModel, TBus>::checkDmaPhi1() {
    // Don't execute DMA if not asked
    if (!_dmaStarted) {
        return false;
    }
    
    // If DMA is completed
    if (_dmaCount == 0) {
        stopDma();
        
        return true;//TODO: voir, j'ai mis true pour forcer l'execution du cpu a la fin du DMA pour ne pas perdre un cycle CPU a la fin d'un DMA, a tester
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

template <Model EModel, class TBus>
bool Chip<EModel, TBus>::checkDmaPhi2() {
    // Need to write if DMA started and begin to read/write and in write mode
    return (_dmaStarted && (_dmaCount <= 256) && (this->_readWrite == Cpu6502::ReadWrite::Write));
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::startDma(uint8_t address) {
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

template <Model EModel, class TBus>
void Chip<EModel, TBus>::stopDma() {
    // Restore CPU state
    _bus.setAddressBus((this->_addressBusHigh << 8) | this->_addressBusLow);
    this->_readWrite = Cpu6502::ReadWrite::Read;
    
    // Reenable CPU
    InternalCpu::ready(true);
    
    // Reset DMA flag
    _dmaStarted = false;
}

#endif /* NESEmu_Cpu_s_hpp */
