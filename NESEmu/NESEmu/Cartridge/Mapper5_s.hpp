//
//  Mapper5_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 20/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper5_s_hpp
#define NESEmu_Cartridge_Mapper5_s_hpp


template <class TCpuHardwareInterface, class TPpuHardwareInterface>
Chip<TCpuHardwareInterface, TPpuHardwareInterface>::Chip(std::vector<uint8_t> prgRom, std::size_t prgRamSize, std::vector<uint8_t> chrRom, std::size_t chrRamSize) : Interface<TCpuHardwareInterface, TPpuHardwareInterface>(std::move(prgRom), prgRamSize, std::move(chrRom), chrRamSize), _prgRamHasTwoChips((prgRamSize == (16 * 1024)) || (prgRamSize == (64 * 1024))), _ram(1 * 1024), _prgMode(0x3), _prgRamProtect(0), _scanlineIrqPending(false), _inFrame(false), _8BitMultiplicand(0xFF), _8BitMultiplier(0xFF) {
    _prgBankSwitch[4] = 0xFF;
    //TODO: voir pour le reste
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::clock(TCpuHardwareInterface &cpuHardwareInterface, TPpuHardwareInterface &ppuHardwareInterface) {
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Scanline IRQ Status
    if (address == 0x5204) {
        // Set data bus (only bits 6 and 7)
        cpuHardwareInterface.setDataBus((_scanlineIrqPending << 7) | (_inFrame << 6), 0xC0);
        
        // Clear scanline IRQ pending flag
        _scanlineIrqPending = false;
    }
    // Unsigned 16-bit Product (low byte)
    else if (address == 0x5205) {
        // Calculate result
        uint16_t result = _8BitMultiplicand * _8BitMultiplier;
        
        // Set data bus to low byte result
        cpuHardwareInterface.setDataBus(result & 0xFF);
    }
    // Unsigned 16-bit Product (high byte)
    else if (address == 0x5206) {
        // Calculate result
        uint16_t result = _8BitMultiplicand * _8BitMultiplier;
        
        // Set data bus to high byte result
        cpuHardwareInterface.setDataBus(result >> 8);
    }
    // Prg-Ram
    else if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram
        if (this->hasPrgRam()) {
            // Read Prg-Ram
            //TODO: a voir : _prgBankSwitch[0] & 0x7F
            cpuHardwareInterface.setDataBus(this->readPrgRam(address));//TODO: a faire
        }
    }
    // Prg-Rom
    else if (address >= 0x8000) {//TODO: il faut gerer le bit 7 (RAM/ROM) mais qu'est ce que c'est exactement ?
        // Mode 0 (1 x 32kb bank)
        if (_prgMode == 0x0) {
            // Read Prg-Rom
            cpuHardwareInterface.setDataBus(this->readPrgRom(((_prgBankSwitch[4] & 0x78) << 13) | (address & 0x7FFF)));
        }
        // Mode 1 (2 x 16kb banks) or Mode 2 (first 16kb bank)
        else if ((_prgMode == 0x1) || ((_prgMode == 0x2) && (address < 0xC000))) {
            // Read Prg-Rom
            cpuHardwareInterface.setDataBus(this->readPrgRom(((_prgBankSwitch[2 << ((address >> 14) & 0x1)] & 0x7C) << 13) | (address & 0x3FFF)));
        }
        // Mode 2 (first and second 8kb banks) or Mode 3 (4 x 8kb banks)
        else if (((_prgMode == 0x2) && (address >= 0xC000)) || (_prgMode == 0x3)) {
            // Read Prg-Rom
            cpuHardwareInterface.setDataBus(this->readPrgRom((_prgBankSwitch[1 + ((address >> 13) & 0x3)] << 13) | (address & 0x1FFF)));
        }
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Get data
    uint8_t data = cpuHardwareInterface.getDataBus();
    
    // PPUCTRL Monitoring
    if (address == 0x2000) {
        _ppu2000Bit5 = (data & 0x20) != 0;
    }
    // PPUMASK Monitoring
    else if (address == 0x2001) {
        _ppu2001Bit3 = (data & 0x8) != 0;
        _ppu2001Bit4 = (data & 0x10) != 0;
    }
    // PRG Mode
    else if (address == 0x5100) {
        _prgMode = data & 0x3;
    }
    // CHR Mode
    else if (address == 0x5101) {
        _chrMode = data & 0x3;
    }
    // PRG-RAM Protect 1
    else if (address == 0x5102) {
        _prgRamProtect = (_prgRamProtect & 0xFC) | (data & 0x3);
    }
    // PRG-RAM Protect 2
    else if (address == 0x5103) {
        _prgRamProtect = (_prgRamProtect & 0xF3) | ((data & 0x3) << 2);
    }
    // Extended RAM mode
    else if (address == 0x5104) {
        _extendedRamMode = data & 0x3;
    }
    // Nametable Mapping
    else if (address == 0x5105) {
        _nametableMapping = data;
    }
    // Fill Mode Tile
    else if (address == 0x5106) {
        _fillModeTile = data;
    }
    // Fill Mode Color
    else if (address == 0x5107) {
        _fillModeColor = data & 0x3;
    }
    // PRG Bankswitching
    else if ((address >= 0x5113) && (address < 0x5118)) {
        _prgBankSwitch[address - 0x5113] = data;
    }
    // CHR Bankswitching
    else if ((address >= 0x5120) && (address < 0x512C)) {
        _chrBankSwitch[address - 0x5120] = data;
    }
    // Upper CHR Bank bits
    else if (address == 0x5130) {
        _upperChrBankBits = data & 0x3;
    }
    // Vertical Split Mode
    else if (address == 0x5200) {
        _verticalSplitMode = data;
    }
    // Vertical Split Scroll
    else if (address == 0x5201) {
        _verticalSplitScroll = data;
    }
    // Vertical Split Bank
    else if (address == 0x5202) {
        _verticalSplitBank = data;
    }
    // Scanline IRQ Compare Value
    else if (address == 0x5203) {
        _scanlineIrqCompareValue = data;
    }
    // Scanline IRQ Enable
    else if (address == 0x5204) {
        _scanlineIrqEnable = (data & 0x80) != 0;
    }
    // 8-bit Unsigned Multiplicand
    else if (address == 0x5205) {
        _8BitMultiplicand = data;
    }
    // 8-bit Unsigned Multiplier
    else if (address == 0x5206) {
        _8BitMultiplier = data;
    }
    // Prg-Ram
    else if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram and write enabled
        if ((this->hasPrgRam()) && (_prgRamProtect == 0x6)) {//TODO: vraiment pas sur que ce soit comme ca la prgRamProtect !!!!
            // Write Prg-Ram
            this->writePrgRam(address, data);
        }
    }
    // Bank select / data
    /*else if ((address >= 0x8000) && (address < 0xA000)) {
        // Bank select
        if ((address & 0x1) == 0x0) {
            _bankRegisterSelect = data & 0x7;
            _prgRomBankMode = (data & 0x40) != 0x0;
            _chrRomBankMode = (data & 0x80) != 0x0;
        }
        // Bank data
        else {
            // Only upper 7 bits for R0/R1
            if (_bankRegisterSelect < 2) {
                data &= 0xFE;
            }
            // Only lower 6 bits for R6/R7
            else if (_bankRegisterSelect >= 6) {
                data &= 0x3F;
            }
            
            // Set register
            _bankSelect[_bankRegisterSelect] = data;
        }
    }
    // Mirroring / Prg-Ram
    else if ((address >= 0xA000) && (address < 0xC000)) {
        // Nametable mirroring
        if ((address & 0x1) == 0x0) {
            _nametableMirroring = (data & 0x1) != 0x0;
        }
        // Prg-Ram protect
        else {
            _prgRamWriteProtection = (data & 0x40) != 0x0;
            _prgRamChipEnable = (data & 0x80) != 0x0;
        }
    }
    // IRQ latch / reload
    else if ((address >= 0xC000) && (address < 0xE000)) {
        // IRQ latch
        if ((address & 0x1) == 0x0) {
            _irqLatch = data;
        }
        // IRQ reload
        else {
            _irqReload = true;
        }
    }
    // IRQ disable / enable
    else if (address >= 0xE000) {
        _irqEnable = (address & 0x1) != 0x0;
        
        // Put the irq line high if disabled
        if (!_irqEnable) {
            cpuHardwareInterface.irq(true);
        }
    }*/
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    /*// Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Chr-Rom
    if (address < 0x2000) {
        if (this->_chrRamSize > 0) {
            // Read Chr-Ram
            ppuHardwareInterface.setDataBus(this->_chrRam[address & (this->_chrRamSize - 1)]);//TODO: voir si chr-ram est comme chr-rom (banked)
        } else {
            uint16_t mask;
            uint8_t bank;
            
            if (address < 0x800) {
                if (_chrRomBankMode) {
                    bank = _bankSelect[2 + ((address >> 10) & 0x1)];
                    mask = 0x3FF;
                } else {
                    bank = _bankSelect[0];
                    mask = 0x7FF;
                }
            }
            else if (address < 0x1000) {
                if (_chrRomBankMode) {
                    bank = _bankSelect[4 + ((address >> 10) & 0x1)];
                    mask = 0x3FF;
                } else {
                    bank = _bankSelect[1];
                    mask = 0x7FF;
                }
            }
            else if (address < 0x1800) {
                if (_chrRomBankMode) {
                    bank = _bankSelect[0];
                    mask = 0x7FF;
                } else {
                    bank = _bankSelect[2 + ((address >> 10) & 0x1)];
                    mask = 0x3FF;
                }
            }
            else {
                if (_chrRomBankMode) {
                    bank = _bankSelect[1];
                    mask = 0x7FF;
                } else {
                    bank = _bankSelect[4 + ((address >> 10) & 0x1)];
                    mask = 0x3FF;
                }
            }
            
            // Read Chr-Rom
            ppuHardwareInterface.setDataBus(this->_chrRom[((bank << 10) | (address & mask)) & (this->_chrRomSize - 1)]);
        }
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Read VRAM with mirrored address
        ppuHardwareInterface.setDataBus(ppuHardwareInterface.getVram()[getVramAddress(address)]);
    }*/
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    /*// Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Get data
    uint8_t data = ppuHardwareInterface.getDataBus();
    
    // Chr-Ram
    if (address < 0x2000) {
        // Write Chr-Ram if exist
        if (this->_chrRamSize > 0) {
            this->_chrRam[address & (this->_chrRamSize - 1)] = data;
        }
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Write VRAM with mirrored address
        ppuHardwareInterface.getVram()[getVramAddress(address)] = data;
    }*/
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
uint16_t Chip<TCpuHardwareInterface, TPpuHardwareInterface>::getVramAddress(uint16_t address) {
    /*// If hardwired four screen
    if (_mirroringType == MirroringType::FourScreen) {
        return getMirroredAddress<MirroringType::FourScreen>(address);
    }
    
    // Depend of mirroring register
     return (_nametableMirroring) ? getMirroredAddress<MirroringType::Horizontal>(address) : getMirroredAddress<MirroringType::Vertical>(address);*/return 0;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::processIrqCounter(TCpuHardwareInterface &cpuHardwareInterface, bool a12) {
    /*// Save A12 and filter it (only clock if A12 is high and was low for at least 3 CPU cycles before)
    bool savedLastA12 = _lastA12 != 0;
    _lastA12 = (a12 << 3) | (_lastA12 >> 1);//TODO: j'ai du rajouter une etape au filtre pour que la rom de test passe !!!
    
    // Check if IRQ clocked
    if (!(a12 && (!savedLastA12))) {
        return;
    }
    
    // Decrement counter or reload it if zero or forced
    if ((_irqCounter > 0) && (!_irqReload)) {
        --_irqCounter;
    } else {
        _irqCounter = _irqLatch;
        _irqReload = false;
    }
    
    // Generate an IRQ if needed
    if (_irqEnable && (_irqCounter == 0)) {
        cpuHardwareInterface.irq(false);
    }*/
}

#endif /* NESEmu_Cartridge_Mapper5_s_hpp */
