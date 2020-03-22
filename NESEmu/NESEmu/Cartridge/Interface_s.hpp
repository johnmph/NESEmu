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

#endif /* NESEmu_Cartridge_Interface_s_hpp */
