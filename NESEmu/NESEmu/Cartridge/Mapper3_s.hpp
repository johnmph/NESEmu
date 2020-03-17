//
//  Mapper3_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper3_s_hpp
#define NESEmu_Cartridge_Mapper3_s_hpp

// TODO: apparemment pas de prg-ram
template <class TCpuHardwareInterface, class TPpuHardwareInterface>
Chip<TCpuHardwareInterface, TPpuHardwareInterface>::Chip(std::vector<uint8_t> prgRom, std::size_t prgRamSize, std::vector<uint8_t> chrRom, MirroringType mirroringType) : Interface<TCpuHardwareInterface, TPpuHardwareInterface>(std::move(prgRom), prgRamSize, std::move(chrRom), 0), _mirroringType(mirroringType), _chrRomBankSelect(0) {
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
    // Prg-Rom
    else if (address >= 0x8000) {
        // Read Prg-Rom with possible mirrored address
        cpuHardwareInterface.setDataBus(this->_prgRom[address & (this->_prgRomSize - 1)]);
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
    // Writing to Prg-Rom select the Chr-rom bank
    else if (address >= 0x8000) {
        _chrRomBankSelect = data & 0x3;
        
        // TODO: rajouter un assert pour etre sur que le bank select ne depasse pas IChrRomSizeInKb
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Chr-Rom
    if (address < 0x2000) {
        // Read Chr-Rom
        ppuHardwareInterface.setDataBus(this->_chrRom[((_chrRomBankSelect << 13) | address) & (this->_chrRomSize - 1)]);
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

#endif /* NESEmu_Cartridge_Mapper3_s_hpp */
