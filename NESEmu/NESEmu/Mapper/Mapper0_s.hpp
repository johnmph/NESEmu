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
Chip<EMirroring>::Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> chrRom) : _prgRom(std::move(prgRom)), _chrRom(std::move(chrRom)) {
    // 16 or 32kb of prg-rom only
    assert((_prgRom.size() == (16 * 1024)) || (_prgRom.size() == (32 * 1024)));
    
    // Always 8kb of chr-rom
    assert(_chrRom.size() == (8 * 1024));
    
    // Calculate masks
    _prgRomAddressMask = _prgRom.size() - 1;
    _prgRamAddressMask = _prgRam.size() - 1;
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
    /*if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram
        if (IPrgRamSizeInKb > 0) {
            // Read Prg-Ram with possible mirrored address
            connectedBus.setDataBus(_prgRam[address & ((IPrgRamSizeInKb * 1024) - 1)]);
        }
    }
    // Prg-Rom
    else */if (address >= 0x8000) {   // TODO: voir si nécessaire la condition (est ce qu'on peut etre en dessous de 0x6000 dans cette methode ? : oui nécessaire !!! voir http://forums.nesdev.com/viewtopic.php?f=9&t=14421
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
    /*if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram
        if (IPrgRamSizeInKb > 0) {
            // Write Prg-Ram with possible mirrored address
            _prgRam[address & ((IPrgRamSizeInKb * 1024) - 1)] = data;
        }
    }*/
    // Nothing for Prg-Rom (Can't write to a ROM)
}

template <MirroringType EMirroring>
template <class TConnectedBus>
void Chip<EMirroring>::ppuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Chr-Rom
    if (address < 0x2000) {
        // Read Chr-Rom
        connectedBus.setDataBus(_chrRom[address]);
    }
    // Internal VRAM
    else if (address < 0x4000) {    // TODO: pas besoin de la condition car le mask & 0x3FFF est mis via le PPU
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
    
    //std::cout << std::hex << "address = " << +address << ", data = " << +data << "\n";    // TODO: a retirer
    
    // Nothing for Chr-Rom (Can't write to a ROM)
    if (address < 0x2000) {//TODO: j'ai mis ca pour supporter le chr-ram pour certaines rom tests, voir comment bien l'integrer dans ce mapper (via les template parameters) TODO: quand je met ca, dans donkeykong, le sprite mario deconne (bizarre car il ne devrait pas ecrire dans le chr-rom) : normalement ok maintenant
        // Write Chr-Rom
        _chrRom[address] = data;
    }else
    
    // Internal VRAM
    if ((address >= 0x2000) && (address < 0x4000)) {    // TODO: pas besoin de la condition car le mask & 0x3FFF est mis via le PPU
        // Write VRAM with mirrored address
        connectedBus.getVram()[getMirroredAddress<EMirroring>(address)] = data;
    }
}

#endif /* NESEmu_Mapper_Mapper0_s_hpp */
