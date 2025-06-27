//
//  Mapper72_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 27/06/25.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef SOURCE_NESEMU_CARTRIDGE_MAPPER72_S_HPP
#define SOURCE_NESEMU_CARTRIDGE_MAPPER72_S_HPP


template <class TCpuHardwareInterface, class TPpuHardwareInterface>
Chip<TCpuHardwareInterface, TPpuHardwareInterface>::Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> chrRom, MirroringType mirroringType) : Interface<TCpuHardwareInterface, TPpuHardwareInterface>(std::move(prgRom), 0, std::move(chrRom), 0), _mirroringType(mirroringType), _prgRomBankSelect(0), _chrRomBankSelect(0), _latch(0) {
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Prg-Rom (first bank)
    if ((address >= 0x8000) && (address < 0xC000)) {
        // Read Prg-Rom selected bank
        cpuHardwareInterface.setDataBus(this->readPrgRom((_prgRomBankSelect << 14) | (address & 0x3FFF)));
    }
    // Prg-Rom (last bank)
    else if (address >= 0xC000) {
        // Read last Prg-Rom bank
        cpuHardwareInterface.setDataBus(this->readPrgRom((this->getPrgRomSize() - (16 * 1024)) | (address & 0x3FFF)));
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Get data
    uint8_t data = cpuHardwareInterface.getDataBus();
    
    // Latch write
    if (address >= 0x8000) {
        // Prg-rom bank select
        if (((_latch & 0x80) == 0x0) && ((data & 0x80) != 0x0)) {
            _prgRomBankSelect = data & 0x7;
        }

        // Chr-rom bank select
        if (((_latch & 0x40) == 0x0) && ((data & 0x40) != 0x0)) {
            _chrRomBankSelect = data & 0xF;
        }

        // Audio playback control
        /*if (false) {
            // TODO: a terminer
        }*/

        // Save to latch
        _latch = data;
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Chr-Rom
    if (address < 0x2000) {
        // Read Chr-Rom
        ppuHardwareInterface.setDataBus(this->readChrRom((_chrRomBankSelect << 13) | address));
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Read VRAM with mirrored address
        ppuHardwareInterface.setDataBus(ppuHardwareInterface.getVram()[getVramAddress(address)]);
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Get data
    uint8_t data = ppuHardwareInterface.getDataBus();
    
    // Nothing for Chr-Rom (Can't write to a ROM)
    // Internal VRAM (PPU address is always < 0x4000)
    if (address >= 0x2000) {
        // Write VRAM with mirrored address
        ppuHardwareInterface.getVram()[getVramAddress(address)] = data;
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
uint16_t Chip<TCpuHardwareInterface, TPpuHardwareInterface>::getVramAddress(uint16_t address) {
    // Only Vertical or Horizontal mirroring
    if (_mirroringType == MirroringType::Horizontal) {
        return getMirroredAddress<MirroringType::Horizontal>(address);
    }
    
    return getMirroredAddress<MirroringType::Vertical>(address);
}

#endif /* SOURCE_NESEMU_CARTRIDGE_MAPPER72_S_HPP */
