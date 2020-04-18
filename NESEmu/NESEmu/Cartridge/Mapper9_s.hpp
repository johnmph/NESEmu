//
//  Mapper9_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 18/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper9_s_hpp
#define NESEmu_Cartridge_Mapper9_s_hpp


template <class TCpuHardwareInterface, class TPpuHardwareInterface>
Chip<TCpuHardwareInterface, TPpuHardwareInterface>::Chip(std::vector<uint8_t> prgRom, std::size_t prgRamSize, std::vector<uint8_t> chrRom) : Interface<TCpuHardwareInterface, TPpuHardwareInterface>(std::move(prgRom), prgRamSize, std::move(chrRom), 0), _prgRomBankSelect(0) {
    // TODO: reseter aussi les _bankSelect ?
    
    //_chrRomLatch[0] = true;
    //_chrRomLatch[1] = true;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram
        if (this->_prgRamSize > 0) {
            // Read Prg-Ram with possible mirrored address
            cpuHardwareInterface.setDataBus(this->_prgRam[address & (this->_prgRamSize - 1)]);
        }
    }
    // Prg-Rom (first bank)
    else if ((address >= 0x8000) && (address < 0xA000)) {
        // Read Prg-Rom selected bank
        cpuHardwareInterface.setDataBus(this->_prgRom[((_prgRomBankSelect << 13) | (address & 0x1FFF)) & (this->_prgRomSize - 1)]);
    }
    // Prg-Rom (3 last banks)
    else if (address >= 0xA000) {
        // Read 3 last Prg-Rom banks
        cpuHardwareInterface.setDataBus(this->_prgRom[(this->_prgRomSize - (24 * 1024)) + (address - 0xA000)]);
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Get data
    uint8_t data = cpuHardwareInterface.getDataBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram
        if (this->_prgRamSize > 0) {
            // Write Prg-Ram with possible mirrored address
            this->_prgRam[address & (this->_prgRamSize - 1)] = data;
        }
    }
    // Prg ROM Bank select
    else if ((address >= 0xA000) && (address < 0xB000)) {
        _prgRomBankSelect = data & 0xF;
    }
    // Chr ROM Bank select
    else if ((address >= 0xB000) && (address < 0xF000)) {
        _chrRomBankSelect[(address - 0xB000) >> 12] = data & 0x1F;
    }
    // Mirroring
    else if (address >= 0xF000) {
        _mirroringType = ((data & 0x1) != 0) ? MirroringType::Horizontal : MirroringType::Vertical;
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Chr-Rom
    if (address < 0x2000) {
        // Read Chr-Rom
        ppuHardwareInterface.setDataBus(this->_chrRom[((_chrRomBankSelect[((address >> 11) & 0x2) | _chrRomLatch[address >> 12]] << 12) | (address & 0xFFF)) & (this->_chrRomSize - 1)]);
        
        // Check for latch 0
        if (address == 0x0FD8) {
            _chrRomLatch[0] = false;
        }
        else if (address == 0x0FE8) {
            _chrRomLatch[0] = true;
        }
        // Check for latch 1
        else if ((address & 0x1FF8) == 0x1FD8) {
            _chrRomLatch[1] = false;
        }
        else if ((address & 0x1FF8) == 0x1FE8) {
            _chrRomLatch[1] = true;
        }
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

#endif /* NESEmu_Cartridge_Mapper9_s_hpp */
