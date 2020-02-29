//
//  Nes_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 21/01/20.
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
    static constexpr int masterClockSpeedInHz = 21477272 / 4;   // TODO: / 4 pour optimiser la clock principale (gain de +- 14 fps en release)
    static constexpr int cpuMasterClockDivider = 12 / 4;//6
    static constexpr int ppuMasterClockDivider = 4 / 4;
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


template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuHardwareInterface::CpuHardwareInterface(Nes &nes) : _nes(nes) {
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
uint16_t Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuHardwareInterface::getAddressBus() const {
    // Get address
    return _address;
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuHardwareInterface::setAddressBus(uint16_t address) {
    // Set address
    _address = address;
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
uint8_t Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuHardwareInterface::getDataBus() const {
    // Get data with possibly open data bus latch on some or all lines
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    return _data;
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuHardwareInterface::setDataBus(uint8_t data) {
    // Set data
    _data = data;
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuHardwareInterface::setDataBus(uint8_t data, uint8_t mask) {
    // Set data with possibly open data bus latch on some or all lines
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    _data = (_data & ~mask) | (data & mask);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuHardwareInterface::performRead() {
    // RAM
    if (_address < 0x2000) {
        // RAM is mirrored each 0x800 bytes
        _data = _nes._ram[_address & 0x7FF];
    }
    // PPU
    else if (_address < 0x4000) {
        // Only A0 to A2 is passed to the PPU address
        _nes._ppu.readPerformed(*this);
    }
    // Cartridge
    else if (_address >= 0x4020) {   // TODO: est ce qu'un mapper peut gerer les address < 0x4020 non gérée par le CPU ($4000-$4014 or $4018-$4020) ??
        // Read from cartridge
        _nes._cartridgeHardware.cpuReadPerformed(*this);
    }
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuHardwareInterface::performWrite() {
    // RAM
    if (_address < 0x2000) {
        // RAM is mirrored each 0x800 bytes
        _nes._ram[_address & 0x7FF] = _data;
    }
    // PPU
    else if (_address < 0x4000) {
        // Only A0 to A2 is passed to the PPU address
        _nes._ppu.writePerformed(*this);
    }
    // I/O is managed internally by CPU, so it is guaranteed that no 0x4000 - 0x401F address are here
    // Cartridge
    else {
        // Write to cartridge
        _nes._cartridgeHardware.cpuWritePerformed(*this);
    }
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuHardwareInterface::readControllerPort(unsigned int number) {
    // Get current data bus
    uint8_t data = getDataBus();
    
    // Invert it before passing it to controller
    data = ~data;
    
    // Clock the controller
    _nes._controllerPorts[number]->clock(data);
    
    // Set data bus with inverted data
    setDataBus(~data);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuHardwareInterface::irq(bool high) {
    //_nes.mapperInterrupt(high);//TODO: ca ou directement _nes._cpu.irq(high); ?
    _nes._cpu.irq(high);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuHardwareInterface::PpuHardwareInterface(Nes &nes) : _nes(nes) {
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
uint16_t Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuHardwareInterface::getAddressBus() const {
    // Get address
    return (_address & 0xFF00) | _externalOctalLatch;
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuHardwareInterface::setAddressBus(uint16_t address) {
    // Set address (Only 14 bits)
    _address = address & 0x3FFF;
    
    // Save low byte of addressBus in external octal latch
    _externalOctalLatch = _address & 0xFF;
    
    // Notify cartridge hardware that address has changed
    _nes._cartridgeHardware.ppuAddressBusChanged(*this);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
uint8_t Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuHardwareInterface::getDataBus() const {
    // Get data with possibly open data bus latch on some or all lines
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    return _address & 0xFF;
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuHardwareInterface::setDataBus(uint8_t data) {
    // Set data
    _address = (_address & 0xFF00) | data;
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuHardwareInterface::setDataBus(uint8_t data, uint8_t mask) {
    // Set data with possibly open data bus latch on some or all lines
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    uint16_t mask16Bits = mask;
    _address = (_address & ~mask16Bits) | (data & mask);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuHardwareInterface::performRead() {
    // Read from cartridge
    _nes._cartridgeHardware.ppuReadPerformed(*this);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuHardwareInterface::performWrite() {
    // Write to cartridge
    _nes._cartridgeHardware.ppuWritePerformed(*this);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
std::vector<uint8_t> &Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuHardwareInterface::getVram() {    // TODO: est ce qu'on peut mettre const (car meme si on modifie vram, ca n'est pas dans cet objet (mais a voir comme c'est un sous-objet))
    // Get vram
    return _nes._vram;
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuHardwareInterface::interrupt(bool high) {
    _nes.ppuInterrupt(high);//TODO: ca ou directement _nes._cpu.nmi(high); ?
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuHardwareInterface::irq(bool high) {
    //_nes.mapperInterrupt(high);//TODO: ca ou directement _nes._cpu.irq(high); ?
    _nes._cpu.irq(high);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
Nes<EModel, TCartridgeHardware, TGraphicHardware>::Nes(TCartridgeHardware &cartridgeHardware, TGraphicHardware &graphicHardware) : _cartridgeHardware(cartridgeHardware), _cpuHardwareInterface(*this), _ppuHardwareInterface(*this), _cpu(_cpuHardwareInterface), _ppu(_ppuHardwareInterface, _ppuHardwareInterface, graphicHardware), _ram(2 * 1024), _vram(2 * 1024), _currentClockForCpu(0), _currentClockForPpu(0) {
    // Begin with no controller
    connectController(0, std::make_unique<Controller::Nothing>());
    connectController(1, std::make_unique<Controller::Nothing>());
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::powerUp() {
    // Power up CPU
    _cpu.powerUp();
    
    // Power up PPU
    _ppu.powerUp();
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::clock() {
    //static bool f = false;
    static int cpuCycle = 0;
    
    ++_currentClockForCpu;
    ++_currentClockForPpu;
    
    // Perform a ppu clock if necessary
    if (_currentClockForPpu >= Constants::ppuMasterClockDivider) {
        _ppu.clock();
        _currentClockForPpu = 0;
    }
    
    // Perform a cpu clock if necessary
    if (_currentClockForCpu >= Constants::cpuMasterClockDivider) {/*
        if (!f) {
            // Update controllers
            for (int i = 0; i < 2; ++i) {
                _controllerPorts[i]->out(_cpu.getOutSignal() & 0x1);        // TODO: voir pour les performances ici
            }
            
            _cpu.clockPhi1();
        } else {
            _cpu.clockPhi2();
        }
        _currentClockForCpu = 0;
        f = !f;*/
        
        // Update controllers
        for (int i = 0; i < 2; ++i) {
            _controllerPorts[i]->out(_cpu.getOutSignal() & 0x1);        // TODO: voir pour les performances ici
        }
        
        _cpu.clock();
        _currentClockForCpu = 0;
        ++cpuCycle;
        
        if (cpuCycle == 29658) {
            int x = 0;
        }
    }
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::reset(bool high) {  // TODO: a voir et a terminer
    // Reset CPU
    _cpu.reset(high);
    
    // Reset PPU
    _ppu.reset(high);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::connectController(unsigned int portNumber, std::unique_ptr<Controller::Interface> controller) {
    assert(portNumber < 2);
    assert(controller != nullptr);
    
    // Connect the controller
    _controllerPorts[portNumber] = std::move(controller);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::ppuInterrupt(bool high) {
    // PPU interrupt is connected to CPU NMI
    _cpu.nmi(high);
}

#endif /* NESEmu_Nes_s_hpp */
