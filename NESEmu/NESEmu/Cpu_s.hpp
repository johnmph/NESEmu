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
Chip<EModel, TBus, TSoundHardware>::Dma::Dma(Chip &chip) : _chip(chip), _spriteAddress(0x0), _dmcAddress(0x0), _spriteCycleCount(0), _dmcCycleCount(0), _waitCycleCount(0), _writeCycle(false), _idle(true) {//TODO: j'ai du mettre writeCycle a false, pq ???? (car il est switché avant le process) alors pq??? : 7 cycles dans le reset / 9 dans le power up, un nombre impair donc on commence en write cycle
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::Dma::clock() {
    // Toggle read / write cycle before process so we have the same type cycle in process method and CPU instruction execution (which is right after this clock method)
    _writeCycle = !_writeCycle;
    
    // Process
    process();
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::Dma::startSprite(uint8_t address) {
    // Calculate address
    _spriteAddress = address << 8;
    
    // Set count
    _waitCycleCount = 1 + _writeCycle;
    _spriteCycleCount = 513;    // TODO: besoin d'un cycle en plus pour la restauration du CPU apres le DMA (ce cycle executera l'instruction du CPU)
    
    // Disable CPU
    _chip.ready(false);
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::Dma::startDmc(uint16_t address) {
    // Ensure that DMC DMA start on an even (read) cycle
    assert(_writeCycle);//TODO: write cycle car apu clocké apres le dma et donc process en even deja effectué si on a !_writeCycle
    
    // Don't start DMC if already started
    if (_dmcCycleCount > 0) {
        return;
    }
    
    // Copy address
    _dmcAddress = address;
    
    // Set count
    _waitCycleCount = (_spriteCycleCount > 0) ? 0 : 2;//((_chip._readWrite == Cpu6502::ReadWrite::Write) ? 1 : 2);//TODO: changer, depend de plusieurs trucs, c'est 4 cycle en read et 3 en write (mais on parle de CPU write cycle et pas de write cycle d'ici), a voir car on est tjs avec _writeCycle = false ici TODO: si je met 4 je ne passe plus le test apu_test pour le dmc basic car l'interrupt quand dmc filled est trop tard : AVEC la nouvelle technique si je met 4 c'est le dmc rate que je ne passe plus
    _dmcCycleCount = 2;
    
    // Disable CPU
    _chip.ready(false);
    
    /*if (_spriteCycleCount > 0) {
        std::cout << std::hex << "DMC in SPR begin : " << +((_chip._addressBusHigh << 8) | _chip._addressBusLow) << ", " << +_chip.getAddressBus() << ", " << +_chip.getDataBus() <<  "\n";
    }*/
}

template <Model EModel, class TBus, class TSoundHardware>
bool Chip<EModel, TBus, TSoundHardware>::Dma::isWriteCycle() const { // TODO: retirer si pas besoin mais normalement l'apu en a besoin je pense!
    return _writeCycle;
}

template <Model EModel, class TBus, class TSoundHardware>
bool Chip<EModel, TBus, TSoundHardware>::Dma::isIdle() const {
    //return ((_dmcCycleCount == 0) && (_spriteCycleCount == 0)) || (_waitCycleCount > 0);//TODO: ne fonctionne pas car waitCycleCount est decrementé avant d'etre checké !
    return _idle;
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::Dma::process() {
    // If need to wait
    if (_waitCycleCount > 0) {
        // Decrement counter
        --_waitCycleCount;
        
        // Exit
        return;
    }
    
    // If DMC DMA is started
    if (_dmcCycleCount > 0) {
        // Check that we are really on correct cycle
        assert((_dmcCycleCount & 0x1) == _writeCycle);
        
        // Set idle flag
        _idle = false;
        
        // Decrement counter
        --_dmcCycleCount;
        
        // Read cycle
        if (!_writeCycle) {
            // Read cycle (only inputDataLatch / predecode / RW from 6502 are affected)
            _chip.setAddressBus(_dmcAddress);
            _chip._readWrite = Cpu6502::ReadWrite::Read;
        }
        // Write cycle
        else {
            // Notify APU that sample is fetched
            _chip.apuDmcSampleFetched();
            
            // Restore CPU state (no need to restore read/write signal because last cycle was a read)
            _chip.setAddressBus((_chip._addressBusHigh << 8) | _chip._addressBusLow);
            
            // If no sprite DMA pending
            if (_spriteCycleCount == 0) {
                // Reenable CPU
                _chip.ready(true);
                
                // Set idle flag
                _idle = true;
            }/* else {
                std::cout << std::hex << "DMC in SPR end : " << +((_chip._addressBusHigh << 8) | _chip._addressBusLow) << ", " << +_chip.getAddressBus() << ", " << +_chip.getDataBus() <<  "\n";
            }*/
        }
        
        // Exit
        return;
    }
    
    // If Sprite DMA is started
    if (_spriteCycleCount > 0) {
        // Check that we are really on correct cycle
        assert((_spriteCycleCount & 0x1) == (!_writeCycle));
        
        // Set idle flag
        _idle = false;
        
        // Decrement counter
        --_spriteCycleCount;
        
        // Read cycle
        if (!_writeCycle) {
            // If last cycle
            if (_spriteCycleCount == 0) {
                // Restore CPU state
                _chip.setAddressBus((_chip._addressBusHigh << 8) | _chip._addressBusLow);
                _chip._readWrite = Cpu6502::ReadWrite::Read;
                
                // Reenable CPU
                _chip.ready(true);
                
                // Set idle flag
                _idle = true;
            } else {
                // Read cycle (only inputDataLatch / predecode / RW from 6502 is affected)
                _chip.setAddressBus(_spriteAddress);
                _chip._readWrite = Cpu6502::ReadWrite::Read;
                
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
Chip<EModel, TBus, TSoundHardware>::Chip(TBus &bus, TSoundHardware &soundHardware) : _apu(*this, soundHardware), _bus(bus), _dma(*this), InternalCpu(*this)/*, _dmaStarted(false)*/ {
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
    this->_phi2 = false;
    
    // Update PC
    InternalCpu::updateProgramCounter();
    
    // Initialize dataOutput to emulate possible bus conflict which cause a low level to win (it is like an AND operation)
    //_dataOutput = 0xFF;
    
    // Check for DMA
    //bool needToForceExecute = checkDmaPhi1();
    _dma.clock();
    
    // If rdy is low, wait before perform next read cycle (or write cycle if sync) unless we force execute
    if ((!this->_readyWaitRequested) || ((this->_readWrite != Cpu6502::ReadWrite::Read) && (_dma.isIdle()))) {
        //if ((!this->_readyWaitRequested) || ((!this->_sync) && (this->_readWrite != Cpu6502::ReadWrite::Read)) || needToForceExecute) {//TODO: peut etre a la place de ca, appeler simplement l'instruction mais a chaque fois qu'on doit passer a l'instruction d'apres en settant _currentInstruction on le fait via une methode setNextInstruction qui va checker si readWaitRequested et pas write et si oui ne pas setter la next instruction mais voir si executer plusieurs fois l'instruction en cours ne pose pas de probleme (si un add ou autre est ce que ca va faire plusieurs add a la suite car dans ce cas ce n'est pas bon) : tout ceci pour avoir les operations qui s'execute en parallele d'un fetch ou d'un decode opcode (alu) : TODO: peut etre qu'on doit avoir une variable executionAllowed et cette variable est a true et si readyWaitRequested = true, elle reste a true tant que l'isntruction en cours est un write, une fois passé en read elle passe a false et reste a false jusqu'a ce que readyWaitRequested repasse a false (ce qui veut dire que si apres un read il y a un write (comme pendant le dma) ca n'autorise quand meme pas le cpu a refonctionner tant que rdy n'est pas high), voir si ainsi dans la réalité et si oui l'implementer (ca permettra de ne plus avoir besoin de Dma::isIdle) !
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
    
    // Fetch memory for phi2
    if (this->_readWrite == Cpu6502::ReadWrite::Write) {
        // Check for DMA
        if (!_dma.isIdle()) {
            _bus.performWrite();//TODO: optimisation en mettant _bus devant pour eviter le decodage de l'adresse ici car le dma ecrit tjs en $2004
        } else {
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
    /*if (!high) {          // TODO: a reflechir pour le reset et le DMA (peut etre un _dma.reset(high);)
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
        //startDma(data);
        // Start Sprite DMA
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
/*
template <Model EModel, class TBus, class TSoundHardware>
bool Chip<EModel, TBus, TSoundHardware>::checkDmaPhi1() {
    // If DMC started
    if (_dmcStarted) {
        // Decrement DMC counter
        --_dmcCount;
        
        if (_dmcCount == 1) {//TODO: il faut tjs commencer sur le meme cycle qu'un read spr dma si spr dma en execution (et s'il n'a pas attendu, laisser un autre cycle avec un read sur le address bus courant sinon continuer le spr dma le cycle d'apres) : est ce que c'est possible que le dmc dma commence quand le spr dma est en write ? car le dmc dma commence sur un apu cycle je pense, a verifier et voir aussi si un spr dma peut etre en write sur un cycle pair et sur un cycle impair ? peut etre c'est a cause de ca qu'il y a un extra cycle en impair (ou pair je ne sais plus), pour synchroniser avec l'apu dmc dma !!!
            // TODO: pas besoin de sauver et de restaurer par apres le readwrite car meme s'il peut etre en write (fin d'interruption push stack), comme il attend suffisamment longtemps, l'instruction d'apres fait tjs un read !
            _bus.setAddressBus(_dmcSampleAddress);
            this->_readWrite = Cpu6502::ReadWrite::Read;
        }
        else if (_dmcCount == 0) {
            // Notify APU that sample is fetched
            _apu.dmcSampleFetched(_bus.getDataBus());
            
            // Restore CPU state
            _bus.setAddressBus((this->_addressBusHigh << 8) | this->_addressBusLow);
            /*_bus.setAddressBus(_temp);
            if (_temp != ((this->_addressBusHigh << 8) | this->_addressBusLow)) {
                std::cout << "different : " << +_temp << ", " << +((this->_addressBusHigh << 8) | this->_addressBusLow) << "\n";
                std::cout << "spr dma address : " << ((_dmaAddress << 8) | (256 - _dmaCount)) << "\n";
            }/
            
            // Reenable CPU if necessary
            if (!_dmaStarted) {
                InternalCpu::ready(true);
            }
            
            // Reset flag
            _dmcStarted = false;
        }
        
        if ((_dmcCount > 0) && (this->_readWrite == Cpu6502::ReadWrite::Read)) return false;    // TODO: a voir
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
    if (!InternalCpu::getSyncSignal()) {//TOD: je ne pense pas que c'est bon de faire ca !!!
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
}*/
/*
template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::startDma(uint8_t address) {
    // Set address
    _dmaAddress = address;
    
    // 257 for 1 extra cycle + 256 read-write cycles
    _dmaCount = 257; //TODO: apparemment, ca attend 513 ou 514 cycles selon que c'est demandé sur un cycle pair ou impair !!! ici j'attend 513 cycles a chaque fois !
    
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
}*/

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::apuIrq(bool high) {
    // Save signal
    _apuIrqLine = high;
    
    // Irq must be low if any of its input is low
    InternalCpu::irq(_irqLine && _apuIrqLine);
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::apuDmcRequestSample(uint16_t address) {
    // Start DMC DMA
    _dma.startDmc(address);
    
/*    if (_dmcStarted) {
        return;
    }
    // Save address
    _dmcSampleAddress = address;
    
    // 4 cycles delay
    //_dmcCount = (_dmaStarted) ? 2 : 4;
    _dmcCount = (this->_readWrite == Cpu6502::ReadWrite::Write) ? 4 : 5;
    if (_dmaStarted) {
        std::cout << "dmc dma in spr dma\n";
        _dmcCount = 3;
        if (_dmaCount == 2) _dmcCount = 2;
        if (_dmaCount == 1) _dmcCount = 4;
    }
    
    // Set DMC flag
    _dmcStarted = true;
    
    // Disable CPU
    InternalCpu::ready(false);*/
    
    /*
    uint16_t currentAddress = getAddressBus();
    setAddressBus(address);  //TODO: pas bon, a changer, juste pour tests, il faut faire comme un dma !
    performRead();
    setAddressBus(currentAddress);
    _apu.dmcSampleFetched(getDataBus());*/
}

template <Model EModel, class TBus, class TSoundHardware>
void Chip<EModel, TBus, TSoundHardware>::apuDmcSampleFetched() {
    _apu.dmcSampleFetched(getDataBus());
}

#endif /* NESEmu_Cpu_s_hpp */
