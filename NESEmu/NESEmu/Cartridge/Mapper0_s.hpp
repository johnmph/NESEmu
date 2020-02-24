//
//  Mapper0_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper0_s_hpp
#define NESEmu_Cartridge_Mapper0_s_hpp


template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
Mapper0<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::Mapper0(std::istream &istream) : _prgRom(IPrgRomSizeInKb), _prgRam(IPrgRamSizeInKb), _chrRom(8 * 1024) {
    // TODO: voir si read via istream ici ou bien dans un factory a part et avoir directement ici les vectors a copier simplement
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper0<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::cpuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram
        if (IPrgRamSizeInKb > 0) {
            // Read Prg-Ram with possible mirrored address
            connectedBus.setDataBus(_prgRam[address & ((IPrgRamSizeInKb * 1024) - 1)]);
        }
    }
    // Prg-Rom
    else if (address >= 0x8000) {   // TODO: voir si nécessaire la condition (est ce qu'on peut etre en dessous de 0x6000 dans cette methode ?
        // Read Prg-Rom with possible mirrored address
        connectedBus.setDataBus(_prgRom[address & ((IPrgRomSizeInKb * 1024) - 1)]);
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper0<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::cpuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    uint8_t data = connectedBus.getDataBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram
        if (IPrgRamSizeInKb > 0) {
            // Write Prg-Ram with possible mirrored address
            _prgRam[address & ((IPrgRamSizeInKb * 1024) - 1)] = data;
        }
    }
    // Nothing for Prg-Rom (Can't write to a ROM)
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper0<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::ppuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Chr-Rom
    if (address < 0x2000) {//TODO: ca ou mirror ??
        // Read Chr-Rom
        connectedBus.setDataBus(_chrRom[address]);
    }
    // Internal VRAM
    else if (address < 0x4000) {
        // Read VRAM with mirrored address
        connectedBus.setDataBus(connectedBus.getVram()[address & 0xFFF]);   // TODO: attention on doit prendre EMirroring en compte, voir comment
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper0<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::ppuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    uint8_t data = connectedBus.getDataBus();
    
    // Nothing for Chr_rom (Can't write to  ROM)
    // Internal VRAM
    if ((address >= 0x2000) && (address < 0x4000)) {
        // Write VRAM with mirrored address
        connectedBus.getVram()[address & 0xFFF] = data;         // TODO: attention on doit prendre EMirroring en compte, voir comment
    }
}

#endif /* NESEmu_Cartridge_Mapper0_s_hpp */
