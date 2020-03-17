//
//  Mapper1_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper1_s_hpp
#define NESEmu_Cartridge_Mapper1_s_hpp


template <class TCpuHardwareInterface, class TPpuHardwareInterface, Model EModel>
Chip<TCpuHardwareInterface, TPpuHardwareInterface, EModel>::Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> prgRam, std::vector<uint8_t> chrRom, std::size_t chrRamSize) : _prgRom(std::move(prgRom)), _prgRam(std::move(prgRam)), _chrRom(std::move(chrRom)), _chrRam(chrRamSize), _prgRomSize(_prgRom.size()), _prgRamSize(_prgRam.size()), _hasChrRam(_chrRam.size() > 0), _chrRomOrRamSize((_hasChrRam) ? _chrRam.size() : _chrRom.size()), _shiftRegister(0x10), _shiftCount(0) {//TODO: a voir pour les valeurs des registres
    // Set internal registers
    _internalRegisters[0] = 0xC;        // TODO a voir
    _internalRegisters[1] = 0x0;
    _internalRegisters[2] = 0x0;
    _internalRegisters[3] = 0x0;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface, Model EModel>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface, EModel>::cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram and enabled
        if ((_prgRamSize > 0) && ((_internalRegisters[3] & 0x10) == 0x0)) {
            // Read Prg-Ram with possible mirrored address
            cpuHardwareInterface.setDataBus(_prgRam[address & (_prgRamSize - 1)]);
        }
    }
    // Prg-Rom
    else if (address >= 0x8000) {   // TODO: voir si nécessaire la condition (est ce qu'on peut etre en dessous de 0x6000 dans cette methode ? : oui nécessaire !!!
        
        uint16_t mask;
        uint8_t bank;
        
        // 32kb switch
        if ((_internalRegisters[0] & 0x8) == 0x0) {
            mask = 0x7FFF;
            bank = _internalRegisters[3] & 0xE;
        }
        // Last 16kb switch
        else if ((_internalRegisters[0] & 0x4) == 0x0) {
            mask = 0x3FFF;
            bank = (address < 0xC000) ? 0x0 : _internalRegisters[3];
        }
        // First 16kb switch
        else {
            mask = 0x3FFF;
            bank = (address < 0xC000) ? _internalRegisters[3] : ((_prgRomSize >> 14) - 1);
        }
        
        // Read Prg-Rom
        cpuHardwareInterface.setDataBus(_prgRom[((bank << 14) | (address & mask)) & (_prgRomSize - 1)]);
    }
    
    // A read is performed so we reset the _lastCycleWasWrite flag
    _lastCycleWasWrite = false;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface, Model EModel>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface, EModel>::cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Get data
    uint8_t data = cpuHardwareInterface.getDataBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram and enabled
        if ((_prgRamSize > 0) && ((_internalRegisters[3] & 0x10) == 0x0)) {
            // Write Prg-Ram with possible mirrored address
            _prgRam[address & (_prgRamSize - 1)] = data;
        }
    }
    // If we write to rom and if it's the first write of consecutives writes
    else if ((address >= 0x8000) && (!_lastCycleWasWrite)) {
        // We perform a write, so we set _lastCycleWasWrite flag
        _lastCycleWasWrite = true;//TODO: voir si ici ou en dehors du if
        
        // Clear registers
        if ((data & 0x80) != 0x0) {
            _shiftRegister = 0x10;
            _shiftCount = 0;
            _internalRegisters[0] |= 0xC0;
            
            return;
        }
        
        // Shift register and load LSB in shift register MSB
        _shiftRegister >>= 1;
        _shiftRegister |= (data & 0x1) << 4;
        ++_shiftCount;
        
        // If last shift
        if (_shiftCount >= 5) {
            _internalRegisters[(address >> 13) & 0x3] = _shiftRegister;
            
            // Reset shift registers
            _shiftRegister = 0x10;
            _shiftCount = 0;
        }
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface, Model EModel>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface, EModel>::ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Chr-Rom / Ram
    if (address < 0x2000) {
        // Read Chr-Rom / Ram
        ppuHardwareInterface.setDataBus((_hasChrRam) ? _chrRam[getChrRamAddress(address) & (_chrRomOrRamSize - 1)] : _chrRom[getChrRamAddress(address) & (_chrRomOrRamSize - 1)]);
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Read VRAM with mirrored address
        ppuHardwareInterface.setDataBus(ppuHardwareInterface.getVram()[getVramAddress(address)]);
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface, Model EModel>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface, EModel>::ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Get data
    uint8_t data = ppuHardwareInterface.getDataBus();
    
    // Chr-Ram
    if (address < 0x2000) {
        // Write Chr-Ram if exist
        if (_hasChrRam) {
            _chrRam[getChrRamAddress(address) & (_chrRomOrRamSize - 1)] = data;
        }
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Write VRAM with mirrored address
        ppuHardwareInterface.getVram()[getVramAddress(address)] = data;
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface, Model EModel>
std::size_t Chip<TCpuHardwareInterface, TPpuHardwareInterface, EModel>::getChrRamAddress(uint16_t address) {
    uint16_t mask;
    uint8_t bank;
    
    // 8kb switch
    if ((_internalRegisters[0] & 0x10) == 0x0) {
        mask = 0x1FFF;
        bank = _internalRegisters[1] & 0x1E;
    }
    // 4kb switch
    else {
        mask = 0xFFF;
        bank = _internalRegisters[1 + ((address >> 12) & 0x1)];
    }
    
    return (bank << 12) | (address & mask);
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface, Model EModel>
uint16_t Chip<TCpuHardwareInterface, TPpuHardwareInterface, EModel>::getVramAddress(uint16_t address) {
    switch (_internalRegisters[0] & 0x3) {
        case 0x0 :
            return getMirroredAddress<MirroringType::SingleScreen>(address);
        break;
        
        case 0x1 :
            return 0x400 | getMirroredAddress<MirroringType::SingleScreen>(address);
        break;
        
        case 0x2 :
            return getMirroredAddress<MirroringType::Vertical>(address);
        break;
    }
    
    // 0x3 = Horizontal mirroring
    return getMirroredAddress<MirroringType::Horizontal>(address);
}

#endif /* NESEmu_Cartridge_Mapper1_s_hpp */
