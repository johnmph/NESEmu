//
//  Mapper1.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 15/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "Mapper1.hpp"


namespace NESEmu { namespace Mapper { namespace Mapper1 {
    
    MMC1::MMC1() : _shiftRegister(0x10), _shiftCount(0) {
        // Set internal registers
        _internalRegisters[0] = 0xC;        // TODO a voir
        _internalRegisters[1] = 0x0;
        _internalRegisters[2] = 0x0;
        _internalRegisters[3] = 0x0;
    }
    
    bool MMC1::isPrgRamReadEnabled() const {
        return (_internalRegisters[3] & 0x10) == 0x0;
    }
    
    bool MMC1::isPrgRamWriteEnabled() const {
        return (_internalRegisters[3] & 0x10) == 0x0;
    }
    
    std::size_t MMC1::getPrgRomAddress(uint16_t address, std::size_t size) const {
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
            bank = (address < 0xC000) ? _internalRegisters[3] : ((size >> 14) - 1);
        }
        
        // Return address
        return (bank << 14) | (address & mask);
    }
    
    std::size_t MMC1::getPrgRamAddress(uint16_t address, std::size_t size) const {//TODO: a voir car possible d'avoir 16ko de prg-ram !!!
        return address & (size - 1);
    }
    
    std::size_t MMC1::getChrRomAddress(uint16_t address, std::size_t size) const {
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
    
    std::size_t MMC1::getChrRamAddress(uint16_t address, std::size_t size) const {
        return getChrRomAddress(address, size);
    }
    
    uint16_t MMC1::getMirroredVRamAddress(uint16_t address) const {
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
    
} } }
