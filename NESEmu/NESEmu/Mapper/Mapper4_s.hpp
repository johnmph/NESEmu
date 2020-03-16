//
//  Mapper4_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Mapper4_s_hpp
#define NESEmu_Mapper_Mapper4_s_hpp

// TODO: avoir un enum version (comme le ppu, le cpu, ...) car il y a plusieurs versions du MMC3 avec une gestion differente de l'irq
template <unsigned int IPrgRomSizeInKb, unsigned int IChrRomSizeInKb>
Mapper4<IPrgRomSizeInKb, IChrRomSizeInKb>::Mapper4(std::istream &istream) : _prgRom(IPrgRomSizeInKb * 1024), _prgRam(8 * 1024), _chrRom(IChrRomSizeInKb * 1024), _bankRegisterSelect(0), _prgRomBankMode(false), _chrRomBankMode(false), _nametableMirroring(false), _prgRamWriteProtection(false), _prgRamChipEnable(false), _lastA12(0), _irqEnable(false), _irqReload(false), _irqLatch(0), _irqCounter(0) {
    // TODO: voir si read via istream ici ou bien dans un factory a part et avoir directement ici les vectors a copier simplement : doit etre en dehors
    // TODO: pour tests :
    // Skip header
    istream.seekg(0x10);
    
    // Read Prg-Rom
    istream.read(reinterpret_cast<char *>(_prgRom.data()), IPrgRomSizeInKb * 1024);
    
    // Read Chr-Rom
    istream.read(reinterpret_cast<char *>(_chrRom.data()), IChrRomSizeInKb * 1024);
    
    // TODO: reseter aussi les _bankSelect ?
    //memset(_bankSelect, 0, sizeof(_bankSelect[0]) * 8);
}

template <unsigned int IPrgRomSizeInKb, unsigned int IChrRomSizeInKb>
template <class TConnectedBus, class TInterruptHardware>
void Mapper4<IPrgRomSizeInKb, IChrRomSizeInKb>::clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Process IRQ counter
    processIrqCounter(interruptHardware, (address & 0x1000) != 0x0);
}

template <unsigned int IPrgRomSizeInKb, unsigned int IChrRomSizeInKb>
template <class TConnectedBus>
void Mapper4<IPrgRomSizeInKb, IChrRomSizeInKb>::cpuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If Prg-Ram enabled
        if (_prgRamChipEnable) {
            // Read Prg-Ram
            connectedBus.setDataBus(_prgRam[address & 0x1FFF]);
        }
    }
    // Prg-Rom
    else if (address >= 0x8000) {   // TODO: voir si nécessaire la condition (est ce qu'on peut etre en dessous de 0x6000 dans cette methode ? : oui nécessaire !!!
        
        uint8_t bank;
        
        if (address < 0xA000) {
            bank = (_prgRomBankMode) ? ((IPrgRomSizeInKb >> 3) - 2)  : _bankSelect[6];
        }
        else if (address < 0xC000) {
            bank = _bankSelect[7];
        }
        else if (address < 0xE000) {
            bank = (_prgRomBankMode) ? _bankSelect[6] : ((IPrgRomSizeInKb >> 3) - 2);
        }
        else {
            bank = (IPrgRomSizeInKb >> 3) - 1;
        }
        
        // Read Prg-Rom
        connectedBus.setDataBus(_prgRom[(bank << 13) | (address & 0x1FFF)]);
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IChrRomSizeInKb>
template <class TConnectedBus>
void Mapper4<IPrgRomSizeInKb, IChrRomSizeInKb>::cpuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    uint8_t data = connectedBus.getDataBus();
    
    // Prg-Ram
    if ((address >= 0x6000) && (address < 0x8000)) {
        // If Prg-Ram enabled and write enabled
        if (_prgRamChipEnable && !_prgRamWriteProtection) {
            // Write Prg-Ram
            _prgRam[address & 0x1FFF] = data;
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
            _nametableMirroring = (data & 0x1) != 0x0;  // TODO: voir pour les four-screen cartridge !!
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
            connectedBus.irq(true);
        }
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IChrRomSizeInKb>
template <class TConnectedBus>
void Mapper4<IPrgRomSizeInKb, IChrRomSizeInKb>::ppuReadPerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Chr-Rom
    if (address < 0x2000) {
        
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
        connectedBus.setDataBus(_chrRom[(bank << 10) | (address & mask)]);
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Get mirrored address TODO peut etre changer ca par un nametableMirroring qui est une reference de methode, voir si plus rapide : en tout cas mettre ca dans une methode a part getMirroredVramAddress car le meme qu'en dessous
        uint16_t mirroredAddress = (_nametableMirroring) ? getMirroredAddress<MirroringType::Horizontal>(address) : getMirroredAddress<MirroringType::Vertical>(address);
        
        // Read VRAM with mirrored address
        connectedBus.setDataBus(connectedBus.getVram()[mirroredAddress]);
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IChrRomSizeInKb>
template <class TConnectedBus>
void Mapper4<IPrgRomSizeInKb, IChrRomSizeInKb>::ppuWritePerformed(TConnectedBus &connectedBus) {
    // Get address
    uint16_t address = connectedBus.getAddressBus();
    
    // Get data
    uint8_t data = connectedBus.getDataBus();
    
    // Nothing for Chr-Rom (Can't write to a ROM)
    // Internal VRAM (PPU address is always < 0x4000)
    if (address >= 0x2000) {
        // Get mirrored address TODO peut etre changer ca par un nametableMirroring qui est une reference de methode, voir si plus rapide
        uint16_t mirroredAddress = (_nametableMirroring) ? getMirroredAddress<MirroringType::Horizontal>(address) : getMirroredAddress<MirroringType::Vertical>(address);
        
        // Write VRAM with mirrored address
        connectedBus.getVram()[mirroredAddress] = data;
    }
}

template <unsigned int IPrgRomSizeInKb, unsigned int IChrRomSizeInKb>
template <class TConnectedBus>
void Mapper4<IPrgRomSizeInKb, IChrRomSizeInKb>::processIrqCounter(TConnectedBus &connectedBus, bool a12) {
    // Save A12 and filter it (only clock if A12 is high and was low for at least 3 CPU cycles before)
    bool savedLastA12 = _lastA12 != 0;
    _lastA12 = (a12 << 3) | (_lastA12 >> 1);//TODO: j'ai du rajouter une etape au filtre pour que la rom de test passe !!!
    
    // Check if IRQ clocked
    if (!(a12 && (!savedLastA12))) {
        return;
    }
    
    //std::cout << "scanline " << connectedBus._nes._ppu._currentScanline << ", pixel = " << connectedBus._nes._ppu._currentPixel << "\n";
    
    // Decrement counter or reload it if zero or forced
    if ((_irqCounter > 0) && (!_irqReload)) {
        --_irqCounter;
    } else {
        _irqCounter = _irqLatch;
        _irqReload = false;
    }
    
    // Generate an IRQ if needed
    if (_irqEnable && (_irqCounter == 0)) {
        connectedBus.irq(false);
    }
}

#endif /* NESEmu_Mapper_Mapper4_s_hpp */
