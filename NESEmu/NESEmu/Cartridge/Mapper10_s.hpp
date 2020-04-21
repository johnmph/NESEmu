//
//  Mapper10_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 19/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper10_s_hpp
#define NESEmu_Cartridge_Mapper10_s_hpp


template <class TCpuHardwareInterface, class TPpuHardwareInterface>
Chip<TCpuHardwareInterface, TPpuHardwareInterface>::Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> chrRom) : Interface<TCpuHardwareInterface, TPpuHardwareInterface>(std::move(prgRom), 8 * 1024, std::move(chrRom), 0), _prgRomBankSelect(0) {//TODO: tjs 8192 de prg-ram ?
    // TODO: reseter aussi les _bankSelect ?
    
    /*_chrRomLatch[0] = true;
    _chrRomLatch[1] = true;
    _chrRomBankSelect[0] = 0;
    _chrRomBankSelect[1] = 0;
    _chrRomBankSelect[2] = 0;
    _chrRomBankSelect[3] = 0;*/
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram
        if (this->hasPrgRam()) {
            // Read Prg-Ram with possible mirrored address
            cpuHardwareInterface.setDataBus(this->readPrgRam(address));
        }
    }
    // Prg-Rom (first bank)
    else if ((address >= 0x8000) && (address < 0xC000)) {
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
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram
        if (this->hasPrgRam()) {
            // Write Prg-Ram with possible mirrored address
            this->writePrgRam(address, data);
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
        // Check for bank
        bool isSecondBank = (address >> 12) != 0;
        
        // Read Chr-Rom
        ppuHardwareInterface.setDataBus(this->readChrRom((_chrRomBankSelect[(isSecondBank << 1) | _chrRomLatch[isSecondBank]] << 12) | (address & 0xFFF)));
        
        // Check for latchs
        if ((address & 0xFF8) == 0xFD8) {
            _chrRomLatch[isSecondBank] = false;
        }
        else if ((address & 0xFF8) == 0xFE8) {
            _chrRomLatch[isSecondBank] = true;
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

#endif /* NESEmu_Cartridge_Mapper10_s_hpp */
