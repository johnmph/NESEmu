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
template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, unsigned int IChrRomSizeInKb, MirroringType EMirroring>
Mapper3<IPrgRomSizeInKb, IPrgRamSizeInKb, IChrRomSizeInKb, EMirroring>::Mapper3(std::istream &istream) : _prgRom(IPrgRomSizeInKb * 1024), _prgRam(IPrgRamSizeInKb * 1024), _chrRom(IChrRomSizeInKb * 1024), _chrRomBankSelect(0) {
    // TODO: voir si read via istream ici ou bien dans un factory a part et avoir directement ici les vectors a copier simplement : doit etre en dehors
    // TODO: pour tests :
    // Skip header
    istream.seekg(0x10);
    
    // Read Prg-Rom
    istream.read(reinterpret_cast<char *>(_prgRom.data()), IPrgRomSizeInKb * 1024);
    
    // Read Chr-Rom
    istream.read(reinterpret_cast<char *>(_chrRom.data()), IChrRomSizeInKb * 1024);
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, unsigned int IChrRomSizeInKb, MirroringType EMirroring>
template <class TConnectedBus, class TInterruptHardware>
void Mapper3<IPrgRomSizeInKb, IPrgRamSizeInKb, IChrRomSizeInKb, EMirroring>::clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware) {
    // Does nothing
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, unsigned int IChrRomSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper3<IPrgRomSizeInKb, IPrgRamSizeInKb, IChrRomSizeInKb, EMirroring>::cpuReadPerformed(TConnectedBus &connectedBus) {
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
    else if (address >= 0x8000) {   // TODO: voir si nécessaire la condition (est ce qu'on peut etre en dessous de 0x6000 dans cette methode ? : oui nécessaire !!!
        // Read Prg-Rom with possible mirrored address
        connectedBus.setDataBus(_prgRom[address & ((IPrgRomSizeInKb * 1024) - 1)]);
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, unsigned int IChrRomSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper3<IPrgRomSizeInKb, IPrgRamSizeInKb, IChrRomSizeInKb, EMirroring>::cpuWritePerformed(TConnectedBus &connectedBus) {
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
    // Writing to Prg-Rom select the Chr-rom bank
    else if (address >= 0x8000) {
        _chrRomBankSelect = data & 0x3;
        
        // TODO: rajouter un assert pour etre sur que le bank select ne depasse pas IChrRomSizeInKb
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, unsigned int IChrRomSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper3<IPrgRomSizeInKb, IPrgRamSizeInKb, IChrRomSizeInKb, EMirroring>::ppuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Chr-Rom
    if (address < 0x2000) {
        // Read Chr-Rom
        connectedBus.setDataBus(_chrRom[(_chrRomBankSelect << 13) | address]);
    }
    // Internal VRAM
    else if (address < 0x4000) {
        // Read VRAM with mirrored address
        connectedBus.setDataBus(connectedBus.getVram()[getMirroredAddress<EMirroring>(address)]);
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, unsigned int IChrRomSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper3<IPrgRomSizeInKb, IPrgRamSizeInKb, IChrRomSizeInKb, EMirroring>::ppuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    uint8_t data = connectedBus.getDataBus();
    
    // Nothing for Chr-Rom (Can't write to a ROM)
    // Internal VRAM
    if ((address >= 0x2000) && (address < 0x4000)) {
        // Write VRAM with mirrored address
        connectedBus.getVram()[getMirroredAddress<EMirroring>(address)] = data;
    }
}

#endif /* NESEmu_Cartridge_Mapper3_s_hpp */
