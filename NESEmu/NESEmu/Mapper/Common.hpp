//
//  Common.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Common_hpp
#define NESEmu_Mapper_Common_hpp

// See https://wiki.nesdev.com/w/index.php/CPU_memory_map
// See https://wiki.nesdev.com/w/index.php/PPU_memory_map
// See https://wiki.nesdev.com/w/index.php/Cartridge_connector

#include <cstdint>
#include <vector>


namespace NESEmu { namespace Mapper {
    
    // See https://wiki.nesdev.com/w/index.php/Mirroring
    enum class MirroringType {
        Horizontal,
        Vertical,
        SingleScreen,
        FourScreen
    };
    
    template <MirroringType EMirroring>
    uint16_t getMirroredAddress(uint16_t address);
    
    // Function template explicit instantiation of the specialization
    template <>
    uint16_t getMirroredAddress<MirroringType::Horizontal>(uint16_t address);
    
    template <>
    uint16_t getMirroredAddress<MirroringType::Vertical>(uint16_t address);
    
    template <>
    uint16_t getMirroredAddress<MirroringType::SingleScreen>(uint16_t address);
    
    template <>
    uint16_t getMirroredAddress<MirroringType::FourScreen>(uint16_t address);
    
    
    template <class TMMC>
    struct PrgRom {
        
        PrgRom(std::vector<uint8_t> data);
        
        // Cpu memory bus
        template <class TConnectedBus>
        void cpuReadPerformed(TMMC const &mmc, TConnectedBus &connectedBus) const;
        
    private:
        std::vector<uint8_t> const _data;
        //uint16_t const _addressMask;
    };
    
    template <class TMMC>
    struct PrgRam {
        
        PrgRam(std::vector<uint8_t> data);
        
        // Cpu memory bus
        template <class TConnectedBus>
        void cpuReadPerformed(TMMC const &mmc, TConnectedBus &connectedBus) const;
        
        template <class TConnectedBus>
        void cpuWritePerformed(TMMC const &mmc, TConnectedBus &connectedBus);
        
    private:
        std::vector<uint8_t> _data;
        //uint16_t const _addressMask;
    };
    
    template <class TMMC>
    struct ChrRom {
        
        ChrRom(std::vector<uint8_t> data);
        
        // Ppu memory bus
        template <class TConnectedBus>
        void ppuReadPerformed(TMMC const &mmc, TConnectedBus &connectedBus) const;
        
    private:
        std::vector<uint8_t> const _data;
        //uint16_t const _addressMask;
    };
    
    template <class TMMC>
    struct ChrRam {
        
        ChrRam(std::vector<uint8_t> data);
        
        // Ppu memory bus
        template <class TConnectedBus>
        void ppuReadPerformed(TMMC const &mmc, TConnectedBus &connectedBus) const;
        
        template <class TConnectedBus>
        void ppuWritePerformed(TMMC const &mmc, TConnectedBus &connectedBus);
        
    private:
        std::vector<uint8_t> _data;
        //uint16_t const _addressMask;
    };
    
    template <class TMMC>
    struct InternalVRam {
        
        // Ppu memory bus
        template <class TConnectedBus>
        void ppuReadPerformed(TMMC const &mmc, TConnectedBus &connectedBus) const;
        
        template <class TConnectedBus>
        void ppuWritePerformed(TMMC const &mmc, TConnectedBus &connectedBus);
    };
    
    #include "Common_s.hpp"
    
} }

#endif /* NESEmu_Mapper_Common_hpp */
