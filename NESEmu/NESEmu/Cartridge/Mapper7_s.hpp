//
//  Mapper7_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper7_s_hpp
#define NESEmu_Cartridge_Mapper7_s_hpp


template <class TCpuHardwareInterface, class TPpuHardwareInterface>
Chip<TCpuHardwareInterface, TPpuHardwareInterface>::Chip(std::vector<uint8_t> prgRom, std::size_t chrRamSize) : _prgRom(std::move(prgRom)), _chrRam(chrRamSize), _prgRomSize(_prgRom.size()), _chrRamSize(_chrRam.size()), _prgRomBankSelect(0), _vramBankSelect(0) {
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Prg-Rom
    if (address >= 0x8000) {
        // Read Prg-Rom selected bank
        cpuHardwareInterface.setDataBus(_prgRom[((_prgRomBankSelect << 15) | (address & 0x7FFF)) & (_prgRomSize - 1)]);
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Get data
    uint8_t data = cpuHardwareInterface.getDataBus();
    
    // Writing to Prg-Rom select the Prg-rom and VRAM bank
    if (address >= 0x8000) {
        _prgRomBankSelect = data & 0x7;
        _vramBankSelect = data & 0x10;
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Chr-Ram
    if (address < 0x2000) {
        // Read Chr-Ram
        ppuHardwareInterface.setDataBus(_chrRam[address & (_chrRamSize - 1)]);
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Read VRAM with mirrored address
        ppuHardwareInterface.setDataBus(ppuHardwareInterface.getVram()[(_vramBankSelect << 6) | getMirroredAddress<MirroringType::SingleScreen>(address)]);
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
        // Write Chr-Ram
        _chrRam[address & (_chrRamSize - 1)] = data;
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Write VRAM with mirrored address
        ppuHardwareInterface.getVram()[(_vramBankSelect << 6) | getMirroredAddress<MirroringType::SingleScreen>(address)] = data;
    }
}

#endif /* NESEmu_Cartridge_Mapper7_s_hpp */
