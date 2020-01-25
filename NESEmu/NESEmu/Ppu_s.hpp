//
//  Ppu_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 23/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Ppu_s_hpp
#define NESEmu_Ppu_s_hpp


template <>
struct Constants<Model::Ricoh2C02> {//TODO: + voir overscan (normalement c'est 224 scanline la frame visible en NTSC)
    static constexpr int visiblePixelsPerScanlineCount = 256;
    static constexpr int hBlankLengthInPixel = 85;
    static constexpr int visibleScanlinePerFrameCount = 240;
    static constexpr int postRenderLengthInScanline = 1;
    static constexpr int vBlankLengthInScanline = 20;
};

template <>
struct Constants<Model::Ricoh2C07> {
    static constexpr int visiblePixelsPerScanlineCount = 256;
    static constexpr int hBlankLengthInPixel = 85;
    static constexpr int visibleScanlinePerFrameCount = 240;
    static constexpr int postRenderLengthInScanline = 1;
    static constexpr int vBlankLengthInScanline = 70;
};


template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::Chip(TBus &bus, TInterruptHardware &interruptHardware, TGraphicHardware &graphicHardware) : _objectAttributeMemory(64), _bus(bus), _interruptHardware(interruptHardware), _graphicHardware(graphicHardware) {
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::clock() {
    // TODO: ne pas oublier de mettre _statusVBlankStarted a false a la fin du vblank (quand on calcule le scanline pre-render) et de le mettre a true au debut du vblank
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::reset(bool high) {
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::read(uint16_t address) {
    // Status register
    if (address == 0x0002) {    // TODO: constantes a la place des valeurs ?
        // Least significant bits previously written into a PPU register (due to register not being updated for this address)
        _dataLatch &= 0x1F;
        
        // Update register with status
        _dataLatch |= _statusSpriteOverflow << 5;
        _dataLatch |= _statusSprite0Hit << 6;
        _dataLatch |= _statusVBlankStarted << 7;
        
        //_status = _dataLatch;
        
        // Reset VBlank started flag
        _statusVBlankStarted = false;
        
        // Check for interrupt
        checkInterrupt();
        
        // Reset write toggle
        _writeToggle = false;
    }
    // OAM data register
    else if (address == 0x0004) {
        _dataLatch = _objectAttributeMemory[_oamAddress];
    }
    // Data register
    else if (address == 0x0007) {
        _dataLatch = _data; // TODO: on doit lire a l'adresse _address (via des fonctions read / write qui seront gérées par le cartridge), si on lit en open bus behavior, on doit retourner address & 0xFF car le bus de data est multiplexé avec le bus d'adresse low, voir : https://wiki.nesdev.com/w/index.php/Open_bus_behavior
        
        // Increment address
        _address += _controlAddressIncrementPerCpuAccess;
    }
    
    // dataLatch is set with read value or previous dataLatch value if read a write only register
    return _dataLatch;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::write(uint16_t address, uint8_t data) {
    // When data is written, data latch is filled with the value even if the write occurs on a read-only register
    _dataLatch = data;
    
    // Control register
    if (address == 0x0000) {
        //_control = data;
        
        _temporaryAddress = (_temporaryAddress & 0x73FF) | (data & 0x3) << 10;
        _controlAddressIncrementPerCpuAccess = ((data & 0x4) != 0) ? 32 : 1;//((data & 0x4) << 3) | (((data & 0x4) >> 2) ^ 0x1);
        _controlSpritePatternTableAddress = (data & 0x8) << 9;
        _controlBackgroundPatternTableAddress = (data & 0x10) << 8;
        _controlSpriteSize8x16px = data & 0x20;
        _controlMasterSlaveSelect = data & 0x40;
        
        bool oldControlGenerateNmiForVBlank = _controlGenerateNmiForVBlank;
        _controlGenerateNmiForVBlank = data & 0x80;
        
        // Check for interrupt if generateNmiForVBlank changed
        if (oldControlGenerateNmiForVBlank != _controlGenerateNmiForVBlank) {   // TODO: voir si ainsi
            checkInterrupt();
        }
        /*
        // Generate immediatly a NMI if PPU is in VBlank and if generate nmi for VBlank is enabled
        if ((_statusVBlankStarted == true) && (oldControlGenerateNmiForVBlank == false) && (_controlGenerateNmiForVBlank == true)) {    // TODO: voir si statusVBlankStarted est suffisant pour le test (et pas checker le numero de la ligne courante pour etre sur qu'on est en vblank)
            checkInterrupt();
        }*/
    }
    // Mask register
    else if (address == 0x0001) {
        //_mask = data;
        
        _maskGrayscale = data & 0x1;
        _maskShowBackgroundFirst8px = data & 0x2;
        _maskShowSpritesFirst8px = data & 0x4;
        _maskShowBackground = data & 0x8;
        _maskShowSprites = data & 0x10;
        _maskEmphasizeRed = data & 0x20;
        _maskEmphasizeGreen = data & 0x40;
        _maskEmphasizeBlue = data & 0x80;
    }
    // OAM address register
    else if (address == 0x0003) {
        _oamAddress = data;
        
        // TODO: voir pour les bugs (2C02 seulement)
    }
    // OAM data register
    else if (address == 0x0004) {
        // Write data to OAM
        _objectAttributeMemory[_oamAddress] = data;
        
        // Increment OAM address
        ++_oamAddress;
        
        // TODO: apparement, ne peut etre ecrit que pendant le vblank !!!! : https://wiki.nesdev.com/w/index.php/PPU_registers
    }
    // Scroll register
    else if (address == 0x0005) {
        //_scroll = data;
        
        // First write
        if (_writeToggle == false) {
            _temporaryAddress = (_temporaryAddress & 0x7FE0) | (data >> 3);
            _fineXScroll = data & 0x7;
        }
        // Second write
        else {
            _temporaryAddress = (_temporaryAddress & 0xC1F) | ((data & 0x7) << 12) | ((data & 0xF8) << 2);
        }
        
        // Toggle write
        _writeToggle = !_writeToggle;
    }
    // Address register
    else if (address == 0x0006) {
        //_address = data;
        
        // First write
        if (_writeToggle == false) {
            _temporaryAddress = (_temporaryAddress & 0xFF) | ((data & 0x3F) << 8);
        }
        // Second write
        else {
            _temporaryAddress = (_temporaryAddress & 0x7F00) | data;
            _address = _temporaryAddress;
        }
        
        // Toggle write
        _writeToggle = !_writeToggle;
    }
    // Data register
    else {
        _data = data;   // TODO: on doit ecrire a l'adresse _address (via des fonctions read / write qui seront gérées par le cartridge)
        
        // Increment address
        _address += _controlAddressIncrementPerCpuAccess;
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::checkInterrupt() {
    // See https://wiki.nesdev.com/w/index.php/NMI
    _interruptHardware.ppuInterrupt((_statusVBlankStarted == true) && (_controlGenerateNmiForVBlank == true));
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isRenderingEnabled() const {
    return _maskShowBackground | _maskShowSprites;
}

#endif /* NESEmu_Ppu_s_hpp */
