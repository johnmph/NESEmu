//
//  Mapper0n_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Mapper0n_s_hpp
#define NESEmu_Mapper_Mapper0n_s_hpp


template <class TConnectedBus>
void PrgRom::cpuReadPerformed(TConnectedBus &connectedBus, uint16_t address) const {
    if (address >= 0x8000) {
        // Read Prg-Rom with possible mirrored address
        connectedBus.setDataBus(_data[address & _addressMask]);
    }
}
/*
template <class TConnectedBus>
void PrgRom::cpuWritePerformed(TConnectedBus &connectedBus, uint16_t address) {
}*/

template <class TConnectedBus>
void PrgRam::cpuReadPerformed(TConnectedBus &connectedBus, uint16_t address) const {
    if ((address >= 0x6000) && (address < 0x8000)) {
        // Read Prg-Ram with possible mirrored address
        connectedBus.setDataBus(_data[address & _addressMask]);
    }
}

template <class TConnectedBus>
void PrgRam::cpuWritePerformed(TConnectedBus &connectedBus, uint16_t address) {
    if ((address >= 0x6000) && (address < 0x8000)) {
        // Write Prg-Ram with possible mirrored address
        _data[address & _addressMask] = connectedBus.getDataBus();
    }
}

template <class TConnectedBus>
void ChrRom::ppuReadPerformed(TConnectedBus &connectedBus, uint16_t address) const {
    if (address < 0x2000) {
        // Read Chr-Rom with possible mirrored address
        connectedBus.setDataBus(_data[address & _addressMask]);
    }
}

template <class TConnectedBus>
void ChrRom::ppuWritePerformed(TConnectedBus &connectedBus, uint16_t address) {
}

template <class TConnectedBus>
void ChrRam::ppuReadPerformed(TConnectedBus &connectedBus, uint16_t address) const {
    if (address < 0x2000) {
        // Read Chr-Ram with possible mirrored address
        connectedBus.setDataBus(_data[address & _addressMask]);
    }
}

template <class TConnectedBus>
void ChrRam::ppuWritePerformed(TConnectedBus &connectedBus, uint16_t address) {
    if (address < 0x2000) {
        // Write Chr-Ram with possible mirrored address
        _data[address & _addressMask] = connectedBus.getDataBus();
    }
}



template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
Chip<EMirroring, TCpuChips, TPpuChips>::Chip(TCpuChips cpuChips, TPpuChips ppuChips) : _cpuChips(std::move(cpuChips)), _ppuChips(std::move(ppuChips)) {
}// TODO: comment faire avec le constructors et les constructors des Chips ?? : il va falloir surement deriver de chacun des types : ou bien on ne fait pas ainsi mais on a une liste finie de chip (PrgRomChip, PrgRamChip, ...), on peut aussi avoir un truc plus traditionnel avec juste des methodes avec enable_if et des parametres template hasPrgRam, hasChrRam, ... : voir ainsi avec les tuples

template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
template <class TConnectedBus, class TInterruptHardware>
void Chip<EMirroring, TCpuChips, TPpuChips>::clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware) {
    // Does nothing
}
/*
template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
template <class TConnectedBus, class Chip>
void Chip<EMirroring, TCpuChips, TPpuChips>::cpuReadPerformed(TConnectedBus &connectedBus, uint16_t address) {
    std::get<Chip>(_chips).cpuReadPerformed(connectedBus, address);//TODO: attention, get avec le type est en C++14 seulement, en C++11 c'est avec un index
}

template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
template <class TConnectedBus, class FirstChip, class ...OtherChips>
void Chip<EMirroring, TCpuChips, TPpuChips>::cpuReadPerformed(TConnectedBus &connectedBus, uint16_t address) {
    cpuReadPerformed<FirstChip>(connectedBus, address);
    cpuReadPerformed<OtherChips...>(connectedBus, address);
}*/

template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
template <class TConnectedBus>
void Chip<EMirroring, TCpuChips, TPpuChips>::cpuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // TODO: il faut parcourir toutes les chips et appeler leur cpuReadPerformed
    cpuReadPerformed(_cpuChips, connectedBus, address);
}

template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
template <class TConnectedBus>
void Chip<EMirroring, TCpuChips, TPpuChips>::cpuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    //uint8_t data = connectedBus.getDataBus();
    
    // TODO: il faut parcourir toutes les chips et appeler leur cpuWritePerformed
    cpuWritePerformed(_cpuChips, connectedBus, address);
}

template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
template <class TConnectedBus>
void Chip<EMirroring, TCpuChips, TPpuChips>::ppuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // TODO: il faut parcourir toutes les chips et appeler leur ppuReadPerformed
    ppuReadPerformed(_ppuChips, connectedBus, address);
    
    // Internal VRAM
    if ((address >= 0x2000) && (address < 0x4000)) {    // TODO: pas besoin de la condition car le mask & 0x3FFF est mis via le PPU
        // Read VRAM with mirrored address
        connectedBus.setDataBus(connectedBus.getVram()[getMirroredAddress<EMirroring>(address)]);
    }
}

template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
template <class TConnectedBus>
void Chip<EMirroring, TCpuChips, TPpuChips>::ppuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    uint8_t data = connectedBus.getDataBus();
    
    // TODO: il faut parcourir toutes les chips et appeler leur ppuWritePerformed
    ppuWritePerformed(_ppuChips, connectedBus, address);
    
    // Internal VRAM
    if ((address >= 0x2000) && (address < 0x4000)) {    // TODO: pas besoin de la condition car le mask & 0x3FFF est mis via le PPU
        // Write VRAM with mirrored address
        connectedBus.getVram()[getMirroredAddress<EMirroring>(address)] = data;
    }
}

template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
template <class TConnectedBus, class ...TChips>
void Chip<EMirroring, TCpuChips, TPpuChips>::cpuReadPerformed(std::tuple<TChips...> &chipsList, TConnectedBus &connectedBus, uint16_t address) {
    // Fold expression in C++11 (second line is to avoid warning on non used variable)
    int fold[] = { ((std::get<TChips>(chipsList).cpuReadPerformed(connectedBus, address)), 0)... };
    (void) fold;
}

template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
template <class TConnectedBus, class ...TChips>
void Chip<EMirroring, TCpuChips, TPpuChips>::cpuWritePerformed(std::tuple<TChips...> &chipsList, TConnectedBus &connectedBus, uint16_t address) {
    // Fold expression in C++11 (second line is to avoid warning on non used variable)
    int fold[] = { ((std::get<TChips>(chipsList).cpuWritePerformed(connectedBus, address)), 0)... };
    (void) fold;
}

template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
template <class TConnectedBus, class ...TChips>
void Chip<EMirroring, TCpuChips, TPpuChips>::ppuReadPerformed(std::tuple<TChips...> &chipsList, TConnectedBus &connectedBus, uint16_t address) {
    // Fold expression in C++11 (second line is to avoid warning on non used variable)
    int fold[] = { ((std::get<TChips>(chipsList).ppuReadPerformed(connectedBus, address)), 0)... };
    (void) fold;
}

template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
template <class TConnectedBus, class ...TChips>
void Chip<EMirroring, TCpuChips, TPpuChips>::ppuWritePerformed(std::tuple<TChips...> &chipsList, TConnectedBus &connectedBus, uint16_t address) {
    // Fold expression in C++11 (second line is to avoid warning on non used variable)
    int fold[] = { ((std::get<TChips>(chipsList).ppuWritePerformed(connectedBus, address)), 0)... };
    (void) fold;
}

#endif /* NESEmu_Mapper_Mapper0n_s_hpp */
