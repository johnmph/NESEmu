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


template <Model EModel>
Nes<EModel>::Nes() : _cpu(*this), _ppu(*this, *this, *this), _ram(2 * 1024) {
}

template <Model EModel>
void Nes<EModel>::clock() {
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

template <Model EModel>
void Nes<EModel>::reset(bool high) {  // TODO: a voir et a terminer
    // Reset CPU
    _cpu.reset(high);
    
    // Reset PPU
    _ppu.reset(high);
}

template <Model EModel>
uint8_t Nes<EModel>::read(uint16_t address) {   // TODO: a la place de ca, avoir l'emulation de la chip 74139 ?? (ca revient au meme normalement)
    // RAM
    if (address < 0x2000) {
        // RAM is mirrored each 0x800 bytes
        return _ram[address & 0x7FF];
    }
    // PPU
    else if (address < 0x4000) {
        // Only A0 to A2 is passed to the PPU address
        return _ppu.read(address & 0x7);
    }
    // Cartridge
    else if (address >= 0x4020) {   // TODO: voir pour l'adresse et pour le open bus behaviour si par exemple la rom ne contient pas de WRAM !!
        // Read from cartridge
        //return _cartridge.read(address);  // TODO: a mettre une fois la classe faite, voir si mask ?
    }
    
    // Open data bus latch (reading open bus (no device active address) repeats the last value that was read from the bus before this read)
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    return _cpu.getDataBus();   // TODO: voir si correct, voir c'est affecté par le write aussi (dans ce cas ce code n'est pas bon), voir avec le ppu
}

template <Model EModel>
void Nes<EModel>::write(uint16_t address, uint8_t data) {
    // RAM
    if (address < 0x2000) {
        // RAM is mirrored each 0x800 bytes
        _ram[address & 0x7FF] = data;
    }
    // PPU
    else if (address < 0x4000) {
        // Only A0 to A2 is passed to the PPU address
        _ppu.write(address & 0x7, data);
    }
    // I/O is managed internally by CPU, so it is guaranteed that no 0x4000 - 0x401F address are here
    // Cartridge
    else {
        // Write to cartridge
        //_cartridge.write(address, data);  // TODO: a mettre une fois la classe faite, voir si mask ?
    }
    
    // Write does not affect open data bus latch because when CPU read on the open data bus latch, the last operation was a read (for the opcode, operand or address)
}

template <Model EModel>
void Nes<EModel>::ppuInterrupt(bool high) {
    // PPU interrupt is connected to CPU NMI
    _cpu.nmi(high);
}

#endif /* NESEmu_Nes_s_hpp */
