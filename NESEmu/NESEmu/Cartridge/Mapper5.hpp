//
//  Mapper5.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 20/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper5_hpp
#define NESEmu_Cartridge_Mapper5_hpp

#include <vector>
#include "Interface.hpp"
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/MMC5

namespace NESEmu { namespace Cartridge { namespace Mapper5 {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Chip : Interface<TCpuHardwareInterface, TPpuHardwareInterface> {
        
        Chip(std::vector<uint8_t> prgRom, std::size_t prgRamSize, uint8_t prgRamChipCount, std::vector<uint8_t> chrRom, std::size_t chrRamSize);
        
        // TODO: il faudra rajouter une methode virtual dans Interface reset et ici l'overrider (il n'est pas vraiment au courant du reset via un signal envoyé mais par une absence de signal M2 pendant un certain temps mais bon ici on le fera comme ca
        void clock(TCpuHardwareInterface &cpuHardwareInterface, TPpuHardwareInterface &ppuHardwareInterface) override;
        
        // Cpu memory bus
        void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        
        // Ppu memory bus
        void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        
    private:
        
        bool is8x16SpriteMode() const;
        bool isPPUFetchSpriteData() const;
        void processScanlineDetection(uint16_t address);
        void checkPPUIsRendering();
        void checkInterrupt(TCpuHardwareInterface &cpuHardwareInterface);
        
        std::size_t const _prgRamChipSize;
        uint8_t const _prgRamChipCount;
        
        std::vector<uint8_t> _ram;
        uint8_t _prgMode;
        uint8_t _chrMode;
        bool _ppu2000Bit5;
        bool _ppu2001Bit3;
        bool _ppu2001Bit4;
        uint8_t _prgRamProtect;
        uint8_t _extendedRamMode;
        uint8_t _nametableMapping;
        uint8_t _fillModeTile;
        uint8_t _fillModeColor;
        uint8_t _prgBankSwitch[5];
        uint8_t _chrBankSwitch[12];
        uint8_t _upperChrBankBits;
        uint8_t _verticalSplitMode;
        uint8_t _verticalSplitScroll;
        uint8_t _verticalSplitBank;
        uint8_t _scanlineIrqCompareValue;
        bool _scanlineIrqEnabled;
        bool _scanlineIrqPending;
        bool _inFrame;
        uint8_t _8BitMultiplicand;
        uint8_t _8BitMultiplier;
        bool _lastWrittenChrRegisterIsBackgroundSet;
        
        uint16_t _lastPPUReadAddress;
        uint8_t _ppuReadAddressConsecutiveCounter;
        uint8_t _scanlineCounter;
        uint8_t _cpuWithoutPPUReadCycleCounter;
        bool _ppuIsReading;
        
        uint8_t _ppuFetchCounter;  // TODO : voir si uint8_t est suffisant : normalement oui car normalement 170 fetch du ppu par scanline
        
        // TODO: il reste le timer, la detection d'irq, ...
        
        // TODO: avoir un tableau constant de mask pour le PRG-RAM qui selon le nombre de chip et leur taille va determiner des masks pour les 8 valeurs des bits 0, 1 et 2 du registre courant. Si le mask == 0 alors open bus !
    };
    
    #include "Mapper5_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Mapper5_hpp */
