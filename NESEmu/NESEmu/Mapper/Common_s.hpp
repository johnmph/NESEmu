//
//  Common_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 14/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Common_s_hpp
#define NESEmu_Mapper_Common_s_hpp


// Prg-Rom
template <class TMMC>
PrgRom<TMMC>::PrgRom(std::vector<uint8_t> data) : _data(std::move(data))/*, _addressMask(_data.size() - 1)*/ {
}

template <class TMMC>
template <class TConnectedBus>
void PrgRom<TMMC>::cpuReadPerformed(TMMC const &mmc, TConnectedBus &connectedBus) const {
    uint16_t address = connectedBus.getAddressBus();
    
    if (address >= 0x8000) {
        // Read Prg-Rom with possible mirrored address
        connectedBus.setDataBus(_data[mmc.getPrgRomAddress(address, _data.size())/*address & _addressMask*/]);
    }
}

// Prg-Ram
template <class TMMC>
PrgRam<TMMC>::PrgRam(std::vector<uint8_t> data) : _data(std::move(data))/*, _addressMask(_data.size() - 1)*/ {
}

template <class TMMC>
template <class TConnectedBus>
void PrgRam<TMMC>::cpuReadPerformed(TMMC const &mmc, TConnectedBus &connectedBus) const {
    uint16_t address = connectedBus.getAddressBus();
    
    if ((address >= 0x6000) && (address < 0x8000) && mmc.isPrgRamReadEnabled()) {
        // Read Prg-Ram with possible mirrored address
        connectedBus.setDataBus(_data[mmc.getPrgRamAddress(address, _data.size())/*address & _addressMask*/]);
    }
}

template <class TMMC>
template <class TConnectedBus>
void PrgRam<TMMC>::cpuWritePerformed(TMMC const &mmc, TConnectedBus &connectedBus) {
    uint16_t address = connectedBus.getAddressBus();
    
    if ((address >= 0x6000) && (address < 0x8000) && mmc.isPrgRamWriteEnabled()) {
        // Write Prg-Ram with possible mirrored address
        _data[mmc.getPrgRamAddress(address, _data.size())/*address & _addressMask*/] = connectedBus.getDataBus();
    }
}


// Chr-Rom
template <class TMMC>
ChrRom<TMMC>::ChrRom(std::vector<uint8_t> data) : _data(std::move(data))/*, _addressMask(_data.size() - 1)*/ {
}

template <class TMMC>
template <class TConnectedBus>
void ChrRom<TMMC>::ppuReadPerformed(TMMC const &mmc, TConnectedBus &connectedBus) const {
    uint16_t address = connectedBus.getAddressBus();
    
    if (address < 0x2000) {
        // Read Chr-Rom with possible mirrored address
        connectedBus.setDataBus(_data[mmc.getChrRomAddress(address, _data.size())/*address & _addressMask*/]);
    }
}


// Chr-Ram
template <class TMMC>
ChrRam<TMMC>::ChrRam(std::vector<uint8_t> data) : _data(std::move(data))/*, _addressMask(_data.size() - 1)*/ {
}

template <class TMMC>
template <class TConnectedBus>
void ChrRam<TMMC>::ppuReadPerformed(TMMC const &mmc, TConnectedBus &connectedBus) const {
    uint16_t address = connectedBus.getAddressBus();
    
    if (address < 0x2000) {
        // Read Chr-Ram with possible mirrored address
        connectedBus.setDataBus(_data[mmc.getChrRamAddress(address, _data.size())/*address & _addressMask*/]);
    }
}

template <class TMMC>
template <class TConnectedBus>
void ChrRam<TMMC>::ppuWritePerformed(TMMC const &mmc, TConnectedBus &connectedBus) {
    uint16_t address = connectedBus.getAddressBus();
    
    if (address < 0x2000) {
        // Write Chr-Ram with possible mirrored address
        _data[mmc.getChrRamAddress(address, _data.size())/*address & _addressMask*/] = connectedBus.getDataBus();
    }
}


// Internal VRAM
template <class TMMC>
template <class TConnectedBus>
void InternalVRam<TMMC>::ppuReadPerformed(TMMC const &mmc, TConnectedBus &connectedBus) const {
    uint16_t address = connectedBus.getAddressBus();
    
    if ((address >= 0x2000) && (address < 0x4000)) {    // TODO: pas besoin de la condition car le mask & 0x3FFF est mis via le PPU
        // Read VRAM with mirrored address
        connectedBus.setDataBus(connectedBus.getVram()[mmc.getMirroredVRamAddress(address)]);
    }
}

template <class TMMC>
template <class TConnectedBus>
void InternalVRam<TMMC>::ppuWritePerformed(TMMC const &mmc, TConnectedBus &connectedBus) {
    uint16_t address = connectedBus.getAddressBus();
    
    // Internal VRAM
    if ((address >= 0x2000) && (address < 0x4000)) {    // TODO: pas besoin de la condition car le mask & 0x3FFF est mis via le PPU
        // Write VRAM with mirrored address
        connectedBus.getVram()[mmc.getMirroredVRamAddress(address)] = connectedBus.getDataBus();
    }
}

#endif /* NESEmu_Mapper_Common_s_hpp */
