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
void PrgRom::cpuReadPerformed(TConnectedBus &connectedBus) const {
    uint16_t address = connectedBus.getAddressBus();
    
    if (address >= 0x8000) {
        // Read Prg-Rom with possible mirrored address
        connectedBus.setDataBus(_data[address & _addressMask]);
    }
}

template <class TConnectedBus>
void PrgRam::cpuReadPerformed(TConnectedBus &connectedBus) const {
    uint16_t address = connectedBus.getAddressBus();
    
    if ((address >= 0x6000) && (address < 0x8000)) {
        // Read Prg-Ram with possible mirrored address
        connectedBus.setDataBus(_data[address & _addressMask]);
    }
}

template <class TConnectedBus>
void PrgRam::cpuWritePerformed(TConnectedBus &connectedBus) {
    uint16_t address = connectedBus.getAddressBus();
    
    if ((address >= 0x6000) && (address < 0x8000)) {
        // Write Prg-Ram with possible mirrored address
        _data[address & _addressMask] = connectedBus.getDataBus();
    }
}

template <class TConnectedBus>
void ChrRom::ppuReadPerformed(TConnectedBus &connectedBus) const {
    uint16_t address = connectedBus.getAddressBus();
    
    if (address < 0x2000) {
        // Read Chr-Rom with possible mirrored address
        connectedBus.setDataBus(_data[address & _addressMask]);
    }
}

template <class TConnectedBus>
void ChrRam::ppuReadPerformed(TConnectedBus &connectedBus) const {
    uint16_t address = connectedBus.getAddressBus();
    
    if (address < 0x2000) {
        // Read Chr-Ram with possible mirrored address
        connectedBus.setDataBus(_data[address & _addressMask]);
    }
}

template <class TConnectedBus>
void ChrRam::ppuWritePerformed(TConnectedBus &connectedBus) {
    uint16_t address = connectedBus.getAddressBus();
    
    if (address < 0x2000) {
        // Write Chr-Ram with possible mirrored address
        _data[address & _addressMask] = connectedBus.getDataBus();
    }
}

template <MirroringType EMirroring>
template <class TConnectedBus>
void InternalVRam<EMirroring>::ppuReadPerformed(TConnectedBus &connectedBus) const {
    uint16_t address = connectedBus.getAddressBus();
    
    if ((address >= 0x2000) && (address < 0x4000)) {    // TODO: pas besoin de la condition car le mask & 0x3FFF est mis via le PPU
        // Read VRAM with mirrored address
        connectedBus.setDataBus(connectedBus.getVram()[getMirroredAddress<EMirroring>(address)]);
    }
}

template <MirroringType EMirroring>
template <class TConnectedBus>
void InternalVRam<EMirroring>::ppuWritePerformed(TConnectedBus &connectedBus) {
    uint16_t address = connectedBus.getAddressBus();
    
    // Internal VRAM
    if ((address >= 0x2000) && (address < 0x4000)) {    // TODO: pas besoin de la condition car le mask & 0x3FFF est mis via le PPU
        // Write VRAM with mirrored address
        connectedBus.getVram()[getMirroredAddress<EMirroring>(address)] = connectedBus.getDataBus();
    }
}



template <class ...TChips>
Chip<TChips...>::Chip(std::tuple<TChips...> chips) : _chips(std::move(chips)) {
}

template <class ...TChips>
template <class TConnectedBus, class TInterruptHardware>
void Chip<TChips...>::clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware) {
    // Does nothing
}

template <class ...TChips>
template <class TConnectedBus>
void Chip<TChips...>::cpuReadPerformed(TConnectedBus &connectedBus) {
    // Browse all chips
    chipCpuReadPerformed<TConnectedBus, TChips...>(connectedBus);
}

template <class ...TChips>
template <class TConnectedBus>
void Chip<TChips...>::cpuWritePerformed(TConnectedBus &connectedBus) {
    // Browse all chips
    chipCpuWritePerformed<TConnectedBus, TChips...>(connectedBus);
}

template <class ...TChips>
template <class TConnectedBus>
void Chip<TChips...>::ppuReadPerformed(TConnectedBus &connectedBus) {
    // Browse all chips
    chipPpuReadPerformed<TConnectedBus, TChips...>(connectedBus);
}

template <class ...TChips>
template <class TConnectedBus>
void Chip<TChips...>::ppuWritePerformed(TConnectedBus &connectedBus) {
    // Browse all chips
    chipPpuWritePerformed<TConnectedBus, TChips...>(connectedBus);
}


template <class ...TChips>
template <class TConnectedBus>
void Chip<TChips...>::chipCpuReadPerformed(TConnectedBus &connectedBus) {
}

template <class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<HasCpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Chip<TChips...>::chipCpuReadPerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).cpuReadPerformed(connectedBus);//TODO: attention, get avec le type est en C++14 seulement, en C++11 c'est avec un index
    chipCpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<!HasCpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Chip<TChips...>::chipCpuReadPerformed(TConnectedBus &connectedBus) {
    chipCpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class ...TChips>
template <class TConnectedBus>
void Chip<TChips...>::chipCpuWritePerformed(TConnectedBus &connectedBus) {
}

template <class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<HasCpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Chip<TChips...>::chipCpuWritePerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).cpuWritePerformed(connectedBus);
    chipCpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<!HasCpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Chip<TChips...>::chipCpuWritePerformed(TConnectedBus &connectedBus) {
    chipCpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class ...TChips>
template <class TConnectedBus>
void Chip<TChips...>::chipPpuReadPerformed(TConnectedBus &connectedBus) {
}

template <class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<HasPpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Chip<TChips...>::chipPpuReadPerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).ppuReadPerformed(connectedBus);
    chipPpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<!HasPpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Chip<TChips...>::chipPpuReadPerformed(TConnectedBus &connectedBus) {
    chipPpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class ...TChips>
template <class TConnectedBus>
void Chip<TChips...>::chipPpuWritePerformed(TConnectedBus &connectedBus) {
}

template <class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<HasPpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Chip<TChips...>::chipPpuWritePerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).ppuWritePerformed(connectedBus);
    chipPpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<!HasPpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Chip<TChips...>::chipPpuWritePerformed(TConnectedBus &connectedBus) {
    chipPpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

#endif /* NESEmu_Mapper_Mapper0n_s_hpp */
