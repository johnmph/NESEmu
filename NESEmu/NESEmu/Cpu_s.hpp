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
struct Constants<Model::Ricoh2A03> {//TODO: taper ca dans apu mais voir si necessaire
    // APU
    static constexpr int apuFrameCounterRateInHz = 60;
};

template <>
struct Constants<Model::Ricoh2A07> {
    // APU
    static constexpr int apuFrameCounterRateInHz = 50;
};


template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
Chip<EModel, TBus, TControllerHardware, TSoundManager>::Chip(TBus &bus, TControllerHardware &controllerHardware, TSoundManager &soundManager) : _apu(*this, soundManager), _dma(*this), _bus(bus), _controllerHardware(controllerHardware), InternalCpu(*this) {
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::powerUp(uint16_t programCounter, uint8_t stackPointer, uint8_t accumulator, uint8_t xIndex, uint8_t yIndex, uint8_t statusFlags) {
    // Power up CPU
    InternalCpu::powerUp(programCounter, stackPointer, accumulator, xIndex, yIndex, statusFlags);
    
    // Power up APU
    _apu.powerUp();
    
    // Power up DMA
    _dma.powerUp();
    
    // Internal
    //_6502BusAddress = 0x0;//TODO: normalement pas besoin mais verifier quand meme
    //_6502BusData = 0x0;
    _readWrite = Cpu6502::ReadWrite::Read;//TODO: voir si nécessaire car copié du rw du 6502 si pas dma : oui si dma clock apres l'instruction !
    
    _outLatch = 0x0;
    
    _irqLine = true;
    _apuIrqLine = true;
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::clock() {
    // Execute phi1
    clockPhi1();
    
    // Execute phi2
    clockPhi2();
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::clockPhi1() {
    // End phi2
    endPhi2();
    
    // Start phi1
    startPhi1();
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::clockPhi2() {
    // End phi1
    endPhi1();
    
    // Start phi2
    startPhi2();
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::startPhi1() {
    // Start phi1
    InternalCpu::_phi2 = false;
    
    // Update PC
    InternalCpu::updateProgramCounter();
    
    // Initialize dataOutput to emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //_dataOutput = 0xFF;
    
    // If rdy is low, wait before perform next read cycle
    if ((!InternalCpu::_readyWaitRequested) || (InternalCpu::_readWrite != Cpu6502::ReadWrite::Read)) {
        //if ((!this->_readyWaitRequested) || ((!this->_sync) && (this->_readWrite != Cpu6502::ReadWrite::Read)) || needToForceExecute) {//TODO: peut etre a la place de needToForceExecute, appeler simplement l'instruction mais a chaque fois qu'on doit passer a l'instruction d'apres en settant _currentInstruction on le fait via une methode setNextInstruction qui va checker si readWaitRequested et pas write et si oui ne pas setter la next instruction mais voir si executer plusieurs fois l'instruction en cours ne pose pas de probleme (si un add ou autre est ce que ca va faire plusieurs add a la suite car dans ce cas ce n'est pas bon) : tout ceci pour avoir les operations qui s'execute en parallele d'un fetch ou d'un decode opcode (alu) : TODO: dmaWithASO foire a cause de ca car l'accu n'est pas mis a jour avant (pendant) le dma
        // Execute current stage
        (this->*(InternalCpu::_currentInstruction))();
    }
    
    // Clock APU
    _apu.clock();
    
    // Clock DMA
    _dma.clock();
    
    // Clock controller hardware
    _controllerHardware.clock(*this);
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::endPhi1() {
    //InternalCpu::endPhi1();
    // Read data on dataBus if it is in read mode
    if (_readWrite == Cpu6502::ReadWrite::Read) {
        // DMA idle
        if (_dma.isIdle()) {
            // Perform read
            performRead();
        }
        // DMA running
        else {
            // Perform read on external bus
            _bus.performRead();
            
            // Update internal data bus with external data bus
            setDataBus(_bus.getDataBus());
        }
    }
    // Set dataBus with last read value in it is in write mode
    /*else {
        setDataBus(this->_inputDataLatch);//TODO: a voir pq c'est nécessaire pour les tests
    }*/
    
    // Check for overflow flag
    //InternalCpu::template checkSetOverflow<InternalCpu::SetOverflowEnabled>();
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::startPhi2() {
    // Start phi2
    InternalCpu::_phi2 = true;
    
    // Fetch memory for phi2
    if (_readWrite == Cpu6502::ReadWrite::Write) {
        // DMA idle
        if (_dma.isIdle()) {
            // Write dataOutput to dataBus
            setDataBus(InternalCpu::_dataOutput);
            
            // Perform write
            performWrite();
        }
        // DMA running
        else {
            // Update external data bus with internal data bus
            //_bus.setDataBus(getDataBus());    // TODO: pas besoin car deja passé par le bus !
            
            // Perform write on external data bus
            _bus.performWrite();
        }
    }
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::endPhi2() {
    //InternalCpu::endPhi2();
    // Copy data from dataBus on internal registers
    //if (this->_readWrite == Cpu6502::ReadWrite::Read) {//TODO: par apres voir si pas toujours lu meme en write (tester avec visual) (peut etre plus besoin du setDataBus(this->_inputDataLatch); dans endPhi1 si ca !!! a voir
    uint16_t address = getAddressBus();
    if ((InternalCpu::_readWrite == Cpu6502::ReadWrite::Read) && (address != 0x4015)) {//TODO: voir si pas plus optimisé et plus propre !!! (a cause du delai du PPU read !
        setDataBus(_bus.getDataBus());
    }
    
        InternalCpu::_inputDataLatch = getDataBus();
        InternalCpu::_predecode = InternalCpu::_inputDataLatch;
    //}
    
    // Check reset
    //InternalCpu::template checkReset<InternalCpu::ResetAccurate>();   // TODO: normalement pas besoin car initialisé avec une config performance
    
    // Check interrupts line
    InternalCpu::checkNmi();
    InternalCpu::checkIrq();
    
    // Check ready line
    InternalCpu::checkReady();
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::reset(bool high) {
    // Reset CPU
    InternalCpu::reset(high);
    
    // Reset APU
    _apu.reset(high);
    
    // Reset DMA
    _dma.reset(high);
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::nmi(bool high) {
    InternalCpu::nmi(high);
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::irq(bool high) {
    // Save signal
    _irqLine = high;
    
    // Irq must be low if any of its input is low
    InternalCpu::irq(_irqLine && _apuIrqLine);
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
float Chip<EModel, TBus, TControllerHardware, TSoundManager>::getAd1Signal() const {//TODO: utiliser ca pour le sound manager plutot que le systeme de maintenant ?
    return _apu.getSquareOutput();
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
float Chip<EModel, TBus, TControllerHardware, TSoundManager>::getAd2Signal() const {
    return _apu.getTriangleNoiseDmcOutput();
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
Cpu6502::ReadWrite Chip<EModel, TBus, TControllerHardware, TSoundManager>::getReadWriteSignal() const {//TODO: voir lequel retourner !! normalement l'externe donc _readWrite
    return _readWrite;//InternalCpu::getReadWriteSignal();
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
uint8_t Chip<EModel, TBus, TControllerHardware, TSoundManager>::getOutSignal() const {
    return _outLatch;
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
bool Chip<EModel, TBus, TControllerHardware, TSoundManager>::getOe1Signal() const {
    return !((_bus.getAddressBus() == 0x4016) && (_readWrite == Cpu6502::ReadWrite::Read));
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
bool Chip<EModel, TBus, TControllerHardware, TSoundManager>::getOe2Signal() const {
    return !((_bus.getAddressBus() == 0x4017) && (_readWrite == Cpu6502::ReadWrite::Read));
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
bool Chip<EModel, TBus, TControllerHardware, TSoundManager>::getM2Signal() const {
    return InternalCpu::getM2Signal();
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
uint16_t Chip<EModel, TBus, TControllerHardware, TSoundManager>::getAddressBus() const {
    //return _bus.getAddressBus();
    
    // Get address
    return _6502BusAddress;
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::setAddressBus(uint16_t address) {
    //_bus.setAddressBus(address);
    
    // Set address
    _6502BusAddress = address;
    
    // Set address on external address bus
    _bus.setAddressBus(address);
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
uint8_t Chip<EModel, TBus, TControllerHardware, TSoundManager>::getDataBus() const {
    //return _bus.getDataBus();
    
    // Get data with possibly open data bus latch on some or all lines
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    return _6502BusData;
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::setDataBus(uint8_t data) {
    //_bus.setDataBus(data);
    
    // Set data
    _6502BusData = data;
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::setDataBus(uint8_t data, uint8_t mask) {
    //_bus.setDataBus(data, mask);
    
    // Set data with possibly open data bus latch on some or all lines
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    _6502BusData = (_6502BusData & ~mask) | (data & mask);
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::performRead() {
    // The CPU itself decodes reads of $4015 (APU status) and $4016-$4017 (controller read). But no circuit in the Control Deck decodes reads from $4000-$4014 or $4018-$7FFF
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    
    uint16_t address = getAddressBus();
    
    // APU status
    if (address == 0x4015) {
        setDataBus(_apu.getStatusRegister(), 0xDF); // TODO: voir si ok, ne passe pas par l'external data bus !!!
        
        _bus.setDataBus(_last4015DataWritten);
    }
    // Controller 1
    // See https://wiki.nesdev.com/w/index.php/Controller_reading
    // See https://wiki.nesdev.com/w/index.php/Controller_reading_code
    else if (address == 0x4016) {
        // Get current data bus
        uint8_t dataBus = _bus.getDataBus();
        
        // Read controller port 0
        _bus.setDataBus(_controllerHardware.readControllerPort(0, dataBus));
        
        // Get data from external data bus
        setDataBus(_bus.getDataBus());
    }
    // Controller 2
    // Same as controller 1
    else if (address == 0x4017) {
        // Get current data bus
        uint8_t dataBus = _bus.getDataBus();
        
        // Read controller port 0
        _bus.setDataBus(_controllerHardware.readControllerPort(1, dataBus));
        
        // Get data from external data bus
        setDataBus(_bus.getDataBus());
    }
    // External bus
    else {
        // Perform read
        _bus.performRead();
        
        // Get data from external data bus
        setDataBus(_bus.getDataBus());
    }
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::performWrite() {
    uint16_t address = getAddressBus();
    uint8_t data = getDataBus();
    
    // Set data on external data bus
    _bus.setDataBus(data);//TODO: plutot dans le startPhi2 ?
    
    // APU channels
    if ((address >= 0x4000) && (address < 0x4014)) {
        _apu.setChannelRegister(address, data);
    }
    // Sprite DMA
    else if (address == 0x4014) {
        _dma.startSprite(data);
    }
    // APU status
    else if (address == 0x4015) {
        _apu.setStatusRegister(data);
        _last4015DataWritten = data;
    }
    // Controller 1 / 2
    // See https://wiki.nesdev.com/w/index.php/Controller_reading
    // See https://wiki.nesdev.com/w/index.php/Controller_reading_code
    else if (address == 0x4016) {
        // Set outLatch
        _outLatch = data & 0x7;
        
        // Set out 0 of controller hardware
        _controllerHardware.setOut0((_outLatch & 0x1) != 0);
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
        // Perform write
        _bus.performWrite();
    }
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::apuIrq(bool high) {
    // Save signal
    _apuIrqLine = high;
    
    // Irq must be low if any of its input is low
    InternalCpu::irq(_irqLine && _apuIrqLine);
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::apuDmcRequestSample(uint16_t address) {
    // Start DMC DMA
    _dma.startDmc(address);
}

template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
void Chip<EModel, TBus, TControllerHardware, TSoundManager>::apuDmcSampleFetched() {
    // Notify APU that a DMC sample is fetched
    _apu.dmcSampleFetched(getDataBus());
}

#endif /* NESEmu_Cpu_s_hpp */
