//
//  Interface.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Interface_hpp
#define NESEmu_Cartridge_Interface_hpp

#include <cstdint>
#include <vector>


namespace NESEmu { namespace Cartridge {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Interface {
        
        Interface(std::vector<uint8_t> prgRom, std::size_t prgRamSize, std::vector<uint8_t> chrRom, std::size_t chrRamSize);
        virtual ~Interface() = 0;
        
        // Persistent memory
        bool hasPersistentMemory() const;
        std::vector<uint8_t> &getPersistentMemory();// TODO: renommer getPersistentMemory() et mettre en pure virtual car selon le mapper ca peut etre autre chose que la prg-ram ? (et surement le retourner en copie et pas en reference sinon les mappers qui n'en n'ont pas ne seront rien retourner comme reference !!) + un setPersistentMemory : OU BIEN avoir un setPersistentMemoryHandler qui sauvera une fonction (std::function) qui sera appelée par la cartridge si elle a une persistent memory (mais dans ce cas il faudra aussi des methodes notifyInserted et notifyRemoved pour savoir quand lire et ecrire la persistent memory via la function sauvée)
        
        // Clock
        virtual void clock(TCpuHardwareInterface &cpuHardwareInterface, TPpuHardwareInterface &ppuHardwareInterface);
        
        // Cpu memory bus
        virtual void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) = 0;
        virtual void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) = 0;
        
        // Ppu memory bus
        virtual void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) = 0;
        virtual void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) = 0;
        
    protected:
        std::vector<uint8_t> const _prgRom;
        std::vector<uint8_t> _prgRam;
        std::vector<uint8_t> const _chrRom;
        std::vector<uint8_t> _chrRam;
        std::size_t const _prgRomSize;
        std::size_t const _prgRamSize;
        std::size_t const _chrRomSize;
        std::size_t const _chrRamSize;
    };
    
    #include "Interface_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Interface_hpp */
