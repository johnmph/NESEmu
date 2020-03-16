//
//  Mapper2_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Mapper2_s_hpp
#define NESEmu_Mapper_Mapper2_s_hpp


template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
Mapper2<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::Mapper2(std::istream &istream) : _prgRom(IPrgRomSizeInKb * 1024), _prgRam(IPrgRamSizeInKb * 1024), _chrRom(8 * 1024), _prgRomBankSelect(0) {
    // TODO: voir si read via istream ici ou bien dans un factory a part et avoir directement ici les vectors a copier simplement : doit etre en dehors
    // TODO: pour tests :
    // Skip header
    istream.seekg(0x10);
    
    // Read Prg-Rom
    istream.read(reinterpret_cast<char *>(_prgRom.data()), IPrgRomSizeInKb * 1024);
    
    // Read Chr-Rom
    istream.read(reinterpret_cast<char *>(_chrRom.data()), 8 * 1024);
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus, class TInterruptHardware>
void Mapper2<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware) {
    // Does nothing
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper2<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::cpuReadPerformed(TConnectedBus &connectedBus) {
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
    // Prg-Rom (first bank)
    else if ((address >= 0x8000) && (address < 0xC000)) {   // TODO: voir si nécessaire la condition (est ce qu'on peut etre en dessous de 0x6000 dans cette methode ? : oui nécessaire !!!
        // Read Prg-Rom selected bank
        connectedBus.setDataBus(_prgRom[(_prgRomBankSelect << 14) | (address & 0x3FFF)]);
    }
    // Prg-Rom (last bank)
    else if (address >= 0xC000) {
        // Read last Prg-Rom bank
        connectedBus.setDataBus(_prgRom[((IPrgRomSizeInKb - 16) * 1024) | (address & 0x3FFF)]);
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper2<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::cpuWritePerformed(TConnectedBus &connectedBus) {
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
    // Writing to Prg-Rom select the Prg-rom bank
    else if (address >= 0x8000) {
        _prgRomBankSelect = data & 0x7;
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper2<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::ppuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Chr-Rom
    if (address < 0x2000) {
        // Read Chr-Rom
        connectedBus.setDataBus(_chrRom[address]);
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Read VRAM with mirrored address
        connectedBus.setDataBus(connectedBus.getVram()[getMirroredAddress<EMirroring>(address)]);
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper2<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::ppuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    uint8_t data = connectedBus.getDataBus();
    
    // Nothing for Chr-Rom (Can't write to a ROM)
    if (address < 0x2000) {//TODO: j'ai mis ca pour supporter le chr-ram pour certaines rom tests, voir comment bien l'integrer dans ce mapper (via les template parameters) TODO: quand je met ca, dans donkeykong, le sprite mario deconne (bizarre car il ne devrait pas ecrire dans le chr-rom) : normalement ok maintenant
     // Write Chr-Rom
     _chrRom[address] = data;
     }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Write VRAM with mirrored address
        connectedBus.getVram()[getMirroredAddress<EMirroring>(address)] = data;
    }
}

#endif /* NESEmu_Mapper_Mapper2_s_hpp */
