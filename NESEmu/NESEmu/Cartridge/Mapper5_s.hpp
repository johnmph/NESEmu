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
Chip<TCpuHardwareInterface, TPpuHardwareInterface>::Chip(std::vector<uint8_t> prgRom, std::size_t prgRamSize, uint8_t prgRamChipCount, std::vector<uint8_t> chrRom, std::size_t chrRamSize) : Interface<TCpuHardwareInterface, TPpuHardwareInterface>(std::move(prgRom), prgRamSize, std::move(chrRom), chrRamSize), _prgRamChipSize(prgRamSize / prgRamChipCount), _prgRamChipCount(prgRamChipCount), _ram(1 * 1024), _prgMode(0x3), _prgRamProtect(0), _scanlineIrqPending(false), _inFrame(false), _8BitMultiplicand(0xFF), _8BitMultiplier(0xFF) {
    _prgBankSwitch[4] = 0xFF;
    //TODO: voir pour le reste
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::clock(TCpuHardwareInterface &cpuHardwareInterface, TPpuHardwareInterface &ppuHardwareInterface) {
    // Check if PPU is rendering
    checkPPUIsRendering();
    
    // Check interrupt
    checkInterrupt(cpuHardwareInterface);   // TODO: voir si ici, ou ici et aussi en processScanlineCounter
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
    // CL3 / SL3 Status TODO : seulement MMC5A
    else if (address == 0x5208) {
        std::cout << "MMC5: 0x5208 read\n";//TODO: a faire
    }
    // Timer TODO : seulement MMC5A
    else if (address == 0x5209) {
        std::cout << "MMC5: 0x5209 read\n";//TODO: a faire
    }
    // Expansion RAM
    else if ((address >= 0x5C00) && (address < 0x6000)) {
        // Read extended RAM if mode allows it
        if (_extendedRamMode >= 2) {
            cpuHardwareInterface.setDataBus(_ram[address & 0x3FF]);
        }
    }
    // Prg-Ram
    else if ((address >= 0x6000) && (address < 0x8000)) {
        // Get second chip bit
        bool secondChip = ((_prgBankSwitch[0] >> 2) & 0x1) != 0;
        
        // If has Prg-Ram and not in second chip if it has less than two chips (or chip greater than 32kb)
        if (this->hasPrgRam() && ((_prgRamChipSize > (32 * 1024)) || (!(secondChip && (_prgRamChipCount < 2))))) {
            uint32_t chipMask = static_cast<uint32_t>(_prgRamChipSize - 1);
            uint32_t chipAddress = (((_prgBankSwitch[0] & 0xF) << 13) | (address & 0x1FFF)) & chipMask;
            
            // Read Prg-Ram
            cpuHardwareInterface.setDataBus(this->readPrgRam((_prgRamChipSize * secondChip) | chipAddress));
        }
    }
    // Prg-Rom
    else if (address >= 0x8000) {
        // Mode 0 (1 x 32kb bank)
        if (_prgMode == 0x0) {
            // Read Prg-Rom
            cpuHardwareInterface.setDataBus(this->readPrgRom(((_prgBankSwitch[4] & 0x78) << 13) | (address & 0x7FFF)));
        }
        // Mode 1 (2 x 16kb banks) or Mode 2 (first 16kb bank)
        else if ((_prgMode == 0x1) || ((_prgMode == 0x2) && (address < 0xC000))) {
            uint8_t bankRegisterNumber = 2 << ((address >> 14) & 0x1);
            bool isPrgRam = ((_prgBankSwitch[bankRegisterNumber] & 0x80) == 0) && (address < 0xC000);
            
            // Read Prg-Ram
            if (isPrgRam) {
                cpuHardwareInterface.setDataBus(this->readPrgRam(((_prgBankSwitch[bankRegisterNumber] & 0xC) << 13) | (address & 0x3FFF)));
            }
            // Read Prg-Rom
            else {
                cpuHardwareInterface.setDataBus(this->readPrgRom(((_prgBankSwitch[bankRegisterNumber] & 0x7C) << 13) | (address & 0x3FFF)));
            }
        }
        // Mode 2 (first and second 8kb banks) or Mode 3 (4 x 8kb banks)
        else if (((_prgMode == 0x2) && (address >= 0xC000)) || (_prgMode == 0x3)) {
            uint8_t bankRegisterNumber = 1 + ((address >> 13) & 0x3);
            bool isPrgRam = ((_prgBankSwitch[bankRegisterNumber] & 0x80) == 0) && (address < 0xE000);
            
            // Read Prg-Ram
            if (isPrgRam) {
                cpuHardwareInterface.setDataBus(this->readPrgRam(((_prgBankSwitch[bankRegisterNumber] & 0xF) << 13) | (address & 0x1FFF)));
            }
            // Read Prg-Rom
            else {
                cpuHardwareInterface.setDataBus(this->readPrgRom(((_prgBankSwitch[bankRegisterNumber] & 0x7F) << 13) | (address & 0x1FFF)));
            }
        }
        
        // If read for NMI vector
        if ((address == 0xFFFA) || (address == 0xFFFB)) {//TODO: ca doit faire ca aussi quand le 241eme scanline est detecté mais comment ?
            // Reset scanline counter
            _scanlineCounter = 0;
            
            // Clear in frame flag
            _inFrame = false;
            
            // Clear scanline IRQ pending flag
            _scanlineIrqPending = false;    // TODO: ca doit etre fait aussi si le scanline 0 est detecté mais comment ?
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
        
        _lastWrittenChrRegisterIsBackgroundSet = (address >= 0x5128);   // TODO: pour le chr bank quand PPU read/write 2007 mais comment on sait ca ici (que l'acces au chr bank est pour un read/write 2007) ??? via le last cpu read/write address ?
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
        _scanlineIrqEnabled = (data & 0x80) != 0;
    }
    // 8-bit Unsigned Multiplicand
    else if (address == 0x5205) {
        _8BitMultiplicand = data;
    }
    // 8-bit Unsigned Multiplier
    else if (address == 0x5206) {
        _8BitMultiplier = data;
    }
    // CL3 / SL3 Data Direction / Output Data Source TODO : seulement MMC5A
    else if (address == 0x5207) {
        std::cout << "MMC5: 0x5207 write\n";//TODO: a faire
    }
    // CL3 / SL3 Status TODO : seulement MMC5A
    else if (address == 0x5208) {
        std::cout << "MMC5: 0x5208 write\n";//TODO: a faire
    }
    // Timer LSB TODO : seulement MMC5A
    else if (address == 0x5209) {
        std::cout << "MMC5: 0x5209 write\n";//TODO: a faire
    }
    // Timer MSB TODO : seulement MMC5A
    else if (address == 0x520A) {
        std::cout << "MMC5: 0x520A write\n";//TODO: a faire
    }
    // Unknown !!! (Maybe external RAM chip ??)
    else if ((address >= 0x5800) && (address < 0x5C00)) {
        std::cout << "MMC5: 0x5800-0x5BFF write\n";//TODO: a faire
    }
    // Expansion RAM
    else if ((address >= 0x5C00) && (address < 0x6000)) {
        // Write extended RAM if mode allows it
        if (_extendedRamMode < 3) {
            _ram[address & 0x3FF] = ((_extendedRamMode == 2) || _inFrame) ? data : 0;//TODO: voir pour inFrame !
        }
    }
    // Prg-Ram
    else if ((address >= 0x6000) && (address < 0x8000)) {
        // Get second chip bit
        bool secondChip = ((_prgBankSwitch[0] >> 2) & 0x1) != 0;
        
        // If has Prg-Ram and not in second chip if it has less than two chips (or chip greater than 32kb)
        if (this->hasPrgRam() && (_prgRamProtect == 0x6) && ((_prgRamChipSize > (32 * 1024)) || (!(secondChip && (_prgRamChipCount < 2))))) {//TODO: vraiment pas sur que ce soit comme ca la prgRamProtect !!!!
            uint32_t chipMask = static_cast<uint32_t>(_prgRamChipSize - 1);
            uint32_t chipAddress = (((_prgBankSwitch[0] & 0xF) << 13) | (address & 0x1FFF)) & chipMask;
            
            // Write Prg-Ram
            this->writePrgRam((_prgRamChipSize * secondChip) | chipAddress, data);
        }
    }
    // Prg-Ram mapped in Prg-Rom address space
    else if (address >= 0x8000) {
        // Mode 1 (2 x 16kb banks) or Mode 2 (first 16kb bank)
        if ((_prgMode == 0x1) || ((_prgMode == 0x2) && (address < 0xC000))) {
            uint8_t bankRegisterNumber = 2 << ((address >> 14) & 0x1);
            bool isPrgRam = ((_prgBankSwitch[bankRegisterNumber] & 0x80) == 0) && (address < 0xC000);
            
            // Write Prg-Ram
            if (isPrgRam) {
                this->writePrgRam(((_prgBankSwitch[bankRegisterNumber] & 0xC) << 13) | (address & 0x3FFF), data);
            }
        }
        // Mode 2 (first and second 8kb banks) or Mode 3 (4 x 8kb banks)
        else if (((_prgMode == 0x2) && (address >= 0xC000)) || (_prgMode == 0x3)) {
            uint8_t bankRegisterNumber = 1 + ((address >> 13) & 0x3);
            bool isPrgRam = ((_prgBankSwitch[bankRegisterNumber] & 0x80) == 0) && (address < 0xE000);
            
            // Write Prg-Ram
            if (isPrgRam) {
                this->writePrgRam(((_prgBankSwitch[bankRegisterNumber] & 0xF) << 13) | (address & 0x1FFF), data);
            }
        }
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Chr-Rom
    if (address < 0x2000) {
        // Calculate bank register number
        uint8_t bankRegisterNumber = ((1 << (3 - _chrMode)) * ((address >> (13 - _chrMode)) + 1)) - 1;  // TODO: voir si mieux par calcul ou par mémoire (tableau avec les valeurs) mais normalement ici on sera souvent en cache miss puisque c'est le PPU qui lit et apres il fait des operations (comme plotpixel) qui vont devoir charger aussi en mémoire mais bon c'est a voir avec les caches lines
        
        // Correct bank register number if 8x16 mode and fetch background tiles
        if ((is8x16SpriteMode()) && (!isPPUFetchSpriteData())) {
            bankRegisterNumber = (bankRegisterNumber & 0x3) + 0x8;
        }
        
        // Calculate mask
        uint16_t mask = 0x1FFF >> _chrMode;
        
        // Read Chr-rom
        ppuHardwareInterface.setDataBus(this->readChrRom((_chrBankSwitch[bankRegisterNumber] << (13 - _chrMode)) | (address & mask)));
    }
    // Internal VRAM (PPU address is always < 0x4000)
    else {
        // Process scanline detection
        processScanlineDetection(address);
        
        // Get mapping mode
        uint8_t mappingMode = (_nametableMapping >> ((address >> 9) & 0x6)) & 0x3;
        
        // On-board VRAM
        if (mappingMode < 2) {
            ppuHardwareInterface.setDataBus(ppuHardwareInterface.getVram()[(mappingMode << 12) | (address & 0xFFF)]);
        }
        // Internal Expansion RAM
        else if (mappingMode == 2) {
            // Only read in expansion RAM if mode allows it, else read 0
            ppuHardwareInterface.setDataBus((_extendedRamMode < 2) ? _ram[address & 0x3FF] : 0);//TODO: pas sur car il y a peut etre une difference entre le mode nametable et le mode attribute data
        }
        // Fill-mode
        else {
            ppuHardwareInterface.setDataBus(((address & 0x3C0) != 0x3C0) ? _fillModeTile : _fillModeColor);//TODO: voir si ok la condition
        }
    }
    
    // Increment PPU fetch counter TODO: voir si incremente sur toutes les address ou sur seulement >= 0x2000 (nametable / attribute table)
    ++_ppuFetchCounter;//TODO: probleme : si c'est le CPU qui fetche via un read 2007 ???
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) {
    // Get address
    uint16_t address = ppuHardwareInterface.getAddressBus();
    
    // Get data
    uint8_t data = ppuHardwareInterface.getDataBus();
    
    // Nothing for Chr-Rom (Can't write to a ROM)   // TODO: voir si possible chr-ram avec MMC5?
    // Internal VRAM (PPU address is always < 0x4000)
    if (address >= 0x2000) {
        // Get mapping mode
        uint8_t mappingMode = (_nametableMapping >> ((address >> 9) & 0x6)) & 0x3;
        
        // On-board VRAM
        if (mappingMode < 2) {
            ppuHardwareInterface.getVram()[(mappingMode << 12) | (address & 0xFFF)] = data;
        }
        // Internal Expansion RAM
        else if (mappingMode == 2) {
            // Only write in expansion RAM if mode allows it
            if (_extendedRamMode < 2) {//TODO: ok, on peut ecrire la ram ainsi ?
                _ram[address & 0x3FF] = data;//TODO: pas sur car il y a peut etre une difference entre le mode nametable et le mode attribute data
            }
        }//TODO: rien d'écrit en fill mode ?
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
bool Chip<TCpuHardwareInterface, TPpuHardwareInterface>::is8x16SpriteMode() const {
    return _ppu2000Bit5 && (_ppu2001Bit3 || _ppu2001Bit4);
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
bool Chip<TCpuHardwareInterface, TPpuHardwareInterface>::isPPUFetchSpriteData() const {
    return (_ppuFetchCounter > 128) && (_ppuFetchCounter <= 160);//TODO: voir les valeurs
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::processScanlineDetection(uint16_t address) {
    // Set PPU is reading flag
    _ppuIsReading = true;
    
    // Get last PPU read address
    uint16_t lastAddress = _lastPPUReadAddress;
    
    // Save current address as last PPU read address
    _lastPPUReadAddress = lastAddress;
    
    // If address doesn't match last read address or if it is not a PPU nametable fetch (Here the address can't be less than 0x2000, so we need to check only if greater than 0x2FFF)
    if ((address != lastAddress) || (address >= 0x3000)) {
        // Reset counter
        _ppuReadAddressConsecutiveCounter = 0;
        
        // Exit
        return;
    }
    
    // Increment counter
    ++_ppuReadAddressConsecutiveCounter;
    
    // If three consecutive read on same address (counter equals 2 because we start to count after first read)
    if (_ppuReadAddressConsecutiveCounter == 2) {
        // Not the first scanline
        if (_inFrame) {
            // Increment scanline counter
            ++_scanlineCounter;
            
            // If reached the compare value
            if (_scanlineCounter == _scanlineIrqCompareValue) {
                // Set scanline IRQ pending flag
                _scanlineIrqPending = true;
            }
        }
        // First scanline
        else {
            // Reset scanline counter
            _scanlineCounter = 0;
            
            // Reset PPU fetch counter
            _ppuFetchCounter = 0;
            
            // Set in frame flag
            _inFrame = true;
        }
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::checkPPUIsRendering() {
    // If PPU read value
    if (_ppuIsReading) {
        // Reset CPU cycle counter
        _cpuWithoutPPUReadCycleCounter = 0;
        
        // Clear PPU read flag
        _ppuIsReading = false;
        
        // Exit
        return;
    }
    
    // Increment CPU cycle counter
    ++_cpuWithoutPPUReadCycleCounter;
    
    // If reached 3 CPU cycle without PPU read
    if (_cpuWithoutPPUReadCycleCounter == 3) {  // TODO: il peut faire un overflow et refaire ca mais ce n'est pas grave vu qu'il va reset le inFrame qui doit etre deja a false (une fois qu'il est a true, _ppuIsReading sera a true aussi et on ne sera pas ici) mais a voir quand meme
        // Clear in frame flag
        _inFrame = false;
        
        // TODO: reset le irq ici aussi ??? mais alors faire attention au commentaire ci dessus
    }
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Chip<TCpuHardwareInterface, TPpuHardwareInterface>::checkInterrupt(TCpuHardwareInterface &cpuHardwareInterface) {
    // Check scanline IRQ
    bool scanlineIrq = _scanlineIrqPending && _scanlineIrqEnabled;
    
    // Check timer IRQ
    bool timerIrq = false;//TODO: a terminer
    
    // Set IRQ signal on CPU (inverted signal)
    cpuHardwareInterface.irq(!(scanlineIrq || timerIrq));
}

#endif /* NESEmu_Cartridge_Mapper5_s_hpp */
