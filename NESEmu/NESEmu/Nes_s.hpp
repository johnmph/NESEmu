//
//  Nes_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 10/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Nes_s_hpp
#define NESEmu_Nes_s_hpp


template <>
struct Constants<Model::Ntsc> {
    // CPU
    static constexpr Cpu::Model cpuModel = Cpu::Model::Ricoh2A03;
    
    // PPU
    static constexpr Ppu::Model ppuModel = Ppu::Model::Ricoh2C02;
    
    // Clock
    static constexpr int masterClockSpeedInHz = 21477272;// / 4;   // TODO: / 4 pour optimiser la clock principale (gain de +- 14 fps en release)
    static constexpr int cpuMasterClockDivider = 12;// / 4
    static constexpr int ppuMasterClockDivider = 4;// / 4;
};

template <>
struct Constants<Model::Pal> {
    // CPU
    static constexpr Cpu::Model cpuModel = Cpu::Model::Ricoh2A07;
    
    // PPU
    static constexpr Ppu::Model ppuModel = Ppu::Model::Ricoh2C07;
    
    // Clock
    static constexpr int masterClockSpeedInHz = 26601712;
    static constexpr int cpuMasterClockDivider = 16;
    static constexpr int ppuMasterClockDivider = 5;
};


template <Model EModel, class TGraphicHardware, class TSoundHardware>
Nes<EModel, TGraphicHardware, TSoundHardware>::CpuHardwareInterface::CpuHardwareInterface(Nes &nes) : _nes(nes) {
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
uint16_t Nes<EModel, TGraphicHardware, TSoundHardware>::CpuHardwareInterface::getAddressBus() const {
    // Get address
    return _address;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::CpuHardwareInterface::setAddressBus(uint16_t address) {
    // Set address
    _address = address;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
uint8_t Nes<EModel, TGraphicHardware, TSoundHardware>::CpuHardwareInterface::getDataBus() const {
    // Get data with possibly open data bus latch on some or all lines
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    return _data;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::CpuHardwareInterface::setDataBus(uint8_t data) {
    // Set data
    _data = data;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::CpuHardwareInterface::setDataBus(uint8_t data, uint8_t mask) {
    // Set data with possibly open data bus latch on some or all lines
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    _data = (_data & ~mask) | (data & mask);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::CpuHardwareInterface::performRead() {
    // RAM
    if (_address < 0x2000) {
        // RAM is mirrored each 0x800 bytes
        setDataBus(_nes._ram[_address & 0x7FF]);
    }
    // PPU
    else if (_address < 0x4000) {
        // Only A0 to A2 is passed to the PPU address
        _nes._ppu.readPerformed(*this);
    }
    // Cartridge
    //else if (_address >= 0x4020) {   // TODO: est ce qu'un mapper peut gerer les address < 0x4020 non gérée par le CPU ($4000-$4014 or $4018-$4020) ?? voir http://forums.nesdev.com/viewtopic.php?f=9&t=14421 : OUI voir le MMC5 et le comment ci dessous en write
    // Read from cartridge
    _nes._cartridge->cpuReadPerformed(*this);
    //}
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::CpuHardwareInterface::performWrite() {
    // RAM
    if (_address < 0x2000) {
        // RAM is mirrored each 0x800 bytes
        _nes._ram[_address & 0x7FF] = getDataBus();
    }
    // PPU
    else if (_address < 0x4000) {
        // Only A0 to A2 is passed to the PPU address
        _nes._ppu.writePerformed(*this);
    }
    // I/O is managed internally by CPU, so it is guaranteed that no 0x4000 - 0x401F address are here
    // Cartridge
    //else {  // TODO: pas tout a fait vrai, en fait le MMC5 monitor ce qui est lu/ecrit dans le PPU donc surement pas de else ici !!!
    // Write to cartridge
    _nes._cartridge->cpuWritePerformed(*this);
    //}
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::CpuHardwareInterface::readControllerPort(unsigned int number) {
    // Get current data bus
    uint8_t data = getDataBus();
    
    // Invert it before passing it to controller
    data = ~data;
    
    // Clock the controller
    _nes._controllerPorts[number]->clock(data);
    
    // Set data bus with inverted data
    setDataBus(~data);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::CpuHardwareInterface::irq(bool high) {
    _nes.cartridgeInterrupt(high);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
Nes<EModel, TGraphicHardware, TSoundHardware>::PpuHardwareInterface::PpuHardwareInterface(Nes &nes) : _nes(nes) {
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
uint16_t Nes<EModel, TGraphicHardware, TSoundHardware>::PpuHardwareInterface::getAddressBus() const {
    // Get address
    return (_address & 0xFF00) | _externalOctalLatch;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::PpuHardwareInterface::setAddressBus(uint16_t address) {
    // Set address (Only 14 bits)
    _address = address & 0x3FFF;
    
    // Save low byte of addressBus in external octal latch
    _externalOctalLatch = _address & 0xFF;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
uint8_t Nes<EModel, TGraphicHardware, TSoundHardware>::PpuHardwareInterface::getDataBus() const {
    // Get data with possibly open data bus latch on some or all lines
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    return _address & 0xFF;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::PpuHardwareInterface::setDataBus(uint8_t data) {
    // Set data
    _address = (_address & 0xFF00) | data;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::PpuHardwareInterface::setDataBus(uint8_t data, uint8_t mask) {
    // Set data with possibly open data bus latch on some or all lines
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    uint16_t mask16Bits = mask;
    _address = (_address & ~mask16Bits) | (data & mask);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::PpuHardwareInterface::performRead() {
    // Read from cartridge
    _nes._cartridge->ppuReadPerformed(*this);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::PpuHardwareInterface::performWrite() {
    // Write to cartridge
    _nes._cartridge->ppuWritePerformed(*this);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
std::vector<uint8_t> &Nes<EModel, TGraphicHardware, TSoundHardware>::PpuHardwareInterface::getVram() {    // TODO: est ce qu'on peut mettre const (car meme si on modifie vram, ca n'est pas dans cet objet (mais a voir comme c'est un sous-objet))
    // Get vram
    return _nes._vram;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::PpuHardwareInterface::interrupt(bool high) {
    _nes.ppuInterrupt(high);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
Nes<EModel, TGraphicHardware, TSoundHardware>::Nes(TGraphicHardware &graphicHardware, TSoundHardware &soundHardware) : _cpuHardwareInterface(*this), _ppuHardwareInterface(*this), _cpu(_cpuHardwareInterface, soundHardware), _ppu(_ppuHardwareInterface, _ppuHardwareInterface, graphicHardware), _ram(2 * 1024), _vram(2 * 1024), _currentClockForCpu(0), _currentClockForPpu(0), _isCpuPhi2(false) {
    // Begin with no controller
    disconnectController(0);
    disconnectController(1);
    
    // Set sound hardware sampler frequency to cpu frequency (because apu is clocked each time cpu is clocked)
    soundHardware.setSamplerFrequency(Constants::masterClockSpeedInHz / Constants::cpuMasterClockDivider);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::powerUp() {
    // Power up CPU
    _cpu.powerUp();
    
    // Power up PPU
    _ppu.powerUp();
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::reset(bool high) {  // TODO: a voir et a terminer
    // Reset CPU
    _cpu.reset(high);
    
    // Reset PPU
    _ppu.reset(high);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::connectController(unsigned int portNumber, std::unique_ptr<Controller::Interface> controller) {
    assert(portNumber < 2);
    assert(controller != nullptr);
    
    // Connect the controller
    _controllerPorts[portNumber] = std::move(controller);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
std::unique_ptr<Controller::Interface> Nes<EModel, TGraphicHardware, TSoundHardware>::disconnectController(unsigned int portNumber) {
    assert(portNumber < 2);
    
    // Get connected controller
    auto controller = std::move(_controllerPorts[portNumber]);
    
    // Disconnect the controller by setting it with Controller::Nothing
    _controllerPorts[portNumber] = std::make_unique<Controller::Nothing>();
    
    return controller;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::insertCartridge(std::unique_ptr<Cartridge::Interface<CpuHardwareInterface, PpuHardwareInterface>> cartridge) {
    assert(cartridge != nullptr);
    
    _cartridge = std::move(cartridge);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
auto Nes<EModel, TGraphicHardware, TSoundHardware>::removeCartridge() -> std::unique_ptr<Cartridge::Interface<CpuHardwareInterface, PpuHardwareInterface>> {
    // Get inserted cartridge
    auto cartridge = std::move(_cartridge);
    
    // Remove cartridge by setting it with Cartridge::Nothing
    _cartridge = std::make_unique<Cartridge::Nothing<CpuHardwareInterface, PpuHardwareInterface>>();
    
    return cartridge;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
template <class TLoopFunction>
void Nes<EModel, TGraphicHardware, TSoundHardware>::run(TLoopFunction &&loopFunction) {
    // Run loop
    for (;;) {
        // Do a full cycle
        clockFull();
        
        // Check if need to stop
        if (loopFunction(*this)) {
            break;
        }
    }
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::clockFull() {   // TODO: gros gain de FPS ainsi !!!, surement rajouter clock (avec un template parameter pour choisir si PPU ou CPU en 1er s'il tombe en meme temps)
    /*_cpu.clock(false);
     _ppu.clock();
     
     // Update controllers
     for (int i = 0; i < 2; ++i) {
     _controllerPorts[i]->out(_cpu.getOutSignal() & 0x1);        // TODO: voir pour les performances ici
     }
     
     _cartridge->clock(_cpuHardwareInterface, _ppuHardwareInterface);
     _cpu.clockPhi1();
     
     _ppu.clock();
     
     _cpu.clockPhi2();
     
     _ppu.clock();
     */
    
    // Update controllers
    for (int i = 0; i < 2; ++i) {
        _controllerPorts[i]->out(_cpu.getOutSignal() & 0x1);        // TODO: une fois les modifs faite dans le CPU pour le getOutSignal et le 4016 write, je n'aurai plus besoin de clocker ca ici !
    }
    
    // Clock cartridge
    _cartridge->clock(_cpuHardwareInterface, _ppuHardwareInterface);
    
    // Clock cpu phi1
    _cpu.clockPhi1();
    
    // Clock ppu twice
    _ppu.clock();
    _ppu.clock();
    
    // Clock cpu phi2
    _cpu.clockPhi2();
    
    // Clock ppu
    _ppu.clock();
    
    /*
     TODO: cette combinaison resoud le prob de la barre de statut qui tremble dans Simpsons (combiné avec les io direct du PPU plutot que delayé)
     _cpu.startPhi1();
     
     _ppu.clock();
     _ppu.clock();
     
     _cpu.endPhi1();
     
     _ppu.clock();
     
     _cpu.startPhi2();
     
     _cpu.endPhi2();
     
     */
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::clock() {
    // Perform a cpu clock if necessary
    if (_currentClockForCpu <= 0) {
        // If in cpu phi1
        if (!_isCpuPhi2) {
            // Update controllers
            for (int i = 0; i < 2; ++i) {
                _controllerPorts[i]->out(_cpu.getOutSignal() & 0x1);        // TODO: voir pour les performances ici
            }
            
            // Clock cartridge
            _cartridge->clock(_cpuHardwareInterface, _ppuHardwareInterface);
            
            // Clock cpu phi1
            _cpu.clockPhi1();
        } else {
            // Clock cpu phi2
            _cpu.clockPhi2();
        }
        
        // Update state
        _currentClockForCpu = Constants::cpuMasterClockDivider / 2;//TODO: mettre ca dans une constante cpuMasterHalfClockDivider dans Nes
        _isCpuPhi2 = !_isCpuPhi2;
    }
    
    // Perform a ppu clock if necessary
    if (_currentClockForPpu <= 0) {
        // Clock ppu
        _ppu.clock();
        
        // Update state
        _currentClockForPpu = Constants::ppuMasterClockDivider;
    }
    
    // Decrement clock counters
    --_currentClockForCpu;
    --_currentClockForPpu;
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::ppuInterrupt(bool high) {
    // PPU interrupt is connected to CPU NMI
    _cpu.nmi(high);
}

template <Model EModel, class TGraphicHardware, class TSoundHardware>
void Nes<EModel, TGraphicHardware, TSoundHardware>::cartridgeInterrupt(bool high) {
    // Cartridge interrupt is connected to CPU IRQ
    _cpu.irq(high);
}

#endif /* NESEmu_Nes_s_hpp */
