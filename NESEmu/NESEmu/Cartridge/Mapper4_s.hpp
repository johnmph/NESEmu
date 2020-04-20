//
//  Mapper4_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper4_s_hpp
#define NESEmu_Cartridge_Mapper4_s_hpp

// TODO: avoir un enum version (comme le ppu, le cpu, ...) car il y a plusieurs versions du MMC3 avec une gestion differente de l'irq
template <class TCpuHardwareInterface, class TPpuHardwareInterface>
Chip<TCpuHardwareInterface, TPpuHardwareInterface>::Chip(std::vector<uint8_t> prgRom, std::size_t prgRamSize, std::vector<uint8_t> chrRom, std::size_t chrRamSize, MirroringType mirroringType) : Interface<TCpuHardwareInterface, TPpuHardwareInterface>(std::move(prgRom), prgRamSize, std::move(chrRom), chrRamSize), _mirroringType(mirroringType), _bankRegisterSelect(0), _prgRomBankMode(false), _chrRomBankMode(false), _nametableMirroring(false), _prgRamWriteProtection(false), _prgRamChipEnable(false), _lastA12(0), _irqEnable(false), _irqReload(false), _irqLatch(0), _irqCounter(0) {
    // TODO: reseter aussi les _bankSelect ?
    //memset(_bankSelect, 0, sizeof(_bankSelect[0]) * 8);
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::clock(TCpuHardwareInterface &cpuHardwareInterface, TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Process IRQ counter
    processIrqCounter(cpuHardwareInterface, (address & 0x1000) != 0x0);
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram and enabled
        if ((this->_prgRamSize > 0) && _prgRamChipEnable) {
            // Read Prg-Ram
            cpuHardwareInterface.setDataBus(this->_prgRam[address & (this->_prgRamSize - 1)]);
        }
    }
    // Prg-Rom
    else if (address >= 0x8000) {
        uint8_t bank;
        
        if (address < 0xA000) {
            bank = (_prgRomBankMode) ? ((this->_prgRomSize >> 13) - 2)  : _bankSelect[6];
        }
        else if (address < 0xC000) {
            bank = _bankSelect[7];
        }
        else if (address < 0xE000) {
            bank = (_prgRomBankMode) ? _bankSelect[6] : ((this->_prgRomSize >> 13) - 2);
        }
        else {
            bank = (this->_prgRomSize >> 13) - 1;
        }
        
        // Read Prg-Rom
        cpuHardwareInterface.setDataBus(this->_prgRom[((bank << 13) | (address & 0x1FFF)) & (this->_prgRomSize - 1)]);
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) {
    // Get address
    uint16_t address = cpuHardwareInterface.getAddressBus();
    
    // Get data
    uint8_t data = cpuHardwareInterface.getDataBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If has Prg-Ram and enabled and write enabled
        if ((this->_prgRamSize > 0) && _prgRamChipEnable && !_prgRamWriteProtection) {
            // Write Prg-Ram
            this->_prgRam[address & (this->_prgRamSize - 1)] = data;
        }
    }
    // Bank select / data
    else if ((address >= 0x8000) && (address < 0xA000)) {
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
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
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
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
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
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
uint16_t Chip<TCpuHardwareInterface, TPpuHardwareInterface>::getVramAddress(uint16_t address) {
    // If hardwired four screen
    if (_mirroringType == MirroringType::FourScreen) {
        return getMirroredAddress<MirroringType::FourScreen>(address);
    }
    
    // Depend of mirroring register
    return (_nametableMirroring) ? getMirroredAddress<MirroringType::Horizontal>(address) : getMirroredAddress<MirroringType::Vertical>(address);
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::processIrqCounter(TCpuHardwareInterface &cpuHardwareInterface, bool a12) {
    // Save A12 and filter it (only clock if A12 is high and was low for at least 3 CPU cycles before)
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
    }
}

#endif /* NESEmu_Cartridge_Mapper4_s_hpp */
