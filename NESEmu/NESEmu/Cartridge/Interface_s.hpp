//
//  Interface_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Interface_s_hpp
#define NESEmu_Cartridge_Interface_s_hpp


template <class TCpuHardwareInterface, class TPpuHardwareInterface>
Interface<TCpuHardwareInterface, TPpuHardwareInterface>::Interface(std::vector<uint8_t> prgRom, std::size_t prgRamSize, std::vector<uint8_t> chrRom, std::size_t chrRamSize) : _prgRom(std::move(prgRom)), _prgRam(prgRamSize), _chrRom(std::move(chrRom)), _chrRam(chrRamSize), _prgRomSize(_prgRom.size()), _prgRamSize(_prgRam.size()), _chrRomSize(_chrRom.size()), _chrRamSize(_chrRam.size()) {
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
Interface<TCpuHardwareInterface, TPpuHardwareInterface>::~Interface() {
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
bool Interface<TCpuHardwareInterface, TPpuHardwareInterface>::hasPersistentMemory() const {
    return _prgRam.size() > 0;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
std::vector<uint8_t> &Interface<TCpuHardwareInterface, TPpuHardwareInterface>::getPersistentMemory() {
    return _prgRam;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Interface<TCpuHardwareInterface, TPpuHardwareInterface>::clock(TCpuHardwareInterface &cpuHardwareInterface, TPpuHardwareInterface &ppuHardwareInterface) {
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
bool Interface<TCpuHardwareInterface, TPpuHardwareInterface>::hasPrgRam() const {
    return _prgRamSize > 0;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
bool Interface<TCpuHardwareInterface, TPpuHardwareInterface>::hasChrRam() const {
    return _chrRamSize > 0;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
std::size_t Interface<TCpuHardwareInterface, TPpuHardwareInterface>::getPrgRomSize() const {
    return _prgRomSize;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
std::size_t Interface<TCpuHardwareInterface, TPpuHardwareInterface>::getPrgRamSize() const {
    return _prgRamSize;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
std::size_t Interface<TCpuHardwareInterface, TPpuHardwareInterface>::getChrRomSize() const {
    return _chrRomSize;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
std::size_t Interface<TCpuHardwareInterface, TPpuHardwareInterface>::getChrRamSize() const {
    return _chrRamSize;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
uint8_t Interface<TCpuHardwareInterface, TPpuHardwareInterface>::readPrgRom(uint16_t address) const {
    return _prgRom[address & (_prgRomSize - 1)];
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
uint8_t Interface<TCpuHardwareInterface, TPpuHardwareInterface>::readPrgRam(uint16_t address) const {
    return _prgRam[address & (_prgRamSize - 1)];
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Interface<TCpuHardwareInterface, TPpuHardwareInterface>::writePrgRam(uint16_t address, uint8_t data) {
    _prgRam[address & (_prgRamSize - 1)] = data;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
uint8_t Interface<TCpuHardwareInterface, TPpuHardwareInterface>::readChrRom(uint16_t address) const {
    return _chrRom[address & (_chrRomSize - 1)];
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
uint8_t Interface<TCpuHardwareInterface, TPpuHardwareInterface>::readChrRam(uint16_t address) const {
    return _chrRam[address & (_chrRamSize - 1)];
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Interface<TCpuHardwareInterface, TPpuHardwareInterface>::writeChrRam(uint16_t address, uint8_t data) {
    _chrRam[address & (_chrRamSize - 1)] = data;
}

#endif /* NESEmu_Cartridge_Interface_s_hpp */
