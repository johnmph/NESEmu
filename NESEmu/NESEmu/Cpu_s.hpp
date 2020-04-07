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
Chip<EModel, TBus, TSoundHardware>::Dma::Dma(Chip &chip) : _chip(chip), _spriteAddress(0x0), _dmcAddress(0x0), _spriteCycleCount(0), _dmcCycleCount(0), _spriteWaitCycleCount(0), _dmcWaitCycleCount(0), _writeCycle(false)/*, _idle(true)*/, _ready(true) {//TODO: j'ai du mettre writeCycle a false, pq ???? (car il est switché avant le process) alors pq??? : 7 cycles dans le reset / 9 dans le power up, un nombre impair donc on commence en write cycle
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::Dma::clockPhi1() {
    // Toggle read / write cycle before process so we have the same type cycle in process method and CPU instruction execution (which is right after this clock method)
    _writeCycle = !_writeCycle;
    
    // Copy read write signal
    _chip._readWrite = _chip.InternalCpu::_readWrite;//TODO: a voir
    
    // Process
    process();
    
    //_writeCycle = !_writeCycle;//TODO: voir si ici ou au dessus (si ici, writeCycle doit etre initialisé a true)
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::Dma::clockPhi2() {
    // Set ready line
    _chip.ready(_ready);
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::Dma::startSprite(uint8_t address) {
    // Calculate address
    _spriteAddress = address << 8;
    
    // Set count
    _spriteWaitCycleCount = (_dmcCycleCount > 0) ? 0 : 1/* + _writeCycle*/;
    _spriteCycleCount = 513;    // TODO: besoin d'un cycle en plus pour la restauration du CPU apres le DMA (ce cycle executera l'instruction du CPU) TODO: voir si besoin de ce cycle supplémentaire si dmc dma arrive a ce moment
    
    // Disable CPU
    _chip.ready(false);
    _ready = false;
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::Dma::startDmc(uint16_t address, bool requestedOnEnable) {
    // Ensure that DMC DMA start on an even (read) cycle
    //assert(_writeCycle);//TODO: write cycle car apu clocké apres le dma et donc process en even deja effectué si on a !_writeCycle
    
    // Don't start DMC if already started
    if (_dmcCycleCount > 0) {
        //return;
        std::cout << "Started DMC DMA before last DMC DMA end\n";
    }
    
    // Copy address
    _dmcAddress = address;
    
    // Set count
    _dmcWaitCycleCount = (_spriteCycleCount > 0) ? 0 : 2;
    _dmcCycleCount = (_spriteCycleCount > 0) ? 2 : 4;
    //_spriteWaitCycleCount = 0;//TODO: a voir
    
    // Reset read first sync flag
    _dmcReadFirstSync = false;
    
    if ((_spriteCycleCount == 0) && (!requestedOnEnable)) {
        _dmcWaitCycleCount = 1;
        _dmcReadFirstSync = true;
    }
    
    // Disable CPU
    //_chip.ready(false);
    
    if (_spriteCycleCount > 0) {
        std::cout << std::hex << "DMC in SPR begin : " << +((_chip._addressBusHigh << 8) | _chip._addressBusLow) << ", " << +_chip.getAddressBus() << ", " << +_chip.getDataBus() << std::dec << "\n";
    }
}

template <Model EModel, class TBus, class TSoundHardware>
bool Chip<EModel, TBus, TSoundHardware>::Dma::isWriteCycle() const { // TODO: retirer si pas besoin mais normalement l'apu en a besoin je pense!
    return _writeCycle;
}
/*
template <Model EModel, class TBus, class TSoundHardware>
bool Chip<EModel, TBus, TSoundHardware>::Dma::isIdle() const {
    //return ((_dmcCycleCount == 0) && (_spriteCycleCount == 0)) || (_waitCycleCount > 0);//TODO: ne fonctionne pas car waitCycleCount est decrementé avant d'etre checké !
    return _idle;
}*/

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::Dma::process() {
    // If DMC DMA is started
    if (_dmcCycleCount > 0) {
        // If need to wait
        if (_dmcWaitCycleCount > 0) {
            // Decrement counter
            --_dmcWaitCycleCount;
            
            // Exit
            return;
        }
        
        // Synchronize first time (before disabling CPU)
        if (((_dmcCycleCount & 0x1) != _writeCycle) && (!_dmcReadFirstSync)) {
            _dmcReadFirstSync = true;
            
            return;
        }
        
        // Wait for CPU ending with possible writes
        if (_chip.InternalCpu::_readWrite == Cpu6502::ReadWrite::Write) {//TODO: internal ou pas ?? normalement oui
            return;
        }
        
        // Disable CPU
        //_chip.ready(false);
        _ready = false;
        
        // Synchronize second time (after disabling CPU)
        if ((_dmcCycleCount & 0x1) != _writeCycle) {
            return;
        }
        
        // Check that we are really on correct cycle
        assert((_dmcCycleCount & 0x1) == _writeCycle);
        
        // Set idle flag
        //_idle = false;
        
        // Decrement counter
        --_dmcCycleCount;
        
        // Read cycle
        if (!_writeCycle) {
            if (_dmcCycleCount > 1) {
                return;
            }
            
            // Read cycle (only inputDataLatch / predecode / RW from 6502 are affected)
            _chip.setAddressBus(_dmcAddress);
            _chip._readWrite = Cpu6502::ReadWrite::Read;//TODO: pas obligé normalement car copié d'internalcpu dans le clock et il est read car les write cycles ont été passé avant ici
        }
        // Write cycle
        else {
            if (_dmcCycleCount > 0) {
                return;
            }
            
            // Notify APU that sample is fetched
            _chip.apuDmcSampleFetched();
            
            // Restore CPU state (no need to restore read/write signal because last cycle was a read)
            _chip.setAddressBus((_chip._addressBusHigh << 8) | _chip._addressBusLow);
            
            // If no sprite DMA pending
            if (_spriteCycleCount == 0) {
                // Reenable CPU
                _chip.ready(true);
                _ready = true;
                
                // Set idle flag
                //_idle = true;
            }/* else {
                std::cout << std::hex << "DMC in SPR end : " << +((_chip._addressBusHigh << 8) | _chip._addressBusLow) << ", " << +_chip.getAddressBus() << ", " << +_chip.getDataBus() <<  "\n";
            }*/
        }
        
        // Exit
        return;
    }
    
    // If Sprite DMA is started
    if (_spriteCycleCount > 0) {
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
        
        // Set idle flag
        //_idle = false;
        
        // Decrement counter
        --_spriteCycleCount;
        
        // Read cycle
        if (!_writeCycle) {
            // If last cycle
            if (_spriteCycleCount == 0) {
                // Restore CPU state
                _chip.setAddressBus((_chip._addressBusHigh << 8) | _chip._addressBusLow);
                //_chip._readWrite = Cpu6502::ReadWrite::Read;//TODO: pas obligé normalement car copié d'internalcpu dans le clock et il est read car les write cycles ont été passé avant ici
                
                // Reenable CPU
                _chip.ready(true);
                _ready = true;
                
                // Set idle flag
                //_idle = true;
            } else {
                // Read cycle (only inputDataLatch / predecode / RW from 6502 is affected)
                _chip.setAddressBus(_spriteAddress);
                _chip._readWrite = Cpu6502::ReadWrite::Read;//TODO: pas obligé normalement car copié d'internalcpu dans le clock et il est read car les write cycles ont été passé avant ici
                
                // Increment sprite address
                ++_spriteAddress;
            }
        }
        // Write cycle
        else {
            //std::cout << +_chip.getDataBus() << "\n";
            
            // Write to PPU (only RW from 6502 is affected)
            _chip.setAddressBus(0x2004);
            _chip._readWrite = Cpu6502::ReadWrite::Write;
        }
    }
}


template <Model EModel, class TBus, class TSoundHardware>
Chip<EModel, TBus, TSoundHardware>::Chip(TBus &bus, TSoundHardware &soundHardware) : _apu(*this, soundHardware), _bus(bus), _dma(*this), InternalCpu(*this) {
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::powerUp(uint16_t programCounter, uint8_t stackPointer, uint8_t accumulator, uint8_t xIndex, uint8_t yIndex, uint8_t statusFlags) {
    InternalCpu::powerUp(programCounter, stackPointer, accumulator, xIndex, yIndex, statusFlags);
    
    // Power up APU
    _apu.powerUp();
    
    // Internal
    _readWrite = Cpu6502::ReadWrite::Read;//TODO: voir si nécessaire car copié du rw du 6502 si pas dma : oui si dma clock apres l'instruction !
    
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
void Chip<EModel, TBus, TSoundHardware>::clockPhi1() {
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
    //_apu.clock();//TODO: je l'ai mis ici pour les tests, a voir
    
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
    _apu.clock();//TODO: voir si ok
    
    // Clock DMA
    _dma.clockPhi1();//TODO: le mettre apres l'instruction ??
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::endPhi1() {
    InternalCpu::endPhi1();
    /*// Read data on dataBus if it is in read mode
    if (_readWrite == Cpu6502::ReadWrite::Read) {
        performRead();
    }
    // Set dataBus with last read value in it is in write mode
    /*else {
        setDataBus(this->_inputDataLatch);//TODO: a voir pq c'est nécessaire pour les tests
    }/
    
    // Check for overflow flag
    //InternalCpu::template checkSetOverflow<InternalCpu::SetOverflowEnabled>();*/
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::startPhi2() {
    // Start phi2
    InternalCpu::_phi2 = true;
    
    // Fetch memory for phi2
    if (_readWrite == Cpu6502::ReadWrite::Write) {
        // If internal read/write signal is not the same than external, it's because a DMA is in progress
        if (InternalCpu::_readWrite != Cpu6502::ReadWrite::Write) {
            _bus.performWrite();//TODO: optimisation en mettant _bus devant pour eviter le decodage de l'adresse ici car le dma ecrit tjs en $2004
        } else {
            // Write data to dataBus
            setDataBus(InternalCpu::_dataOutput);
            performWrite();
        }
    }
    
    _dma.clockPhi2();
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::endPhi2() {
    InternalCpu::endPhi2();
    /*// Copy data from dataBus on internal registers
    //if (this->_readWrite == Cpu6502::ReadWrite::Read) {//TODO: par apres voir si pas toujours lu meme en write (tester avec visual) (peut etre plus besoin du setDataBus(this->_inputDataLatch); dans endPhi1 si ca !!! a voir
        InternalCpu::_inputDataLatch = getDataBus();
        InternalCpu::_predecode = InternalCpu::_inputDataLatch;
    //}
    
    // Check reset
    //InternalCpu::template checkReset<InternalCpu::ResetAccurate>();   // TODO: normalement pas besoin car initialisé avec une config performance
    
    // Check interrupts line
    InternalCpu::checkNmi();
    InternalCpu::checkIrq();
    
    // Check ready line
    InternalCpu::checkReady();*/
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::reset(bool high) {
    // If reset
    /*if (!high) {          // TODO: a reflechir pour le reset et le DMA (peut etre un _dma.reset(high);) OUI C'est SUR QU'IL FAUT LE RESET CAR SI ON EXECUTE LES TESTS UN PAR UN C'est ok mais tous en meme temps et ca foire apres le 1er car il n'est pas reset pour les autres tests ! (peut etre pas reset mais PowerUp c'est sur)
        // Stop possible DMA
        stopDma();
    }*/
    
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
Cpu6502::ReadWrite Chip<EModel, TBus, TSoundHardware>::getReadWriteSignal() const {//TODO: voir lequel retourner !! normalement l'externe donc _readWrite
    return _readWrite;//InternalCpu::getReadWriteSignal();
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
        _bus.readControllerPort(0); // TODO: voir si pas meilleure conception que ca sinon renommer TBus en THardware car ca ne fait pas que le bus : il faut un autre template parameter TControllerCircuit ou un nom dans le genre et l'avoir dans la nes et le passer en reference dans le cpu !
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
    // Sprite DMA
    else if (address == 0x4014) {
        _dma.startSprite(data);
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
void Chip<EModel, TBus, TSoundHardware>::apuIrq(bool high) {
    // Save signal
    _apuIrqLine = high;
    
    // Irq must be low if any of its input is low
    InternalCpu::irq(_irqLine && _apuIrqLine);
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::apuDmcRequestSample(uint16_t address, bool requestedOnEnable) {
    // Start DMC DMA
    _dma.startDmc(address, requestedOnEnable);
    
    /*
    uint16_t currentAddress = getAddressBus();
    setAddressBus(address);  //TODO: pas bon, a changer, juste pour tests, il faut faire comme un dma !
    performRead();
    setAddressBus(currentAddress);
    _apu.dmcSampleFetched(getDataBus());*/
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::apuDmcSampleFetched() {
    // Notify APU that a DMC sample is fetched
    _apu.dmcSampleFetched(getDataBus());
}

#endif /* NESEmu_Cpu_s_hpp */
