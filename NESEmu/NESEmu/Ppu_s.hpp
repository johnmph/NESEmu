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
Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::Chip(TBus &bus, TInterruptHardware &interruptHardware, TGraphicHardware &graphicHardware) : _bus(bus), _interruptHardware(interruptHardware), _graphicHardware(graphicHardware), _objectAttributeMemory(/*64*/256), _secondObjectAttributeMemory(/*8*/32), _paletteIndexMemory(32) {
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::powerUp() {
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
    // TODO: voir ici pour les registres remis a 0 : https://wiki.nesdev.com/w/index.php/PPU_pin_out_and_signal_description
    // https://wiki.nesdev.com/w/index.php/PPU_power_up_state
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
        
        // Reset VBlank started flag
        _statusVBlankStarted = false;
        
        // Check for interrupt
        checkInterrupt();   // TODO: peut etre pas ici mais a la fin du clock, voir le bug quand lu avant 1 cycle du VBL pendant et 1 cycle apres le VBL TESTER AVEC Visual2C02
        
        // Reset write toggle
        _writeToggle = false;
    }
    // OAM data register
    else if (address == 0x0004) {
        // Read OAM
        readObjectAttributeMemory();
        
        // Get data
        // After doing some tests on Visual2C02, it seems that PPU drives unused bits low and there is no open bus behaviour implicated
        // Test : testprogram = [ 0x2000, 0x2100, 0x2300, 0x24FF, 0x0018, 0x24FF, 0x0018, 0x24AA, 0x0018, 0x24FF, 0x0018, 0x2302, 0x2514, 0x3400];
        _dataLatch = _oamData;
    }
    // Data register
    else if (address == 0x0007) {
        // Save data read buffer
        uint8_t oldDataReadBuffer = _dataReadBuffer;
        
        // If bad address, open bus behaviour return low address because it is multiplexed with data bus
        // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
        _dataReadBuffer = _address & 0xFF;  // TODO: surement mettre ca dans le read du bus et pas ici !!! : OUI car on read le bus ailleurs qu'ici pour fetcher les tiles/sprites !!! et ca doit aussi faire cet effet !!! : mais plutot dans une methode readVRAM d'ici et pas dans le bus mais dans le bus read il faudrait qu'il retourne une paire de uin8_t : la valeur et le masque (si pas de mémoire lue, mask = 0 sinon mask = FF et on doit faire : _addressBus = (_addressBus & 0xFF00) | (_addressBus & 0xFF & ~readMask) | (readData & readMask);
        
        // Read data from PPU bus
        _dataReadBuffer = _bus.read(_address); // TODO: on doit lire a l'adresse _address (via des fonctions read / write qui seront gérées par le cartridge)
        
        // If palette index address, read to it directly ( see https://wiki.nesdev.com/w/index.php/PPU_registers#PPUDATA )
        if (_address > 0x3EFF) {
            // Addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
            uint8_t paletteAddress = _address & (((_address & 0xFF13) == 0x3F10) ? 0xF : 0x1F);
            
            // Read from palette memory
            _dataLatch = _paletteIndexMemory[paletteAddress];
        } else {
            // Read return the content of the data read buffer, data read buffer is then updated with the read value
            _dataLatch = oldDataReadBuffer;
        }
        
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
        // Can only be written during VBlank, see https://wiki.nesdev.com/w/index.php/PPU_registers
        if ((isRenderingEnabled() == true) && (_currentScanline < (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline))) {
            // TODO: normalement ca fait des incrementations bizarres de oamAddress
            return;
        }
        
        // Write data to OAM
        writeObjectAttributeMemory(data);
        
        // Increment OAM address
        ++_oamAddress;
    }
    // Scroll register
    else if (address == 0x0005) {
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
        // If palette index address, write to it    TODO: gerer la palette, est ce qu'on ecrit aussi via le bus a l'adresse ou juste la palette ??? : normalement juste la palette car sinon des qu'on veut updater la palette on va ecrire dans les mirroirs de la nametable aussi !!!
        if (_address > 0x3EFF) {
            // Addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
            uint8_t paletteAddress = _address & (((_address & 0xFF13) == 0x3F10) ? 0xF : 0x1F);
            
            // Write to palette memory
            _paletteIndexMemory[paletteAddress] = data;
        } else {
            // Write data to PPU bus
            _bus.write(_address, data); // TODO: on doit ecrire a l'adresse _address (via des fonctions read / write qui seront gérées par le cartridge)
        }
        
        // Increment address
        _address += _controlAddressIncrementPerCpuAccess;
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::exts(uint8_t data) {
    // TODO: a faire, voir : https://wiki.nesdev.com/w/index.php/Talk:PPU_pin_out_and_signal_description
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::getExts() const {
    return 0; // TODO: changer
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processVisibleFrameLine() {
    uint8_t pixelIndexedColor;
    
    // Only VRAM access if rendering is enabled
    if (isRenderingEnabled() == true) {
        // Complete tile data is 8 cycles
        uint8_t dataType = _currentPixel & 0x7;
        
        // Fetch tiles data
        fetchTilesData(dataType);
        
        // Process sprites
        processSprites(dataType);
        
        // Calculate pixel
        pixelIndexedColor = calculatePixel();
        
        // Update shift registers
        updateShiftRegisters(dataType);
        
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
    } else {
        // Rendering disabled, use background color
        pixelIndexedColor = 0;
    }
    
    // Get color from index
    uint8_t pixelColor = getColorFromPalette(pixelIndexedColor);    // TODO: normalement il y a un delai de 2 cycles pour récuperer la couleur et plotter le pixel courant !!!
    
    // Plot pixel
    _graphicHardware.plotPixel(_currentPixel, _currentScanline, pixelColor);
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
        
        // Notify hardware that the VBlank has started
        _graphicHardware.notifyVBlankStarted();
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
    
    // Only VRAM access if rendering is enabled
    if (isRenderingEnabled() == true) {
        // Fetch tiles / sprites data
        fetchTilesData(_currentPixel & 0x7);
        
        // Check for Y address copy
        if ((_currentPixel >= 280) && (_currentPixel <= 304)) { // TODO: changer 280 et 304 par des constants de Constants::
            _address = (_address & 0x41F) | (_temporaryAddress & 0x7BE0);
        }
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
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::fetchTilesData(uint8_t dataType) {
    // Nametable set address
    if (dataType == 0x1) {
        _addressBus = 0x2000 | (_address & 0x0FFF);
    }
    // Nametable fetch (NT byte)
    else if (dataType == 0x2) {
        _ntByte = _bus.read(_addressBus);
    }
    // Attributes set address
    else if (dataType == 0x3) {
        _addressBus = 0x23C0 | (_address & 0x0C00) | ((_address >> 4) & 0x38) | ((_address >> 2) & 0x7);
    }
    // Attributes fetch (AT byte)
    else if (dataType == 0x4) {
        _atByte = _bus.read(_addressBus);
    }
    // First pattern table set address
    else if (dataType == 0x5) {
        // One tile is 16 bytes (ntByte << 4), one byte is a 8 pixels row (fineY = _address >> 12)
        _addressBus = _controlBackgroundPatternTableAddress | (_ntByte << 4) | (_address >> 12);    // TODO: pour les sprites c'est _controlSpritePatternTableAddress mais attention pour les 8x16 sprites c'est différent, donc peut etre que je ne peux pas utiliser  fetchTilesData pour fetcher les sprites, a voir
    }
    // First pattern table fetch (Low tile byte)
    else if (dataType == 0x6) {
        _lowTileByte = _bus.read(_addressBus);
    }
    // Second pattern table set address
    else if (dataType == 0x7) {
        // Same than low BG tile address but 8 bytes after
        _addressBus = _controlBackgroundPatternTableAddress | (_ntByte << 4) | (_address >> 12) | 0x8;  // TODO: pareil qu'au dessus
    }
    // Second pattern table fetch (High tile byte)
    else {
        _highTileByte = _bus.read(_addressBus);
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processSprites(uint8_t dataType) {
    // Exit if we are no more in process sprites phase (Nothing for the remaining cycles, sprite data fetch is the same than tile data fetch)
    if (_currentPixel > 320) {// TODO: remplacer 320 par des constants de Constants:: // TODO: voir si compilateur optimise en ne faisant pas l'appel de la methode si on a cette condition (comme s'il sortait ce if a l'exterieur de cette methode, a l'endroit ou on appelle cette methode
        return;
    }
    
    // Secondary OAM clear start
    if (_currentPixel == 1) {
        _clearSecondOAMReadOverwrite = 0xFF;
    }
    // Secondary OAM clear stop
    else if (_currentPixel == (64 + 1)) {           // TODO: ceci et aussi le if d'au dessus ne sont pas appelé si rendering est desactivé, que se passe t'il si on laisse le second OAM clear commencer puis qu'on désactive le render avant que le secondOAM clear se termine, ici ca va tjs retourner FF quand on lira OAM data meme si on est plus en secondary OAM clear, voir si ainsi en vrai dans visual2C02 !!! -> PAS BON, des que le rendu est activé, si on est tjs dans les processSprites cycles ca doit remettre ca a 0 donc pas un == !
        _clearSecondOAMReadOverwrite = 0x0;
    }
    
    // If we are in secondary OAM clear phase
    if (_currentPixel < (64 + 1)) {   //TODO: changer 64 par des constants de Constants:: TODO: pareil qu'au dessus, voir si desactiver le rendu pendant ceci va stopper le secondary OAM
        clearSecondaryOAM();
        return;
    }
    
    // If we are in sprite evaluation phase
    if (_currentPixel < (Constants::visiblePixelsPerScanlineCount + 1)) {   // TODO: pareil qu'au dessus
        evaluateSprites(dataType);      // TODO: voir si besoin de dataType pour les sprites et le virer si pas besoin
        return;
    }
    
    // OAMADDR is set to 0 during each of ticks 257-320 (the sprite tile loading interval) of the pre-render and visible scanlines
    _oamAddress = 0x0;  // TODO: tester si vrai dans visual2C02
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isInSecondaryOAMClearPeriod() {
    // TODO: retourne true si on est dans la periode mais pas obligé la phase (car on est dans la période si on est aussi dans le pre-render mais ca ne le fait pas, ca retourne juste les FF)
    // TODO: normalement entre le pixel 1 et 65 (voir si pixel 0 quand meme ou non)
    // TODO: normalement entre le scanline 0 et 239 + le 261 (voir si autre quand meme ou non)
    return (_currentScanline );
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::readObjectAttributeMemory() {
    // If we are in secondary OAM phase and rendering is enabled, we must read $FF instead of real data
    _oamData = ((isRenderingEnabled() == false) || (_currentScanline)) ? _objectAttributeMemory[_oamAddress] : 0xFF;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::writeObjectAttributeMemory(uint8_t data) {
    // If we are in secondary OAM phase and rendering is enabled, we must write $FF instead of data
    _oamData = () ? data : 0xFF;
    
    // Write data to OAM (if attribute byte, need to AND with $E3 because bit 2, 3 and 4 are unused)
    // It's the correct behaviour according to Visual2C02 tests, it could be optimized by doing the IF and AND in the read method instead (it's more likely to be written that to be read) but let it like true behaviour
    _objectAttributeMemory[_oamAddress] = (_oamAddress & 0x2) ? (_oamData & 0xE3) : _oamData;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::clearSecondaryOAM() {
    // Ready cycle
    if (_currentPixel & 0x1) {
        readOAM();
        
        return;
    }
    
    // Write cycle
    _secondObjectAttributeMemory[_secondOamAddress] = _oamData;
    
    // Increment second OAM address
    ++_secondOamAddress;    // TODO: tester dans Visual2C02 pour savoir quand remis a 0
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::evaluateSprites(uint8_t dataType) {
    // TODO : Because OAM is implemented with dynamic RAM instead of static RAM, the data stored in OAM memory will quickly begin to decay into random bits if it is not being refreshed. The OAM memory is refreshed once per scanline while rendering is enabled (if either the sprite or background bit is enabled via the register at $2001), but on an NTSC PPU this refresh is prevented whenever rendering is disabled.
    // TODO: quand et comment fait il ca ? 256 read suivi de write ? de 1 a 256 (inclus) ??
    
    // TODO se fait en 192 cycles PPU (de 65 a 256 inclus), il y a 8 sprites a fetcher + au moins 1 pour le sprite overflow, il faut un cycle de lecture et un cycle d'ecriture ce qui fait 96 cycles chacun, il y a 4 bytes par sprites, ce qui fait 24 sprites max qu'on peut fetcher ? -> sauf que si le sprite Y n'est pas dans le scanline courant, on ne copie pas les 3 bytes restants, donc on sauve 3 * 2 cycles = 6 cycles
    
    // Odd cycle, read data
    if (_currentPixel & 0x1) {
        read(0x0004);   // TODO: pas bon, on ne passe pas par read mais en direct normalement, a confirmer
        return;
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::updateShiftRegisters(uint8_t dataType) {
    // TODO: voir car dans le png de timing ils disent qu'on charge le low byte et dans le wiki ils disent qu'on charge le high byte dans les shifts
    // Check for shifting registers
    if (((_currentPixel >= 2) && (_currentPixel <= (Constants::visiblePixelsPerScanlineCount + 1))) || ((_currentPixel >= 322) && (_currentPixel <= 337))) {
        // Shift registers
        _bgLowTileShiftRegister <<= 1;
        _bgHighTileShiftRegister <<= 1;
        
        _bgLowAttributeShiftRegister = (_bgLowAttributeShiftRegister << 1) | _bgLowAttributeLatch;
        _bgHighAttributeShiftRegister = (_bgHighAttributeShiftRegister << 1) | _bgHighAttributeLatch;
    }
    
    // Update BGs tile shift registers
    if ((dataType == 0x1) && (((_currentPixel >= 9) && (_currentPixel <= (Constants::visiblePixelsPerScanlineCount + 1))) || ((_currentPixel >= 329) && (_currentPixel <= 337)))) {
        _bgLowAttributeShiftRegister = (_bgLowAttributeShiftRegister & 0xFF00) | _lowTileByte;      // TODO: le & 0xFF00 n'est pas nécessaire car c'est shifté 8x et donc il y a 8x 0 au low byte
        _bgHighAttributeShiftRegister = (_bgHighAttributeShiftRegister & 0xFF00) | _highTileByte;   // TODO: pareil
        
        // Get two bits AT for current tile (be careful, no AND 0x3 is performed here because the AND is performed on the next lines for latchs)
        uint8_t atForCurrentTile = _atByte >> ((_address & 0x2) | ((_address >> 4) & 0x4)); // TODO: voir si bon par rapport a l'adresse comme on lit pour les prochains tiles
        
        // Get two bits separated for each latch (latchs are bool so there is no need to AND the high bit with 0x1)
        _bgLowAttributeLatch = atForCurrentTile & 0x1;
        _bgHighAttributeLatch = atForCurrentTile & 0x2;
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::calculatePixel() {
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::calculateBgPixel() {
    // TODO: ca va etre un genre de : uint8_t pixelIndexed = (((_atHighShiftRegister >> (7 - _fineXScroll)) & 0x1) << 3) | (((_atLowShiftRegister >> (7 - _fineXScroll)) & 0x1) << 2) | ((_highBGTileShiftRegister >> (14 - _fineXScroll)) & 0x2) | ((_lowBGTileShiftRegister >> (15 - _fineXScroll)) & 0x1); // Avec 14 = (7 - _fineXScroll) + 7 -> 7 pour inverser le _fineXScroll et 7 pour décaler a droite en gardant le 2eme bit et avec 15 = (7 - _fineXScroll) + 8 -> 7 pour inverser le _fineXScroll et 8 pour décaler a droite en gardant le 1er bit : OU plus simplement dit 14 pour le 15eme bit et 15 pour le 16eme bit (shift)
    return (((_bgHighAttributeShiftRegister >> (7 - _fineXScroll)) & 0x1) << 3) |
            (((_bgLowAttributeShiftRegister >> (7 - _fineXScroll)) & 0x1) << 2) |
            ((_bgHighTileShiftRegister >> (14 - _fineXScroll)) & 0x2) |
            ((_bgLowTileShiftRegister >> (15 - _fineXScroll)) & 0x1);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::getColorFromPalette(uint8_t index) {
    // TODO: voir pour background palette hack ici : https://wiki.nesdev.com/w/index.php/PPU_palettes
    // TODO: voir aussi pour exts en input
    // TODO: voir emphase colors et grayscale ?
    
    return _paletteIndexMemory[index];// TODO: a changer
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

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isInRenderingPeriod() const {
    return (isRenderingEnabled() == true) && (_currentScanline);
}

#endif /* NESEmu_Ppu_s_hpp */
