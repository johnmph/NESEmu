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
    static constexpr unsigned int visiblePixelsPerScanlineCount = 256;
    static constexpr unsigned int hBlankLengthInPixel = 85;
    static constexpr unsigned int visibleScanlinePerFrameCount = 240;    // TODO: changer par des StartLine ? pour eviter les additions inutiles
    static constexpr unsigned int postRenderLengthInScanline = 1;
    static constexpr unsigned int vBlankLengthInScanline = 20;
    static constexpr unsigned int preRenderLengthInScanline = 1;
};

template <>
struct Constants<Model::Ricoh2C07> {
    static constexpr unsigned int visiblePixelsPerScanlineCount = 256;
    static constexpr unsigned int hBlankLengthInPixel = 85;
    static constexpr unsigned int visibleScanlinePerFrameCount = 240;
    static constexpr unsigned int postRenderLengthInScanline = 1;
    static constexpr unsigned int vBlankLengthInScanline = 70;
    static constexpr unsigned int preRenderLengthInScanline = 1;
};


template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::Chip(TBus &bus, TInterruptHardware &interruptHardware, TGraphicHardware &graphicHardware) : _bus(bus), _interruptHardware(interruptHardware), _graphicHardware(graphicHardware), _objectAttributeMemory(/*64*/256), _secondObjectAttributeMemory(/*8*/32), _paletteIndexMemory(32) {
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::powerUp() {
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::clock() {
    // Process line
    processLine();
    
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
        //_controlSpriteSize8x16px = data & 0x20;
        _controlSpriteSize = ((data & 0x20) != 0) ? 16 : 8; // (((data & 0x20) >> 1) | (((data & 0x20) >> 2) ^ 0x8));
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
            // TODO: normalement ca fait des incrementations bizarres de oamAddress : voir les tests que j'ai fait avec Visual2C02, apparemment on écrit FF (a la place de data) a l'adresse _oamAddress si l'écriture s'est fait pendant un pixel pair (et si avant cycle 7 ou 6? mais ici un cycle PPU = 1 pixel tandis que le cycle en question est un 1/8 de cycle PPU) et on incrémente _oamAddress de 4 sauf si le cycle d'incrémentation tombe pendant les 2 1ers cycles (1/8 de cycle PPU) d'écriture IO car le second oam clear desactive l'incrementation de oamAddress
            // TODO: si pendant le sprite evaluation, si l'écriture se fait pendant la lecture du Y byte et que l'évaluation incrémente de 4 car le sprite n'est pas dans le scanline, ca incrementera 2x de 4 donc de 8, et si le sprite est pris, on incremente de 2x 1 donc de 2 (c'est comme s'il y a un signal qui choisit si on incremente de 1 ou de 4 l'adresse et qu'ici c'est le signal d'incrémentation : peut etre que c'est pour ca qu'on incremente pas tjs dans le second oam clear, peut etre qu'il reset le signal d'incrementation qui etait setté ici ?
            // TODO: il y a des fois aussi dans le sprite evaluation ou l'écriture qui etait censée se faire dans le second OAM se fait dans le OAM normal, il écrit donc la valeur lue juste avant (_oamData et pas data d'ici)
            // TODO: trouvé le signal de choix d'incrementation (1 ou 4) dans visual2C02 (il vaut 1 quand increment de 4 et 0 quand increment de 1, a 1 pendant le second oam clear) : spr_addr_clear_low_bump_high_setup
            // TODO: et le signal d'incrémentation est : spr_addr_load_next_value (avec spr_addr_next)
            // TODO: attention : quand on incremente de 4, on doit etre aligné sur une adresse de 4, donc si par exemple l'addr est a 5, apres l'incrementation de 4 il sera a 8 et non a 9, faire _oamAddress = (_oamAddress & ~0x3) + 4; // Si incrementation == 4
            // TODO: le meilleur moyen d'emuler ca est de retirer ce if, de laisser writeObjectAttributeMemory gérer l'écriture du FF (mais faire attention car en sprite evaluation ca n'ecrit pas) et avoir une methode pour incrementer oamAddress a la place de ++_oamAddress qui gérera l'incrementation par 1 ou par 4 : NON car ca ecrit aussi en sprite evaluation et il faut faire attention au pixel pair
            // TODO: le meilleur moyen d'émuler ca et d'emuler les io en general c'est d'avoir les io qui sont actifs pour 3 cycles PPU ! car un cycle CPU qui lance un io prend 3 cycles PPU, il faudrait donc avoir dans les io des variables qui sont settées : io_ce qui vaut 3, et dès qu'on est dans le clock ici on regarde si io_ce > 0 et si oui on fait les operations io le temps que io_ce > 0 puis a la fin du clock if (io_ce > 0) --io_ce; (pendant le 1er cycle ca ne fait rien apparemment car dans visual2C02 io_ce est tjs a 1, puis il passe a 0 apres 8 cycles visual, donc 1 cycle PPU et il reste ainsi pour 15 cycles visual et le dernier cycle visual il repasse a 1)
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
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isInRenderScanline() const {
    return (_currentScanline < Constants::visibleScanlinePerFrameCount);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isInPostRenderScanline() const {
    return ((_currentScanline >= Constants::visibleScanlinePerFrameCount) && (_currentScanline < (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline)));
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isInVBlankScanline() const {
    return ((_currentScanline >= (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline)) && (_currentScanline < (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline + Constants::vBlankLengthInScanline)));
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isInPreRenderScanline() const {
    // Assume that _currentScanline never goes beyond pre-render scanline
    return (_currentScanline >= (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline + Constants::vBlankLengthInScanline));
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isRenderingEnabled() const {
    return _maskShowBackground | _maskShowSprites;  // TODO: par apres uniformiser les operations logiques et binaires (tester si performance)
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isInRenderingPeriod() const {
    // To be in rendering period, render must be enabled and the current scanline must be in render or pre-render line)
    return (isRenderingEnabled() == true) && ((_currentScanline < Constants::visibleScanlinePerFrameCount) || (_currentScanline == (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline + Constants::vBlankLengthInScanline)));
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isInSecondOamClearPeriod() const {
    // TODO: retourne true si on est dans la periode mais pas obligé la phase (car on est dans la période si on est aussi dans le pre-render mais ca ne le fait pas, ca retourne juste les FF)
    // TODO: normalement entre le pixel 1 et 65 (voir si pixel 0 quand meme ou non)
    // TODO: seulement quand le rendering est activé
    // TODO: normalement entre le scanline 0 et 239 + le 261 (voir si autre quand meme ou non) -> pas les autres
    // TODO: on peut combiner les 2 regles ci-dessus pour avoir : isInRenderingPeriod()
    // To be in second OAM clear period, it must be in rendering period and current pixel must be between 1 and 64 (pixel 0 is already skipped on the clock method)
    return (isInRenderingPeriod() == true) && (_currentPixel <= 64);    // TODO: par apres, uniformiser les tests <= ou < et mettre 64 en constants
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
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processLine() {// TODO: a la place de faire ca (checker le current scanline), utiliser les methodes isInXXXPhase !!! : le meilleur serait d'avoir une methode qui rafraichirerait tous les signaux en fonction du currentPixel, du currentScanline et d'autres choses et ensuite (ou avant le rafraichissement des signaux ?) appeler la methode qui gere les io et qui elle aussi modifierai certains signaux et ensuite faire les operations d'apres ces signaux
    // Idle on first pixel
    if (_currentPixel == 0) {
        return;
    }
    
    // Render line
    if (_currentScanline < Constants::visibleScanlinePerFrameCount) {
        processRenderLine();
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

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processRenderLine() {
    uint8_t pixelIndexedColor;
    
    // Only VRAM access if rendering is enabled
    if (isRenderingEnabled() == true) {
        // Complete tile data is 8 cycles
        uint8_t dataType = _currentPixel & 0x7;
        
        // Fetch tiles data
        fetchTilesData(dataType);
        
        // Process sprites
        processSprites();
        
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
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processSprites() {
    // Exit if we are no more in process sprites phase (Nothing for the remaining cycles, sprite data fetch is the same than tile data fetch)
    if (_currentPixel > 320) {// TODO: remplacer 320 par des constants de Constants:: // TODO: voir si compilateur optimise en ne faisant pas l'appel de la methode si on a cette condition (comme s'il sortait ce if a l'exterieur de cette methode, a l'endroit ou on appelle cette methode
        return;
    }
    
    // If we are in second OAM clear phase
    if (_currentPixel < (64 + 1)) {   //TODO: changer 64 par des constants de Constants:: TODO: pareil qu'au dessus, voir si desactiver le rendu pendant ceci va stopper le second OAM
        clearSecondOAM();
        return;
    }
    
    // If we are in sprite evaluation phase
    if (_currentPixel < (Constants::visiblePixelsPerScanlineCount + 1)) {   // TODO: pareil qu'au dessus
        evaluateSprites();
        return;
    }
    
    // OAMADDR is set to 0 during each of ticks 257-320 (the sprite tile loading interval) of the pre-render and visible scanlines
    _oamAddress = 0x0;  // TODO: tester si vrai dans visual2C02 : OUI meme si on ecrit $2003, il prend la valeur puis repasse a 0 ! jusque 320 puis il ne change plus sa valeur donc exactement ainsi
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::clearSecondOAM() {
    // We can get here in pre-render scanline also, in this case the only thing that it perform is incrementing second OAM address in write phase
    
    // Read cycle
    if (_currentPixel & 0x1) {
        // Read OAM at current OAM address
        if (isInRenderScanline() == true) {
            readObjectAttributeMemory();
        }
        
        return;
    }
    
    // Write cycle
    if (isInRenderScanline() == true) {
        _secondObjectAttributeMemory[_secondOamAddress] = _oamData;
    }
    
    // Increment second OAM address
    if (_secondOamAddressOverflow == false) {
        ++_secondOamAddress;
        
        if (_secondOamAddress > 0x1F) { // TODO: voir si spriteOverflow set !!!!
            _secondOamAddress = 0;
            _secondOamAddressOverflow = true; // TODO: quand remettre ca a false ??
        }
    }
    
    // TODO: tester dans Visual2C02 pour savoir quand _secondOamAddress remis a 0 (on dirait qu'une fois dépassé sa valeur max (0x1F) il revient a 0 ici et peut etre aussi dans le sprite evaluation -> oui il est reseté au début
    // TODO: un sprite clear est comme un sprite eval qui dirait que chaque eval de Y est bon et qui n'incrementerai pas oamAddress (et qui ne gererai pas le 8 sprite overflow ?)
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::evaluateSprites() {
    // TODO : Because OAM is implemented with dynamic RAM instead of static RAM, the data stored in OAM memory will quickly begin to decay into random bits if it is not being refreshed. The OAM memory is refreshed once per scanline while rendering is enabled (if either the sprite or background bit is enabled via the register at $2001), but on an NTSC PPU this refresh is prevented whenever rendering is disabled.
    // TODO: quand et comment fait il ca ? 256 read suivi de write ? de 1 a 256 (inclus) ??
    
    // Odd cycle, read data
    if (_currentPixel & 0x1) {
        readObjectAttributeMemory();
        
        return;
    }
    
    // Even cycle, write data
    //uint8_t dataType = _secondOamAddress & 0x3;  // TODO: pas bon, car quand secondOamAddress overflow ca reste a 0 vu que secondOamAddress reste a 0 !!! (a voir)
    
    // We always increment _oamAddress (by 1 or 4) and on almost all copy (except not in range Y byte) we increment also _secondOamAddress by 1
    _incrementOamAddress = true;
    _incrementSecondOamAddress = true;
    
    // If Y byte (We can't rely on _currentPixel because there is not in sync with the copy/no copy pattern of evaluation, we can't rely on _oamAddress because it can be misaligned due to a previous write to $2003 and we can't rely on _secondOamAddress because once it has been overflow, it stays to 0 but we need to perform the "copy" 4 bytes of the ninth sprite)
    //if (dataType == 0x0) {
    if (_spriteEvaluationCopyByteCount == 0) {  // TODO: voir quand le reset a 0
        // Check if Y is in scanline range and not in sprite overflow (if ninth sprite has been found, it only increment _oamAddress by 4 until it enters in hblank, same behaviour if oamAdddress has overflow (in case of oamAddress != 0 at the beginning of the evaluation))
        if (((_currentScanline - _oamData) < _controlSpriteSize) && (_statusSpriteOverflow == false) && (_oamAddressOverflow == false)) {   // TODO: voir si ok car les types sont unsigned, normalement ok
            // Check for sprite 0 in range (pixel 66 = 'sprite 0' write cycle)
            if (_currentPixel == 66) {
                _sprite0InSecondOam = true; // TODO: voir pour le remettre a 0
            }
            
            // Check for sprite overflow
            if (_secondOamAddressOverflow == true) {    // TODO: il y a un bug dans le 2C02 qui incremente oamAddress de 5 si Y n'est pas dans le range, voir si ca incremente aussi secondOam de 1 !!! : non secondOam ne bouge pas et si le byte checké apres est dans le range ca incremente de 1 et une fois tout lu, oamAddress n'incremente pas la derniere fois (pour garder la valeur alignée avec 4) pour etre plus précis il aligne la valeur incrémentée avec 4 et ne fait pas l'incrémentation de 4 apres. -> Ensuite il incremente de 4 a chaque fois (que le byte soit dans le range ou non)
                // TODO: si le byte checké par après n'est pas dans le range, ca réincremente de 5 !!!
                _statusSpriteOverflow = true;
            }
            
            // Need to copy sprite bytes, so increment _oamAddress by 1 (and increment also _secondOamAddress by 1)
            _spriteEvaluationCopyByteCount = 4;
            _oamAddressIncrement = 1;
        } else {
            // Need to go to next sprite, so increment _oamAddress by 4 and don't increment _secondOamAddress
            // There is a bug in 2C02 that increment by 4 + by 1 (so by 5) _oamAddress when _secondOamAddress has overflow and while the fetched Y byte is not in range, once one fetched Y byte is in range, it set the status sprite overflow and the increment goes to 1 then 4 once all 3 other bytes of the ninth sprite are "copied"
            _spriteEvaluationCopyByteCount = 1;
            _oamAddressIncrement = ((_secondOamAddressOverflow == false) || (_statusSpriteOverflow == true)) ? 4 : 5;
            //_oamAddressIncrement = 4 + ((_secondOamAddressOverflow == true) && (_statusSpriteOverflow == false)); // TODO: voir la meilleure ecriture
            _incrementSecondOamAddress = false;
        }
    }
    
    // If none of the OAM address overflow
    if ((_oamAddressOverflow == false) && (_secondOamAddressOverflow == false)) {
        // Copy other byte to second OAM
        // In OAM, attributes bytes are AND with $E3 because bits 2, 3 and 4 are unused, but in second OAM there is no AND on these bytes so if we have oamAddress misaligned (by a write of an unaligned with 4 value to $0003 on the second OAM clear by example), the attributes bytes in second OAM could have bits 2, 3 or 4 to 1.
        _secondObjectAttributeMemory[_secondOamAddress] = _oamData;
    } else {
        // Read from second OAM
        _oamData = _secondObjectAttributeMemory[_secondOamAddress]; // TODO: voir si ainsi, voir si ca incremente apres secondOamAddress, oamAddress, .. : secondOamAddress n'est plus incrementé une fois overflow, il reste a 0, oamAddress se comporte comme s'il evaluait tjs (de 1 si sprite Y ok, de 4 aligné si sprite Y non ok -> seulement pour le 9eme sprite, puis tjs de 4)
    }
    
    // One byte copied (or read)
    --_spriteEvaluationCopyByteCount;
    
    // Increment addresses
    // TODO: surement dans une methode a part, avec gestion des overflows qui mettent un signal a true car ici j'en aurai besoin : ATTENTION: quand incrementation par 4, ca doit etre aligné !! (
    
    // TODO: 2a. If n has overflowed back to zero (all 64 sprites evaluated), go to 4 : Mais dans le cas ou oamAddress ne valait pas 0 au début, que se passe t'il ? a simuler avec visual2C02 : ca n'incremente plus second oam address et ca incremente oam address de 4 (de facon alignée) et il ne fait plus de write second OAM meme s'il y a un Y byte valid
    
    // TODO: attention : quand on incremente de 4, on doit etre aligné sur une adresse de 4, donc si par exemple l'addr est a 5, apres l'incrementation de 4 il sera a 8 et non a 9, faire _oamAddress = (_oamAddress & ~0x3) + 4; // Si incrementation == 4
    
    // TODO: dans l'incrémentation : si incrementation de 4 et que l'adresse n'est pas alignée on l'aligne avant et on fait l'incrementation SI _statusSpriteOverflow == false, sinon on ne fait l'incrementation de 4 que si l'adresse etait alignée
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
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::readObjectAttributeMemory() {
    // If we are in second OAM period, we must read $FF instead of real data
    _oamData = (isInSecondOamClearPeriod() == false) ? _objectAttributeMemory[_oamAddress] : 0xFF;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::writeObjectAttributeMemory(uint8_t data) {
    // If we are in second OAM phase and rendering is enabled, we must write $FF instead of data
    _oamData = (isInSecondOamClearPeriod() == false) ? data : 0xFF;
    
    // Write data to OAM (if attribute byte, need to AND with $E3 because bit 2, 3 and 4 are unused)
    // It's the correct behaviour according to Visual2C02 tests, it could be optimized by doing the IF and AND in the read method instead (it's more likely to be written that to be read) but let it like true behaviour
    _objectAttributeMemory[_oamAddress] = ((_oamAddress & 0x3) == 0x2) ? (_oamData & 0xE3) : _oamData;
}

#endif /* NESEmu_Ppu_s_hpp */
