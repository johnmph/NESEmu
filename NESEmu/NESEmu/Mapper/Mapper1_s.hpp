//
//  Mapper1_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Mapper1_s_hpp
#define NESEmu_Mapper_Mapper1_s_hpp


template <Model EModel>
Chip<EModel>::Chip(std::vector<uint8_t> prgRom, std::vector<uint8_t> prgRam) : _prgRom(std::move(prgRom)), _prgRam(/*std::move(prgRam)*/8 * 1024), _chrRam(8 * 1024), _shiftRegister(0x10), _shiftCount(0) {//TODO: a voir pour les valeurs des registres
    // Calculate sizes
    _prgRomSize = _prgRom.size();
    _prgRamSize = _prgRam.size();
    
    // Set internal registers
    _internalRegisters[0] = 0xC;        // TODO a voir
    _internalRegisters[1] = 0x0;
    _internalRegisters[2] = 0x0;
    _internalRegisters[3] = 0x0;
}

template <Model EModel>
template <class TConnectedBus, class TInterruptHardware>
void Chip<EModel>::clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware) {
    // Does nothing
}

template <Model EModel>
template <class TConnectedBus>
void Chip<EModel>::cpuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If Prg-Ram enabled
        if ((_internalRegisters[3] & 0x10) == 0x0) {
            // Read Prg-Ram with possible mirrored address
            connectedBus.setDataBus(_prgRam[address & (_prgRamSize - 1)]);
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
        connectedBus.setDataBus(_prgRom[(bank << 14) | (address & mask)]);
    }
}

template <Model EModel>
template <class TConnectedBus>
void Chip<EModel>::cpuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    uint8_t data = connectedBus.getDataBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If Prg-Ram enabled
        if ((_internalRegisters[3] & 0x10) == 0x0) {
            // Write Prg-Ram with possible mirrored address
            _prgRam[address & (_prgRamSize - 1)] = data;
        }
    }
    // Shift register
    else if (address >= 0x8000) {//TODO: attention, si 2 writes consecutifs, il n'y a que le 1er qui est pris en compte (a implementer surement avec clock en sauvant le rw signal du cpu et en le checkant ici (voir https://wiki.nesdev.com/w/index.php/MMC1 ) a tester avec Bill & Ted's Excellent Adventure
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

template <Model EModel>
template <class TConnectedBus>
void Chip<EModel>::ppuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Chr-Ram
    if (address < 0x2000) {
        // Read Chr-Ram
        connectedBus.setDataBus(_chrRam[getChrRamAddress(address)]);
    }
    // Internal VRAM
    else if (address < 0x4000) {
        // Read VRAM with mirrored address
        connectedBus.setDataBus(connectedBus.getVram()[getVramAddress(address)]);
    }
}

template <Model EModel>
template <class TConnectedBus>
void Chip<EModel>::ppuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    uint8_t data = connectedBus.getDataBus();
    
    // Chr-Ram
    if (address < 0x2000) {
        // Write Chr-Ram
        _chrRam[getChrRamAddress(address)] = data;
    }
    // Internal VRAM
    else if (address < 0x4000) {
        // Write VRAM with mirrored address
        connectedBus.getVram()[getVramAddress(address)] = data;
    }
}

template <Model EModel>
uint16_t Chip<EModel>::getChrRamAddress(uint16_t address) {
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

template <Model EModel>
uint16_t Chip<EModel>::getVramAddress(uint16_t address) {
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

#endif /* NESEmu_Mapper_Mapper1_s_hpp */
