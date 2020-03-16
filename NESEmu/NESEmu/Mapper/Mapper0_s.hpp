//
//  Mapper0_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Mapper0_s_hpp
#define NESEmu_Mapper_Mapper0_s_hpp


template <MirroringType EMirroring>
bool NRom<EMirroring>::isPrgRamReadEnabled() const {
    // Always enabled
    return true;
}

template <MirroringType EMirroring>
bool NRom<EMirroring>::isPrgRamWriteEnabled() const {
    // Always enabled
    return true;
}

template <MirroringType EMirroring>
std::size_t NRom<EMirroring>::getPrgRomAddress(uint16_t address, std::size_t size) const {
    return address & (size - 1);
}

template <MirroringType EMirroring>
std::size_t NRom<EMirroring>::getPrgRamAddress(uint16_t address, std::size_t size) const {
    return address & (size - 1);
}

template <MirroringType EMirroring>
std::size_t NRom<EMirroring>::getChrRomAddress(uint16_t address, std::size_t size) const {
    return address & (size - 1);
}

template <MirroringType EMirroring>
std::size_t NRom<EMirroring>::getChrRamAddress(uint16_t address, std::size_t size) const {
    return address & (size - 1);
}

template <MirroringType EMirroring>
uint16_t NRom<EMirroring>::getMirroredVRamAddress(uint16_t address) const {
    return getMirroredAddress<EMirroring>(address);
}



template <MirroringType EMirroring>
Chip<EMirroring>::Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> chrRom) : _prgRom(std::move(prgRom)), _chrRom(std::move(chrRom)), _prgRomAddressMask(_prgRom.size() - 1), _prgRamAddressMask(0), _chrRomOrRamAddressMask(_chrRom.size() - 1), _hasChrRam(false) {
}

template <MirroringType EMirroring>
Chip<EMirroring>::Chip(std::vector<uint8_t> prgRom, std::size_t chrRamSize) : _prgRom(std::move(prgRom)), _chrRam(chrRamSize), _prgRomAddressMask(_prgRom.size() - 1), _prgRamAddressMask(0), _chrRomOrRamAddressMask(_chrRam.size() - 1), _hasChrRam(true) {
}

template <MirroringType EMirroring>
Chip<EMirroring>::Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> prgRam, std::vector<uint8_t> chrRom) : _prgRom(std::move(prgRom)), _prgRam(std::move(prgRam)), _chrRom(std::move(chrRom)), _prgRomAddressMask(_prgRom.size() - 1), _prgRamAddressMask(_prgRam.size() - 1), _chrRomOrRamAddressMask(_chrRom.size() - 1), _hasChrRam(false) {
}

template <MirroringType EMirroring>
Chip<EMirroring>::Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> prgRam, std::size_t chrRamSize) : _prgRom(std::move(prgRom)), _prgRam(std::move(prgRam)), _chrRam(chrRamSize), _prgRomAddressMask(_prgRom.size() - 1), _prgRamAddressMask(_prgRam.size() - 1), _chrRomOrRamAddressMask(_chrRam.size() - 1), _hasChrRam(true) {
}

template <MirroringType EMirroring>
template <class TConnectedBus, class TInterruptHardware>
void Chip<EMirroring>::clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware) {
    // Does nothing
}

template <MirroringType EMirroring>
template <class TConnectedBus>
void Chip<EMirroring>::cpuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram
        if (_prgRamAddressMask > 0) {
            // Read Prg-Ram with possible mirrored address
            connectedBus.setDataBus(_prgRam[address & _prgRamAddressMask]);
        }
    }
    // Prg-Rom
    else if (address >= 0x8000) {   // TODO: voir si nécessaire la condition (est ce qu'on peut etre en dessous de 0x6000 dans cette methode ? : oui nécessaire !!! voir http://forums.nesdev.com/viewtopic.php?f=9&t=14421
        // Read Prg-Rom with possible mirrored address
        connectedBus.setDataBus(_prgRom[address & _prgRomAddressMask]);
    }
}

template <MirroringType EMirroring>
template <class TConnectedBus>
void Chip<EMirroring>::cpuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    uint8_t data = connectedBus.getDataBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram
        if (_prgRamAddressMask > 0) {
            // Write Prg-Ram with possible mirrored address
            _prgRam[address & _prgRamAddressMask] = data;
        }
    }
    // Nothing for Prg-Rom (Can't write to a ROM)
}

template <MirroringType EMirroring>
template <class TConnectedBus>
void Chip<EMirroring>::ppuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Chr-Rom / Ram
    if (address < 0x2000) {
        // Read Chr-Rom / Ram
        connectedBus.setDataBus((_hasChrRam) ? _chrRam[address] : _chrRom[address]);
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Read VRAM with mirrored address
        connectedBus.setDataBus(connectedBus.getVram()[getMirroredAddress<EMirroring>(address)]);
    }
}

template <MirroringType EMirroring>
template <class TConnectedBus>
void Chip<EMirroring>::ppuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    uint8_t data = connectedBus.getDataBus();
    
    // Chr-Ram
    if (address < 0x2000) {
        // Write Chr-Ram if exist
        if (_hasChrRam) {
            _chrRam[address] = data;
        }
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Write VRAM with mirrored address
        connectedBus.getVram()[getMirroredAddress<EMirroring>(address)] = data;
    }
}

#endif /* NESEmu_Mapper_Mapper0_s_hpp */
