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
    static constexpr int masterClockSpeedInHz = 21477272;
    static constexpr int cpuMasterClockDivider = 12;
    static constexpr int ppuMasterClockDivider = 4;
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
Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuBus::CpuBus(Nes &nes) : _nes(nes) {
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
uint8_t Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuBus::read(uint16_t address) {
    return _nes.cpuRead(address);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::CpuBus::write(uint16_t address, uint8_t data) {
    _nes.cpuWrite(address, data);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuBus::PpuBus(Nes &nes) : _nes(nes) {
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
uint8_t Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuBus::read(uint16_t address) {
    return _nes.ppuRead(address);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuBus::write(uint16_t address, uint8_t data) {
    _nes.ppuWrite(address, data);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::PpuBus::interrupt(bool high) {
    _nes.ppuInterrupt(high);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
Nes<EModel, TCartridgeHardware, TGraphicHardware>::Nes(TCartridgeHardware &cartridgeHardware, TGraphicHardware &graphicHardware) : _cartridgeHardware(cartridgeHardware), _cpuBus(*this), _ppuBus(*this), _cpu(_cpuBus), _ppu(_ppuBus, *this, graphicHardware), _ram(2 * 1024), _vram(2 * 1024) {
    // Begin with no controller
    connectController(0, std::make_unique<Controller::Nothing>());
    connectController(1, std::make_unique<Controller::Nothing>());
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::clock() {
    ++_currentClockForCpu;
    ++_currentClockForPpu;
    
    // Perform a ppu clock if necessary
    if (_currentClockForPpu >= Constants::ppuMasterClockDivider) {
        _ppu.clock();
        _currentClockForPpu = 0;
    }
    
    // Perform a cpu clock if necessary
    if (_currentClockForCpu >= Constants::cpuMasterClockDivider) {
        _cpu.clock();
        _currentClockForCpu = 0;
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
    
    _controllers[portNumber] = std::move(controller);   // TODO: + attention il faut gérer l'inversion et l'open bus
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
uint8_t Nes<EModel, TCartridgeHardware, TGraphicHardware>::cpuRead(uint16_t address) {   // TODO: a la place de ca, avoir l'emulation de la chip 74139 ?? (ca revient au meme normalement)
    // RAM
    if (address < 0x2000) {
        // RAM is mirrored each 0x800 bytes
        return _ram[address & 0x7FF];
    }
    // PPU
    else if (address < 0x4000) {
        // Only A0 to A2 is passed to the PPU address
        return _ppu.ioRead(address & 0x7);
    }
    // Cartridge
    else if (address >= 0x4020) {   // TODO: voir pour l'adresse et pour le open bus behaviour si par exemple la rom ne contient pas de WRAM !!
        // Read from cartridge
        return _cartridgeHardware.cpuRead(address);  // TODO: a mettre une fois la classe faite, voir si mask ?
    }
    
    // Open data bus latch (reading open bus (no device active address) repeats the last value that was read from the bus before this read)
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    return _cpu.getDataBus();   // TODO: voir si correct, voir si c'est affecté par le write aussi (dans ce cas ce code n'est pas bon), voir avec le ppu
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::cpuWrite(uint16_t address, uint8_t data) {
    // RAM
    if (address < 0x2000) {
        // RAM is mirrored each 0x800 bytes
        _ram[address & 0x7FF] = data;
    }
    // PPU
    else if (address < 0x4000) {
        // Only A0 to A2 is passed to the PPU address
        _ppu.ioWrite(address & 0x7, data);
    }
    // I/O is managed internally by CPU, so it is guaranteed that no 0x4000 - 0x401F address are here
    // Cartridge
    else {
        // Write to cartridge
        _cartridgeHardware.cpuWrite(address, data);  // TODO: a mettre une fois la classe faite, voir si mask ?
    }
    
    // Write does not affect open data bus latch because when CPU read on the open data bus latch, the last operation was a read (for the opcode, operand or address)
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
uint8_t Nes<EModel, TCartridgeHardware, TGraphicHardware>::ppuRead(uint16_t address) {
    // Read from cartridge
    return _cartridgeHardware.ppuRead(address);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::ppuWrite(uint16_t address, uint8_t data) {
    // Write to cartridge
    _cartridgeHardware.ppuWrite(address, data);
}

template <Model EModel, class TCartridgeHardware, class TGraphicHardware>
void Nes<EModel, TCartridgeHardware, TGraphicHardware>::ppuInterrupt(bool high) {   // TODO: avoir une struct interne pour l'interrupt qui a une methode interrupt ? et qui appelle celle ci ? ainsi dans le PPU on appelle interrupt et pas ppuInterrupt (ce qui est plus propre niveau conception)
    // PPU interrupt is connected to CPU NMI
    _cpu.nmi(high);
}

#endif /* NESEmu_Nes_s_hpp */
