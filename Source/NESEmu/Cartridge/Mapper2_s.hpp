//
//  Mapper2_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper2_s_hpp
#define NESEmu_Cartridge_Mapper2_s_hpp


template <class TCpuHardwareInterface, class TPpuHardwareInterface>
Chip<TCpuHardwareInterface, TPpuHardwareInterface>::Chip(std::vector<uint8_t> prgRom, std::size_t prgRamSize, std::vector<uint8_t> chrRom, std::size_t chrRamSize, MirroringType mirroringType) : Interface<TCpuHardwareInterface, TPpuHardwareInterface>(std::move(prgRom), prgRamSize, std::move(chrRom), chrRamSize), _mirroringType(mirroringType), _prgRomBankSelect(0) {
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
    // Writing to Prg-Rom select the Prg-rom bank
    else if (address >= 0x8000) {
        _prgRomBankSelect = data;// & 0x7;//TODO: pour alwas awakening
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Chr-Rom / Ram
    if (address < 0x2000) {
        // Read Chr-Rom / Ram
        ppuHardwareInterface.setDataBus((this->hasChrRam()) ? this->readChrRam(address) : this->readChrRom(address));
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
    
    // Chr-Ram
    if (address < 0x2000) {
        // Write Chr-Ram if exist
        if (this->hasChrRam()) {
            this->writeChrRam(address, data);
        }
     }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
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

#endif /* NESEmu_Cartridge_Mapper2_s_hpp */
