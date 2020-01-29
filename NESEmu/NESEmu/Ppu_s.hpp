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
Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::Chip(TBus &bus, TInterruptHardware &interruptHardware, TGraphicHardware &graphicHardware) : _objectAttributeMemory(/*64*/256), _secondObjectAttributeMemory(/*8*/32), _paletteIndexMemory(32), _bus(bus), _interruptHardware(interruptHardware), _graphicHardware(graphicHardware) {
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::clock() {
    // Idle on first pixel
    if (_currentPixel > 0) {
        // Visible frame line
        if (_currentScanline < Constants::visibleScanlinePerFrameCount) {
            processVisibleFrameLine();
        }
        // Post-render line
        else if (_currentScanline < (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline)) {
            processPostRenderLine();
        }
        // VBlank line
        else if (_currentScanline < (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline + Constants::vBlankLengthInScanline)) {
            processVBlankLine();
        }
        // Pre-render line
        else {
            processPreRenderLine();
        }
    }
    
    // Increment position counters
    incrementPositionCounters();    // TODO: par apres il est possible de faire une petite optimisation car dedans il recheck la position du scanline courant pour voir si on a fini une frame, mais on fait deja des checks ici et on peut etre sur que si on est pas dans la pre-render line qu'on a pas fini la frame
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
        checkInterrupt();   // TODO: peut etre pas ici mais a la fin du clock, voir le bug quand lu avant 1 cycle du VBL pendant et 1 cycle apres le VBL
        
        // Reset write toggle
        _writeToggle = false;
    }
    // OAM data register
    else if (address == 0x0004) {
        // If attributes byte
        if (_oamAddress & 0x2) {
            _dataLatch &= 0x1C;     // TODO: on ne sait pas vraiment si les lignes inutilisées sont tirées vers le bas par le PPU ou si c'est un effet de l'open bus, voir ici a la section byte 2 : https://wiki.nesdev.com/w/index.php/PPU_OAM
            // TODO: un moyen de tester est d'arriver a changer la valeur du data latch juste apres le fetch de l'address high 0x2004 (0x20) : mais ca n'a rien avoir avec le PPU car c'est l'open bus du cpu les instructions !!! a voir, pour tester peut etre arriver a faire le test dit juste au dessus pour etre sur que ca n'a rien avoir avec le open bus du cpu et si ok essayer de lire 2002 avant par exemple puis 2004
            
            // Get data
            _dataLatch |= _objectAttributeMemory[_oamAddress] & 0xE3;
        }
        // Other bytes
        else {
            // Get data
            _dataLatch = _objectAttributeMemory[_oamAddress];
        }
        
        // When we are in second OAM clear period, read must return 0xFF
        _dataLatch |= _clearSecondOAMReadOverwrite;
    }
    // Data register
    else if (address == 0x0007) {
        // Save data read buffer
        uint8_t oldDataReadBuffer = _dataReadBuffer;
        
        // If bad address, open bus behaviour return low address because it is multiplexed with data bus
        // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
        _dataReadBuffer = _address & 0xFF;  // TODO: surement mettre ca dans le read du bus et pas ici !!!
        
        // Read data from PPU bus
        _dataReadBuffer = _bus.read(_address); // TODO: on doit lire a l'adresse _address (via des fonctions read / write qui seront gérées par le cartridge)
        
        // Read return the content of the data read buffer, data read buffer is then updated with the read value
        _dataLatch = (_address < 0x3F00) ? oldDataReadBuffer : _paletteIndexMemory[_address & 0x1F];
        
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
            checkInterrupt();// TODO: peut etre pas ici mais a la fin du clock, a voir
        }
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
        // Write data to PPU bus
        _bus.write(_address, data); // TODO: on doit ecrire a l'adresse _address (via des fonctions read / write qui seront gérées par le cartridge)
        
        // If palette index address, write to it    TODO: gerer la palette, est ce qu'on ecrit aussi via le bus a l'adresse (ligne d'au dessus) ou juste la palette
        if (_address > 0x3EFF) {
            _paletteIndexMemory[_address & 0x1F] = data;
        }
        
        // Increment address
        _address += _controlAddressIncrementPerCpuAccess;
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::exts(uint8_t data) {
    // TODO: a faire
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::getExts() const {
    return 0; // TODO: changer
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processVisibleFrameLine() {
    fetchTilesData();
    evaluateSprites();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processPostRenderLine() {
    // Nothing happens in post-render line
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processVBlankLine() {
    // Set VBlank flag if necessary
    if ((_currentScanline == (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline)) && (_currentPixel == 1)) { // TODO: changer 1 par des constants de Constants::
        _statusVBlankStarted = true;    // TODO: voir le bug quand lu avant 1 cycle du VBL pendant et 1 cycle apres le VBL
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processPreRenderLine() {
    // Clear flags if necessary
    if (_currentPixel == 1) {   // TODO: changer 1 par des constants de Constants::
        _statusSpriteOverflow = false;
        _statusSprite0Hit = false;
        _statusVBlankStarted = false;
    }
    
    fetchTilesData();
    
    // Check for Y address copy // TODO: avec isRenderingEnabled
    if ((_currentPixel >= 280) && (_currentPixel <= 304)) { // TODO: changer 280 et 304 par des constants de Constants::
        _address = (_address & 0x41F) | (_temporaryAddress & 0x7BE0);
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::incrementPositionCounters() {
    // Increment pixel counter
    ++_currentPixel;
    
    // Check boundary of pixel counter
    if (_currentPixel < (Constants::visiblePixelsPerScanlineCount + Constants::hBlankLengthInPixel)) {
        return;
    }
    
    // Start a new line, reset pixel counter and increment scanline counter
    _currentPixel = 0;
    ++_currentScanline;
    
    // Check boundary of scanline counter
    if (_currentScanline < (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline + Constants::vBlankLengthInScanline + 1)) { //TODO: changer 1 par des constants de Constants::
        return;
    }
    
    // Start a new frame, reset scanline counter and toggle even/odd frame
    _currentScanline = 0;
    _oddFrame = !_oddFrame;
    
    // If start a new odd frame, skip first cycle
    _currentPixel += (_oddFrame & isRenderingEnabled());     // TODO: voir si c bon
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::fetchTilesData() {   // TODO: surement créer une autre fonction qui appelle celle ci pour retirer tout ce qui n'est pas fetch ici
    // No VRAM access if rendering is disabled
    if (isRenderingEnabled() == false) {
        return;
    }
    
    // Complete tile data is 8 cycles
    uint8_t dataType = _currentPixel & 0x7;
    
    // Nametable set address
    if (dataType == 0x1) {
        _addressBus = 0x2000 | (_address & 0x0FFF);
    }
    // Nametable fetch (NT byte)
    else if (dataType == 0x2) {
        _ntByte = _bus.read(_addressBus); // TODO: a voir (mettre ca dans un _nextNTByte ?)
    }
    // Attributes set address
    else if (dataType == 0x3) {
        _addressBus = 0x23C0 | (_address & 0x0C00) | ((_address >> 4) & 0x38) | ((_address >> 2) & 0x7);
    }
    // Attributes fetch (AT byte)
    else if (dataType == 0x4) {
        _atByte = _bus.read(_addressBus);    // TODO: a voir
    }
    // First pattern table set address
    else if (dataType == 0x5) {
        // One tile is 16 bytes (ntByte << 4), one byte is a 8 pixels row (fineY = _address >> 12)
        _addressBus = _controlBackgroundPatternTableAddress | (_ntByte << 4) | (_address >> 12);
    }
    // First pattern table fetch (Low BG tile byte)
    else if (dataType == 0x6) {
        _lowBGTileByte = _bus.read(_addressBus);
    }
    // Second pattern table set address
    else if (dataType == 0x7) {
        // Same than low BG tile address but 8 bytes after
        _addressBus = _controlBackgroundPatternTableAddress | (_ntByte << 4) | (_address >> 12) | 0x8;
    }
    // Second pattern table fetch (High BG tile byte)
    else {
        _highBGTileByte = _bus.read(_addressBus);
    }
    
    // TODO: ici on doit gérer le rendu car apres on shift les registres
    // TODO: ca va etre un genre de : uint8_t pixelIndexed = (_highBGTileShiftRegister >> (14 - _fineXScroll)) & 0x2 | (_lowBGTileShiftRegister >> (15 - _fineXScroll)) & 0x1 // Avec 14 = (7 - _fineXScroll) + 7 -> 7 pour inverser le _fineXScroll et 7 pour décaler a droite en gardant le 2eme bit et avec 15 = (7 - _fineXScroll) + 8 -> 7 pour inverser le _fineXScroll et 8 pour décaler a droite en gardant le 1er bit : OU plus simplement dit 14 pour le 15eme bit et 15 pour le 16eme bit (shift)
    
    // TODO: voir car dans le png de timing ils disent qu'on charge le low byte et dans le wiki ils disent qu'on charge le high byte dans les shifts
    // Check for shifting registers
    if (((_currentPixel >= 2) && (_currentPixel <= (Constants::visiblePixelsPerScanlineCount + 1))) || ((_currentPixel >= 322) && (_currentPixel <= 337))) {
        _lowBGTileShiftRegister <<= 1;
        _highBGTileShiftRegister <<= 1;
    }
    
    // Update BGs tile shift registers
    if ((dataType == 0x1) && (((_currentPixel >= 9) && (_currentPixel <= (Constants::visiblePixelsPerScanlineCount + 1))) || ((_currentPixel >= 329) && (_currentPixel <= 337)))) {
        _lowBGTileShiftRegister = (_lowBGTileShiftRegister & 0xFF00) | _lowBGTileByte;      // TODO: le & 0xFF00 n'est pas nécessaire car c'est shifté 8x et donc il y a 8x 0 au low byte
        _highBGTileShiftRegister = (_highBGTileShiftRegister & 0xFF00) | _highBGTileByte;   // TODO: pareil
    }
    
    // Check for X address increment
    if ((dataType == 0x0) && ((_currentPixel <= Constants::visiblePixelsPerScanlineCount) || (_currentPixel >= 328))) {//TODO: changer 328 par des constants de Constants::
        incrementXOnAddress();
    }
    
    // Check for Y address increment
    if (_currentPixel == Constants::visiblePixelsPerScanlineCount) {
        incrementYOnAddress();
    }
    
    // Check for X address copy
    if (_currentPixel == (Constants::visiblePixelsPerScanlineCount + 1)) {
        _address = (_address & 0x7BE0) | (_temporaryAddress & 0x41F);
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::evaluateSprites() {
    // Secondary OAM clear start
    if (_currentPixel == 1) {
        _clearSecondOAMReadOverwrite = 0xFF;
    }
    // Sprite evaluation for next scanline start
    else if (_currentPixel == (64 + 1)) {
        _clearSecondOAMReadOverwrite = 0x0;
    }
    
    // Sprite evaluation / second OAM clear
    if (_currentPixel < (Constants::visiblePixelsPerScanlineCount + 1)) {
        // TODO: a faire, voir si lire via read (et du coup on affecte _dataLatch, ou en direct (mais alors gérer 0xFF) ?
    }
    /*
    // Secondary OAM clear
    if (_currentPixel < (64 + 1)) {   //TODO: changer 64 par des constants de Constants::
        _secondObjectAttributeMemory[_currentPixel - 1] = 0xFF;    // TODO: quand on lit 2004 pendant cette periode, on doit retourner 0xFF, voir ici : https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation
    }
    // Sprite evaluation for next scanline
    else if (_currentPixel < (Constants::visiblePixelsPerScanlineCount + 1)) {
        // TODO: sprite evaluation
    }*/
    
    // Nothing for the remaining cycles, sprite data fetch is the same than tile data fetch
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::incrementXOnAddress() {
    _address = ((_address & ~0x1F) | ((_address + 1) & 0x1F)) ^ ((((_address & 0x1F) + 1) & 0x20) << 5);    // TODO: utiliser ~ sur les autres calculs aussi ?
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::incrementYOnAddress() {  // TODO: voir si plus optimisé
    // Fine Y scroll doesn't carry out
    if ((_address & 0x7000) != 0x7000) {
        // Increment fine Y and exit
        _address += 0x1000;
        return;
    }
    
    // Get coarse Y scroll in address
    uint16_t coarseYScrollInAddress = _address & 0x03E0;
    
    // If reach end of current name table
    if (coarseYScrollInAddress == 0x03A0) {
        // Reset it and toggle name table
        coarseYScrollInAddress = 0;
        _address ^= 0x800;
    }
    // If reach end of address space
    else if (coarseYScrollInAddress == 0x03E0) {
        // Just reset it
        coarseYScrollInAddress = 0;
    }
    // No carry out, just increment coarse Y
    else {
        coarseYScrollInAddress += 0x20;
    }
    
    // Correct address
    _address = (_address & ~0x73E0) | coarseYScrollInAddress;
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
