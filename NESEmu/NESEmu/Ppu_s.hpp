//
//  Ppu_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 23/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Ppu_s_hpp
#define NESEmu_Ppu_s_hpp

// TODO: constantes a la place des valeurs ?
// TODO: par apres uniformiser les operations logiques et binaires (tester si performance)
// TODO: par apres, uniformiser les tests <= ou < et > ou >=
// TODO: utiliser ~ sur les autres calculs aussi ?


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
Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::Chip(TBus &bus, TInterruptHardware &interruptHardware, TGraphicHardware &graphicHardware) : _bus(bus), _interruptHardware(interruptHardware), _graphicHardware(graphicHardware), _objectAttributeMemory(256), _secondObjectAttributeMemory(32), _paletteIndexMemory(32) {
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::powerUp() {
    // Reset PPUSTATUS
    _statusSpriteOverflow = true;
    _statusSprite0Hit = false;
    _statusVBlankStarted = true;
    
    // Reset OAMADDR
    _oamAddress = 0x0;
    
    // Reset PPUADDR
    _address = 0x0;
    
    // Do a reset
    reset(false);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::clock() {
    // Process line
    processLine();
    
    // Update state
    updateState();
    
    // Check reset
    checkReset();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::reset(bool high) {
    // Save reset line
    _resetLine = high;
    
    // If reset line is low, set reset requested
    if (_resetLine == false) {
        _resetRequested = true;
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::read(uint16_t address) { // TODO: faire le meme pour le reste (un read qui ne retourne rien mais modifie le dataBus ! ainsi si des lignes ne sont pas affectées, on ne les change pas, car si on retourne 0 on a aucun moyen de savoir si on a lu 0 ou si on a rien lu car on avait une mauvaise address), le faire aussi pour le CPU ?
    
    // Set address bus
    _addressBus = address;
    
    // Save low byte of addressBus in external octal latch
    _externalOctalLatch = _addressBus & 0xFF;
    
    // Read data from PPU bus
    uint8_t dataBus = _externalOctalLatch;
    _bus.read(_addressBus, dataBus);
    
    // Data is on low byte of address bus
    _addressBus = (_addressBus & 0xFF00) | dataBus;
    
    return dataBus;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::write(uint16_t address, uint8_t data) {
    // Set address bus
    _addressBus = address;
    
    // Save low byte of addressBus in external octal latch
    _externalOctalLatch = _addressBus & 0xFF;
    
    // Data is on low byte of address bus
    _addressBus = (_addressBus & 0xFF00) | data;
    
    // Write data to PPU bus
    _bus.write((_addressBus & 0xFF00) | _externalOctalLatch, data);// TODO: on doit lire a l'adresse _address (via des fonctions read / write qui seront gérées par le cartridge)
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::ioRead(uint16_t address) {
    // Status register
    if (address == 0x2) {
        // Least significant bits previously written into a PPU register (due to register not being updated for this address)
        _dataLatch &= 0x1F;
        
        // Update register with status
        _dataLatch |= _statusSpriteOverflow << 5;
        _dataLatch |= _statusSprite0Hit << 6;
        _dataLatch |= _statusVBlankStarted << 7;
        
        // Reset VBlank started flag
        _statusVBlankStarted = false;
        _vBlankStartedLatch = false;
        
        // Check for interrupt
        checkInterrupt();
        
        // Reset write toggle
        _writeToggle = false;
    }
    // OAM data register
    else if (address == 0x4) {
        // Read OAM
        readObjectAttributeMemory();
        
        // Get data
        // After doing some tests on Visual2C02, it seems that PPU drives unused bits low and there is no open bus behaviour implicated
        // Test : testprogram = [ 0x2000, 0x2100, 0x2300, 0x24FF, 0x0018, 0x24FF, 0x0018, 0x24AA, 0x0018, 0x24FF, 0x0018, 0x2302, 0x2514, 0x3400];
        _dataLatch = _oamData;
    }
    // Data register
    else if (address == 0x7) {
        // Save data read buffer
        uint8_t oldDataReadBuffer = _dataReadBuffer;
        
        // If bad address, open bus behaviour return low address because it is multiplexed with data bus
        // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
        //_dataReadBuffer = _address & 0xFF;    // TODO: plus besoin car c'est géré dans read
        
        // Read data from VRAM
        _dataReadBuffer = read(_address);
        
        // If palette index address, read to it directly ( see https://wiki.nesdev.com/w/index.php/PPU_registers#PPUDATA )
        if (_address > 0x3EFF) {
            // Read from palette memory (Only 6 lower bits, 2 upper bits from open bus behaviour)
            _dataLatch = (oldDataReadBuffer & 0xC0) | readPaletteIndexMemory(_address & 0xFF);   // TODO: voir si ok
        } else {
            // Read return the content of the data read buffer, data read buffer is then updated with the read value
            _dataLatch = oldDataReadBuffer;
        }
        
        // Increment address
        _address += _controlAddressIncrementPerCpuAccess;   // TODO: surement changer par un signal needIncrement : TODO est ce qu'on incremente aussi si on lit dans la palette ???
    }
    
    // dataLatch is set with read value or previous dataLatch value if read a write only register
    return _dataLatch;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::ioWrite(uint16_t address, uint8_t data) {
    // When data is written, data latch is filled with the value even if the write occurs on a read-only register
    _dataLatch = data;
    
    // Control register
    if (address == 0x0) {
        _temporaryAddress = (_temporaryAddress & 0x73FF) | (data & 0x3) << 10;
        _controlAddressIncrementPerCpuAccess = ((data & 0x4) != 0) ? 32 : 1;//((data & 0x4) << 3) | (((data & 0x4) >> 2) ^ 0x1);
        _controlSpritePatternTableAddress = (data & 0x8) << 9;
        _controlBackgroundPatternTableAddress = (data & 0x10) << 8;
        _controlSpriteSize = ((data & 0x20) != 0) ? 16 : 8; // (((data & 0x20) >> 1) | (((data & 0x20) >> 2) ^ 0x8));
        _controlMasterSlaveSelect = data & 0x40;
        _controlGenerateNmiForVBlank = data & 0x80;
        
        // Check for interrupt
        checkInterrupt();
    }
    // Mask register
    else if (address == 0x1) {
        _maskGrayscale = (data & 0x1) ? 0x30 : 0x1F;
        _maskShowBackgroundFirst8px = data & 0x2;
        _maskShowSpritesFirst8px = data & 0x4;
        _maskShowBackground = data & 0x8;
        _maskShowSprites = data & 0x10;
        _maskEmphasizeRed = data & 0x20;
        _maskEmphasizeGreen = data & 0x40;
        _maskEmphasizeBlue = data & 0x80;
    }
    // OAM address register
    else if (address == 0x3) {
        _oamAddress = data;
        
        // TODO: voir pour les bugs (2C02 seulement)
    }
    // OAM data register
    else if (address == 0x4) {
        // Can only be written during VBlank, see https://wiki.nesdev.com/w/index.php/PPU_registers
        /*if ((isRenderingEnabled() == true) && (_currentScanline < (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline))) {
            // TODO: normalement ca fait des incrementations bizarres de oamAddress : voir les tests que j'ai fait avec Visual2C02, apparemment on écrit FF (a la place de data) a l'adresse _oamAddress si l'écriture s'est fait pendant un pixel pair (et si avant cycle 7 ou 6? mais ici un cycle PPU = 1 pixel tandis que le cycle en question est un 1/8 de cycle PPU) et on incrémente _oamAddress de 4 sauf si le cycle d'incrémentation tombe pendant les 2 1ers cycles (1/8 de cycle PPU) d'écriture IO car le second oam clear desactive l'incrementation de oamAddress
            // TODO: si pendant le sprite evaluation, si l'écriture se fait pendant la lecture du Y byte et que l'évaluation incrémente de 4 car le sprite n'est pas dans le scanline, ca incrementera 2x de 4 donc de 8, et si le sprite est pris, on incremente de 2x 1 donc de 2 (c'est comme s'il y a un signal qui choisit si on incremente de 1 ou de 4 l'adresse et qu'ici c'est le signal d'incrémentation : peut etre que c'est pour ca qu'on incremente pas tjs dans le second oam clear, peut etre qu'il reset le signal d'incrementation qui etait setté ici ?
            // TODO: il y a des fois aussi dans le sprite evaluation ou l'écriture qui etait censée se faire dans le second OAM se fait dans le OAM normal, il écrit donc la valeur lue juste avant (_oamData et pas data d'ici)
            // TODO: trouvé le signal de choix d'incrementation (1 ou 4) dans visual2C02 (il vaut 1 quand increment de 4 et 0 quand increment de 1, a 1 pendant le second oam clear) : spr_addr_clear_low_bump_high_setup
            // TODO: et le signal d'incrémentation est : spr_addr_load_next_value (avec spr_addr_next)
            // TODO: attention : quand on incremente de 4, on doit etre aligné sur une adresse de 4, donc si par exemple l'addr est a 5, apres l'incrementation de 4 il sera a 8 et non a 9, faire _oamAddress = (_oamAddress & ~0x3) + 4; // Si incrementation == 4
            // TODO: le meilleur moyen d'emuler ca est de retirer ce if, de laisser writeObjectAttributeMemory gérer l'écriture du FF (mais faire attention car en sprite evaluation ca n'ecrit pas) et avoir une methode pour incrementer oamAddress a la place de ++_oamAddress qui gérera l'incrementation par 1 ou par 4 : NON car ca ecrit aussi en sprite evaluation et il faut faire attention au pixel pair
            // TODO: le meilleur moyen d'émuler ca et d'emuler les io en general c'est d'avoir les io qui sont actifs pour 3 cycles PPU ! car un cycle CPU qui lance un io prend 3 cycles PPU, il faudrait donc avoir dans les io des variables qui sont settées : io_ce qui vaut 3, et dès qu'on est dans le clock ici on regarde si io_ce > 0 et si oui on fait les operations io le temps que io_ce > 0 puis a la fin du clock if (io_ce > 0) --io_ce; (pendant le 1er cycle ca ne fait rien apparemment car dans visual2C02 io_ce est tjs a 1, puis il passe a 0 apres 8 cycles visual, donc 1 cycle PPU et il reste ainsi pour 15 cycles visual et le dernier cycle visual il repasse a 1)
            return;
        }*/
        
        // Write data to OAM
        writeObjectAttributeMemory(data);
        
        // Increment OAM address
        _needIncrementOAMAddress = true;// TODO: normalement incrementOamAddress(); mais voir quand reseter _oamAddressIncrement (le mettre a 1)
    }
    // Scroll register
    else if (address == 0x5) {
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
    else if (address == 0x6) {
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
    else if (address == 0x7) {
        // If palette index address, write to it
        if (_address > 0x3EFF) {
            // Write to palette memory
            writePaletteIndexMemory(_address, data);
        } else {
            // Write data to PPU bus
            write(_address, data);
        }
        
        // Increment address
        _address += _controlAddressIncrementPerCpuAccess;   // TODO: surement changer par un signal needIncrement : TODO est ce qu'on incremente aussi si on ecrit dans la palette ???
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::exts(uint8_t data) {
    // Don't get anything if PPU is in master mode
    if (_controlMasterSlaveSelect == true) {
        return;
    }
    
    // Save exts indexed color
    _extBackgroundIndex = data;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::getExts(uint8_t &data) const {
    // Don't drive anything if PPU is in slave mode
    if (_controlMasterSlaveSelect == false) {
        return;
    }
    
    // Set data with current pixel index
    data = _currentPixelIndex;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::checkReset() {
    // TODO: voir ici pour les registres remis a 0 : https://wiki.nesdev.com/w/index.php/PPU_pin_out_and_signal_description
    // https://wiki.nesdev.com/w/index.php/PPU_power_up_state
    
    /*
     
     /RST resets certain parts of the chip to their initial power-on state: the clock divider, video phase generator, scanline/pixel counters, and the even/odd frame toggle. It also keeps several registers zeroed out for a full frame: PPUCTRL, PPUMASK ($2001), PPUSCROLL ($2005; the VRAM address latch "T", fine X scroll, and the H/V toggle), and the VRAM read buffer. It is used in the NES to clear the screen when the console is reset either by the button or the CIC, and in a dual-PPU system it can be used to genlock the two PPUs together.
     
     */
    
    // If no need to reset, exit
    if (_resetRequested == false) {
        return;
    }
    
    // Reset PPUCTRL/PPUSCROLL temporary address
    _temporaryAddress = 0x2000;
    
    // Reset PPUCTRL
    _controlAddressIncrementPerCpuAccess = 0x1;
    _controlSpritePatternTableAddress = 0x0;
    _controlBackgroundPatternTableAddress = 0x0;
    _controlSpriteSize = 8;
    _controlMasterSlaveSelect = 0;
    _controlGenerateNmiForVBlank = 0;
    
    // Reset PPUMASK
    _maskGrayscale = false;
    _maskShowBackgroundFirst8px = false;
    _maskShowSpritesFirst8px = false;
    _maskShowBackground = false;
    _maskShowSprites = false;
    _maskEmphasizeRed = false;
    _maskEmphasizeGreen = false;
    _maskEmphasizeBlue = false;
    
    // Reset PPUSTATUS
    // TODO: le bit 7 est unchanged, et pas d'info sur les bits 5 et 6
    
    // Reset PPUSCROLL
    _fineXScroll = 0;
    
    // Reset PPUDATA
    _dataReadBuffer = 0x0;
    
    // Reset internals
    _oddFrame = false;
    _writeToggle = false;
    
    // TODO: voir pour les autres variables (scanline, pixel counter, ...)
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
    return _maskShowBackground | _maskShowSprites;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isInRenderingPeriod() const {
    // To be in rendering period, render must be enabled and the current scanline must be in render or pre-render line)
    return (isRenderingEnabled() == true) && ((_currentScanline < Constants::visibleScanlinePerFrameCount) || (_currentScanline == (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline + Constants::vBlankLengthInScanline)));
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::isInSecondOAMClearPeriod() const {
    // TODO: retourne true si on est dans la periode mais pas obligé la phase (car on est dans la période si on est aussi dans le pre-render mais ca ne le fait pas, ca retourne juste les FF)
    // TODO: normalement entre le pixel 1 et 65 (voir si pixel 0 quand meme ou non)
    // TODO: seulement quand le rendering est activé
    // TODO: normalement entre le scanline 0 et 239 + le 261 (voir si autre quand meme ou non) -> pas les autres
    // TODO: on peut combiner les 2 regles ci-dessus pour avoir : isInRenderingPeriod()
    // To be in second OAM clear period, it must be in rendering period and current pixel must be between 1 and 64 (pixel 0 is already skipped on the clock method)
    return (isInRenderingPeriod() == true) && (_currentPixel <= 64);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processLine() {// TODO: a la place de faire ca (checker le current scanline), utiliser les methodes isInXXXPhase !!! : le meilleur serait d'avoir une methode qui rafraichirerait tous les signaux en fonction du currentPixel, du currentScanline et d'autres choses et ensuite (ou avant le rafraichissement des signaux ?) appeler la methode qui gere les io et qui elle aussi modifierai certains signaux et ensuite faire les operations d'apres ces signaux
    // Idle on first pixel
    if (_currentPixel == 0) {
        
        // For the VBlank frame timing (the case when reading $2002 one PPU cycle before vblank flag is set)
        // See : https://wiki.nesdev.com/w/index.php/PPU_frame_timing
        if (_currentScanline == (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline)) {    // TOOD: il reste un probleme : comme les ioRead/Write ne dependent pas du PPU cycle (alors que je devrai le faire ainsi) si le cycle CPU du read $2002 est appelé avant le cycle PPU ici (scanline 241, pixel 0) ca va remettre ce flag a true et donc le flag vblank sera mis a true au cycle PPU d'apres alors qu'il ne le devrai pas !!, ca ne marche que si ce cycle PPU (scanline 241, pixel 0) est appelé avant le cycle CPU read $2002 (ainsi le flag est mis a false et au cycle PPU d'apres le vblank flag reste a false)
            _vBlankStartedLatch = true;
        }
        
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
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::updateState() {
    // TODO: voir pour les IO aussi ici (ou dans une autre methode updateIO())
    
    // Increment OAM address if necessary
    if (_needIncrementOAMAddress == true) {     // TODO: voir si laisser ici ou mettre ca dans le clock (vu qu'il est appelée par rapport au signal actif, pareil pour celui du bas (second OAM address)
        incrementOAMAddress();
    }
    
    // Increment second OAM address if necessary
    if (_needIncrementSecondOAMAddress == true) {
        incrementSecondOAMAddress();
    }
    
    // TODO: surement dans une methode a part, avec gestion des overflows qui mettent un signal a true car ici j'en aurai besoin : ATTENTION: quand incrementation par 4, ca doit etre aligné !! (
    
    // TODO: 2a. If n has overflowed back to zero (all 64 sprites evaluated), go to 4 : Mais dans le cas ou oamAddress ne valait pas 0 au début, que se passe t'il ? a simuler avec visual2C02 : ca n'incremente plus second oam address et ca incremente oam address de 4 (de facon alignée) et il ne fait plus de write second OAM meme s'il y a un Y byte valid : OK géré avec le code ainsi
    
    // TODO: attention : quand on incremente de 4, on doit etre aligné sur une adresse de 4, donc si par exemple l'addr est a 5, apres l'incrementation de 4 il sera a 8 et non a 9, faire _oamAddress = (_oamAddress & ~0x3) + 4; // Si incrementation == 4
    
    // TODO: dans l'incrémentation : si incrementation de 4 et que l'adresse n'est pas alignée on l'aligne avant et on fait l'incrementation SI _statusSpriteOverflow == false, sinon on ne fait l'incrementation de 4 que si l'adresse etait alignée
    
    // Increment position counters
    incrementPositionCounters();    // TODO: par apres il est possible de faire une petite optimisation car dedans il recheck la position du scanline courant pour voir si on a fini une frame, mais on fait deja des checks ici et on peut etre sur que si on est pas dans la pre-render line qu'on a pas fini la frame
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processRenderLine() {
    // Process pixel
    processPixel();
    
    // Only VRAM access if rendering is enabled
    if (isRenderingEnabled() == false) {
        return;
    }
    
    // If we are in sprite shift period (1-256)
    if (_currentPixel <= Constants::visibleScanlinePerFrameCount) {
        // Update sprite shift registers
        updateSpriteShiftRegisters();   // TODO: voir quand appelé exactement (de 1 a 256 ? de 2 a 257 ? ?) : DE 1 a 256 (le shift en 257 ne sert a rien, voir si le visual le fait quand meme par cohérence des circuits)
    }
    
    // Complete tile data is 8 cycles
    uint8_t dataType = _currentPixel & 0x7;
    
    // Process tiles
    processTiles(dataType);
    
    // Process sprites
    processSprites(dataType);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processPostRenderLine() {
    // Nothing happens in post-render line
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processVBlankLine() {
    // Set VBlank flag if necessary
    if ((_currentScanline == (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline)) && (_currentPixel == 1)) {
        _statusVBlankStarted = _vBlankStartedLatch;
        
        // Notify hardware that the VBlank has started
        _graphicHardware.notifyVBlankStarted();
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processPreRenderLine() {
    // Clear flags if necessary
    if (_currentPixel == 1) {
        _statusSpriteOverflow = false;
        _statusSprite0Hit = false;
        _statusVBlankStarted = false;
        
        // Signal which is set when reset line is low and reset at the end of VBlank
        // See https://wiki.nesdev.com/w/index.php/PPU_power_up_state
        _resetRequested = (_resetLine == false);
    }
    
    // Only VRAM access if rendering is enabled
    if (isRenderingEnabled() == false) {
        return;
    }
    
    // Complete tile data is 8 cycles
    uint8_t dataType = _currentPixel & 0x7;
    
    // Process tiles
    processTiles(dataType);
    
    // Process sprites
    processSprites(dataType);
    
    // Check for Y address copy
    if ((_currentPixel >= 280) && (_currentPixel <= 304)) {
        _address = (_address & 0x41F) | (_temporaryAddress & 0x7BE0);
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processPixel() {
    // If not in process pixel period, exit
    if ((_currentPixel < 2) || (_currentPixel > (Constants::visibleScanlinePerFrameCount + 1))) {
        // No output if no in process pixel period
        _currentPixelIndex = _extBackgroundIndex;        // TODO: voir si ca ou background palette hack ou 0 ?
        
        return;
    }
    
    // Current pixel to draw is delayed by two (Pixel 0 is a idle cycle and pixel 1 is the first sprite shift registers, so we can use these bit out informations only starting at _currentPixel == 2)
    uint8_t pixelNumber = _currentPixel - 2;
    
    if (isRenderingEnabled() == true) {
        // Calculate pixel (need to be put before process tiles and sprites because the xxxBitOut already contains data for pixel and once processXXX are called they are updated with next pixel data)
        _currentPixelIndex = calculatePixel(pixelNumber);
    } else {
        // Rendering disabled, use background color (if VRAM address points to palette during forced vblank, there is the background palette hack)
        _currentPixelIndex = (_address >= 0x3F00) ? (_address & 0xFF) : _extBackgroundIndex;
    }
    
    // Get color from index
    uint8_t pixelColor = getColorFromPaletteIndex(_currentPixelIndex);    // TODO: normalement il y a un delai de 2 cycles pour récuperer la couleur et plotter le pixel courant !!!
    
    // Plot pixel
    _graphicHardware.plotPixel(pixelNumber, _currentScanline, pixelColor);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processTiles(uint8_t dataType) {
    // Exit if we are no more in process tiles period
    if ((_currentPixel > (Constants::visiblePixelsPerScanlineCount + 1)) && (_currentPixel <= 320)) {
        return;
    }
    
    // Fetch tiles
    if (_currentPixel <= Constants::visiblePixelsPerScanlineCount) {
        fetchTiles(dataType);
    }
    
    // Update tile shift registers
    updateTileShiftRegisters(dataType);
    
    // Check for X address increment
    if (dataType == 0x0) {
        incrementXOnAddress();
    }
    
    // Check for Y address increment
    if (_currentPixel == Constants::visiblePixelsPerScanlineCount) {
        incrementYOnAddress();
        
        return;
    }
    
    // Check for X address copy
    if (_currentPixel == (Constants::visiblePixelsPerScanlineCount + 1)) {
        _address = (_address & 0x7BE0) | (_temporaryAddress & 0x41F);
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::processSprites(uint8_t dataType) {
    // Exit if we are no more in process sprites period
    if (_currentPixel > 320) {
        return;
    }
    
    // If we start second OAM clear period
    if (_currentPixel == 1) {
        startClearSecondOAM();
    }
    
    // If we are in second OAM clear period
    if (_currentPixel < (64 + 1)) {
        clearSecondOAM();
        return;
    }
    
    // If we start sprite evaluation period
    if (_currentPixel == (64 + 1)) {
        startEvaluateSprites();
    }
    
    // If we are in sprite evaluation period
    if (_currentPixel < (Constants::visiblePixelsPerScanlineCount + 1)) {
        evaluateSprites();
        return;
    }
    
    // If we start sprite fetch period
    if (_currentPixel == (Constants::visiblePixelsPerScanlineCount + 1)) {
        startFetchSprites();
    }
    
    // We are in sprite fetch period
    fetchSprites(dataType);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::fetchTiles(uint8_t dataType) {
    // Correct dataType for last cycles (Unused NT fetches)
    if (_currentPixel > 336) {
        dataType = ((dataType - 1) & 0x1) + 1;
    }
    
    // Nametable set address
    if (dataType == 0x1) {
        _addressBus = 0x2000 | (_address & 0x0FFF);
    }
    // Nametable fetch (NT byte)
    else if (dataType == 0x2) {
        _ntByte = read(_addressBus);
    }
    // Attributes set address
    else if (dataType == 0x3) {
        _addressBus = 0x23C0 | (_address & 0x0C00) | ((_address >> 4) & 0x38) | ((_address >> 2) & 0x7);
    }
    // Attributes fetch (AT byte)
    else if (dataType == 0x4) {
        _atByte = read(_addressBus);
    }
    // First pattern table set address
    else if (dataType == 0x5) {
        // One tile is 16 bytes (ntByte << 4), one byte is a 8 pixels row (fineY = _address >> 12)
        _addressBus = _controlBackgroundPatternTableAddress | (_ntByte << 4) | (_address >> 12);
    }
    // First pattern table fetch (Low tile byte)
    else if (dataType == 0x6) {
        _lowTileByte = read(_addressBus);
    }
    // Second pattern table set address
    else if (dataType == 0x7) {
        // Same than low BG tile address but 8 bytes after
        _addressBus = _controlBackgroundPatternTableAddress | (_ntByte << 4) | 0x8 | (_address >> 12);
    }
    // Second pattern table fetch (High tile byte)
    else {
        _highTileByte = read(_addressBus);
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::updateTileShiftRegisters(uint8_t dataType) {
    // On pixel 1 and 321 there is no shift registers operations
    if ((_currentPixel == 1) || (_currentPixel == 321)) {
        return;
    }
    
    // Get tile out bits
    _bgLowTileBitOut = (_bgLowTileShiftRegister >> _fineXScroll) & 0x1; // TODO: voir car ce sont les bits de gauches en 1er qui doivent etre lu (voir si reverseBits sur _lowTileByte/highTileByte avant de les charger dans les shiftregisters !!!
    _bgHighTileBitOut = (_bgHighTileShiftRegister >> _fineXScroll) & 0x1;
    
    // Get attribute out bits
    _bgLowAttributeBitOut = (_bgLowAttributeShiftRegister >> _fineXScroll) & 0x1;
    _bgHighAttributeBitOut = (_bgHighAttributeShiftRegister >> _fineXScroll) & 0x1;
    
    // Shift right tile registers and inject 1 on the MSB
    //_bgLowTileShiftRegister = 0x8000 | (_bgLowTileShiftRegister >> 1);        // TODO: les bits injectés sont a 1 MAIS c'est surement car la variable est inversée dans Visual2C02
    //_bgHighTileShiftRegister = 0x8000 | (_bgHighTileShiftRegister >> 1);
    _bgLowTileShiftRegister >>= 1;
    _bgHighTileShiftRegister >>= 1;
    
    // Shift right attribute registers and inject latch on the MSB
    _bgLowAttributeShiftRegister = (_bgLowAttributeLatch << 7) | (_bgLowAttributeShiftRegister >> 1);
    _bgHighAttributeShiftRegister = (_bgHighAttributeLatch << 7) | (_bgHighAttributeShiftRegister >> 1);
    
    // Check if need to update registers
    if (dataType == 0x1) {
        // Update tile registers
        _bgLowTileShiftRegister = (Common::reverseBits(_lowTileByte) << 8) | (_bgLowTileShiftRegister & 0xFF);  // TODO: voir si ok avec reverseBits
        _bgHighTileShiftRegister = (Common::reverseBits(_highTileByte) << 8) | (_bgHighTileShiftRegister & 0xFF);   // TODO: pareil
        
        // Get two bits AT for current tile (be careful, no AND 0x3 is performed here because the AND is performed on the next lines for latchs)
        uint8_t atForCurrentTile = _atByte >> ((_address & 0x2) | ((_address >> 4) & 0x4)); // TODO: voir si bon par rapport a l'adresse comme on lit pour les prochains tiles
        
        // Update attribute latches
        _bgLowAttributeLatch = atForCurrentTile & 0x1;
        _bgHighAttributeLatch = (atForCurrentTile >> 1) & 0x1;
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::startClearSecondOAM() {
    // We use the same code than evaluate sprites to clear OAM, simply do a 32 bytes copy (_oamAddress doesn't change and read to OAM returns $FF in this period)
    _spriteEvaluationCopyByteCount = 32;
    
    // Normally it happen on pixel 0 and not 1 but due to the even/odd pixel 0 jump, it is put here
    // Reset second OAM address
    resetSecondOAMAddress();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::clearSecondOAM() {
    // Reset OAM address overflow
    _oamAddressOverflow = false;
    
    // Clear second OAM using same circuit than evaluate sprites
    clearSecondOAMAndEvaluateSprites();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::startEvaluateSprites() {
    // TODO: peut etre rajouter la copie des bytes _oamAddress & 0xF8 dans _oamAddress : voir Notes dans https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation
    // TODO: et faire des tests visual2C02
    
    // Reset second OAM address
    resetSecondOAMAddress();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::evaluateSprites() {
    // Evalute sprites
    clearSecondOAMAndEvaluateSprites();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::clearSecondOAMAndEvaluateSprites() {
    // TODO : Because OAM is implemented with dynamic RAM instead of static RAM, the data stored in OAM memory will quickly begin to decay into random bits if it is not being refreshed. The OAM memory is refreshed once per scanline while rendering is enabled (if either the sprite or background bit is enabled via the register at $2001), but on an NTSC PPU this refresh is prevented whenever rendering is disabled.
    // TODO: quand et comment fait il ca ? 256 read suivi de write ? de 1 a 256 (inclus) ??
    
    // ** Read cycle **
    if (_currentPixel & 0x1) {
        // Read OAM at current OAM address
        if (isInRenderScanline() == true) {
            readObjectAttributeMemory();
        } else {
            // Read from second OAM
            _oamData = _secondObjectAttributeMemory[_secondOAMAddress];
        }
        
        return;
    }
    
    // ** Write cycle **
    
    // Only increment OAM address if we are in render scanline and if we are not in second OAM clear period
    _needIncrementOAMAddress = (isInRenderScanline() == true) && (isInSecondOAMClearPeriod() == false);
    
    // If Y byte (We can't rely on _currentPixel because there is not in sync with the copy/no copy pattern of evaluation, we can't rely on _oamAddress because it can be misaligned due to a previous write to $2003 and we can't rely on _secondOAMAddress because once it has been overflow, it stays to 0 but we need to perform the "copy" 4 bytes of the ninth sprite)
    if (_spriteEvaluationCopyByteCount == 0) {
        // Check if Y is in scanline range and not in sprite overflow (if ninth sprite has been found, it only increment _oamAddress by 4 until it enters in hblank, same behaviour if _oamAdddress has overflow (in case of _oamAddress != 0 at the beginning of the evaluation))
        if (((_currentScanline - _oamData) < _controlSpriteSize) && (_statusSpriteOverflow == false) && (_oamAddressOverflow == false)) {
            // Check for sprite 0 in range (pixel 66 = 'sprite 0' write cycle)
            if (_currentPixel == 66) {
                _sprite0OnNextScanline = true;
            }
            
            // Check for sprite overflow
            if (_secondOAMAddressOverflow == true) {    // TODO: il y a un bug dans le 2C02 qui incremente _oamAddress de 5 si Y n'est pas dans le range, voir si ca incremente aussi _secondOAMAddress de 1 !!! : non _secondOAMAddress ne bouge pas et si le byte checké apres est dans le range ca incremente de 1 et une fois tout lu, _oamAddress n'incremente pas la derniere fois (pour garder la valeur alignée avec 4) pour etre plus précis il aligne la valeur incrémentée avec 4 et ne fait pas l'incrémentation de 4 apres. -> Ensuite il incremente de 4 a chaque fois (que le byte soit dans le range ou non)
                // TODO: si le byte checké par après n'est pas dans le range, ca réincremente de 5 !!!
                _statusSpriteOverflow = true;
            }
            
            // Need to copy sprite bytes, so increment _oamAddress by 1 and increment also _secondOAMAddress by 1
            _spriteEvaluationCopyByteCount = 4;
            _oamAddressIncrement = 1;
            _needIncrementSecondOAMAddress = true;
        } else {
            // Check for sprite 0 in range (pixel 66 = 'sprite 0' write cycle)
            if (_currentPixel == 66) {
                _sprite0OnNextScanline = false;
            }
            
            // Need to go to next sprite, so increment _oamAddress by 4 and don't increment _secondOAMAddress
            // There is a bug in 2C02 that increment by 4 + by 1 (so by 5) _oamAddress when _secondOAMAddress has overflow and while the fetched Y byte is not in range, once one fetched Y byte is in range, it set the status sprite overflow and the increment goes to 1 then 4 once all 3 other bytes of the ninth sprite are "copied"
            _spriteEvaluationCopyByteCount = 1;
            _oamAddressIncrement = ((_secondOAMAddressOverflow == false) || (_statusSpriteOverflow == true)) ? 4 : 5;
            //_oamAddressIncrement = 4 + ((_secondOAMAddressOverflow == true) && (_statusSpriteOverflow == false)); // TODO: voir la meilleure ecriture
        }
    } else {
        _needIncrementSecondOAMAddress = true;
    }
    
    // If none of the OAM address overflow
    if ((isInRenderScanline() == true) && (_oamAddressOverflow == false) && (_secondOAMAddressOverflow == false)) {
        // Copy other byte to second OAM
        // In OAM, attributes bytes are AND with $E3 because bits 2, 3 and 4 are unused, but in second OAM there is no AND on these bytes so if we have _oamAddress misaligned (by a write of an unaligned with 4 value to $0003 on the second OAM clear by example), the attributes bytes in second OAM could have bits 2, 3 or 4 to 1.
        _secondObjectAttributeMemory[_secondOAMAddress] = _oamData;
    } else {
        // Read from second OAM
        _oamData = _secondObjectAttributeMemory[_secondOAMAddress]; // TODO: voir si ainsi, voir si ca incremente apres secondOamAddress, oamAddress, .. : secondOamAddress n'est plus incrementé une fois overflow, il reste a 0, oamAddress se comporte comme s'il evaluait tjs (de 1 si sprite Y ok, de 4 aligné si sprite Y non ok -> seulement pour le 9eme sprite, puis tjs de 4)
    }
    
    // One byte copied (or read)
    --_spriteEvaluationCopyByteCount;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::startFetchSprites() {
    // Reset second OAM address
    resetSecondOAMAddress();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::fetchSprites(uint8_t dataType) {
    // OAMADDR is set to 0 during each of ticks 257-320 (the sprite tile loading interval) of the pre-render and visible scanlines
    _oamAddress = 0;
    
    // Save _sprite0OnNextScanline on _sprite0OnCurrentScanline (during 257-320) because it can be modified during rendering of the current scanline
    _sprite0OnCurrentScanline = _sprite0OnNextScanline;
    
    // Sprite number start at 0 at beginning of sprite fetch and a sprite take 8 cycles, we can't use (_secondOAMAddress >> 2) because if rendering is disabled/reenabled in fetch sprites, _secondOAMAddress is not incremented during render disabled but spriteNumber is incremented
    uint8_t spriteNumber = (_currentPixel - (Constants::visiblePixelsPerScanlineCount + 1)) >> 3;
    
    // Garbage nametable set address + load sprite Y byte
    if (dataType == 0x1) {
        _addressBus = 0x2000 | (_address & 0x0FFF);
        
        // TODO: je n'arrive pas a trouver ntByte ni le fetchY fetchIndex signal dans visual2C02 !!!
        _spriteY = _secondObjectAttributeMemory[_secondOAMAddress];
        _needIncrementSecondOAMAddress = true;
    }
    // Garbage nametable fetch (NT byte) + load sprite tile index byte
    else if (dataType == 0x2) {
        _ntByte = read(_addressBus);
        
        _spriteTileIndex = _secondObjectAttributeMemory[_secondOAMAddress];
        _needIncrementSecondOAMAddress = true;
    }
    // Garbage nametable set address + load sprite attribute byte
    else if (dataType == 0x3) {
        _addressBus = 0x2000 | (_address & 0x0FFF);
        
        _spAttributeLatches[spriteNumber] = _secondObjectAttributeMemory[_secondOAMAddress];
        _needIncrementSecondOAMAddress = true;
    }
    // Garbage nametable fetch (NT byte) + load sprite X byte
    else if (dataType == 0x4) {
        _ntByte = read(_addressBus);
        
        _spXPositionCounters[spriteNumber] = _secondObjectAttributeMemory[_secondOAMAddress];//TODO: + voir si quand on lit dans le secondObjectAttributeMemory si on change _oamData : oui, vois si possible d'intégrer ca dans readObjectAttributeMemory (et write ?)
        _needIncrementSecondOAMAddress = true;
    }
    // First pattern table set address
    else if (dataType == 0x5) {
        // Get address for sprite
        _addressBus = getAddressForFetchSprites(spriteNumber);
    }
    // First pattern table fetch (Low tile byte)
    else if (dataType == 0x6) {
        _lowTileByte = getDataForFetchSprites(spriteNumber);
    }
    // Second pattern table set address
    else if (dataType == 0x7) {
        // Same than low BG tile address but 8 bytes after
        _addressBus = getAddressForFetchSprites(spriteNumber) | 0x8;
    }
    // Second pattern table fetch (High tile byte)
    else {
        _highTileByte = getDataForFetchSprites(spriteNumber);
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint16_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::getAddressForFetchSprites(uint8_t spriteNumber) {
    uint8_t spriteRow = _currentScanline - _spriteY;
    uint8_t tileIndex = _spriteTileIndex;
    
    // Flip vertically if necessary
    if ((_spAttributeLatches[spriteNumber] & 0x80) == true) {
        spriteRow = (_controlSpriteSize - 1) - spriteRow;
    }
    
    uint16_t patternTableAddress;
    
    // If 8x16 sprites
    if (_controlSpriteSize == 16) {
        patternTableAddress = (_spriteTileIndex & 0x1) << 12;
        
        // If second tile of sprite
        if (spriteRow > 7) {
            ++tileIndex;
            spriteRow -= 8;
        }
    }
    // 8x8 sprites
    else {
        patternTableAddress = _controlSpritePatternTableAddress;
    }
    
    // One tile is 16 bytes (tileIndex << 4), one byte is a 8 pixels row (spriteRow)
    return patternTableAddress | (tileIndex << 4) | spriteRow;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::getDataForFetchSprites(uint8_t spriteNumber) {
    // Read data (even if unused sprite)
    uint8_t data = read(_addressBus);
    
    // If unused sprite, all pixels are transparent
    if ((_currentScanline - _spriteY) >= _controlSpriteSize) {
        data = 0x0;
    }
    
    // Flip horizontally if necessary (in hardware it is reversed if it is NOT flipped because the shift register out the right pixel first)
    if ((_spAttributeLatches[spriteNumber] & 0x40) == false) {
        data = Common::reverseBits(data);
    }
    
    return data;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::updateSpriteShiftRegisters() {
    // Sprites loop
    bool spriteFound = false;
    
    for (int spriteNumber = 0; spriteNumber < 8; ++spriteNumber) {
        // If sprite not ready, decrement its X counter then go to next sprite
        if (_spXPositionCounters[spriteNumber] > 0) {
            --_spXPositionCounters[spriteNumber];
            continue;
        }
        
        // Shift registers
        uint8_t spLowTileBitOut =_spLowTileShiftRegisters[spriteNumber] & 0x1;
        uint8_t spHighTileBitOut =_spHighTileShiftRegisters[spriteNumber] & 0x1;
        _spLowTileShiftRegisters[spriteNumber] >>= 1;
        _spHighTileShiftRegisters[spriteNumber] >>= 1;
        
        // If sprite already found or pixel transparent, skip this sprite
        if ((spriteFound == true) || ((spLowTileBitOut | spHighTileBitOut) == 0x0)) {   // TODO: voir si bon ainsi
            continue;
        }
        
        // Save sprite data
        _spLowTileBitOut = spLowTileBitOut;
        _spHighTileBitOut = spHighTileBitOut;
        _spAttributeLatchActive = _spAttributeLatches[spriteNumber];
        _sprite0Active = ((_sprite0OnCurrentScanline == true) && (spriteNumber == 0));
        
        // Set flag
        spriteFound = true;
    }
    
    // If no sprite found
    if (spriteFound == false) {
        // Initialize sprite data with 0
        _spLowTileBitOut = 0;
        _spHighTileBitOut = 0;
        _spAttributeLatchActive = 0;
        _sprite0Active = false;
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::incrementXOnAddress() {
    _address = ((_address & ~0x1F) | ((_address + 1) & 0x1F)) ^ ((((_address & 0x1F) + 1) & 0x20) << 5);
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
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::incrementOAMAddress() {
    // Continue to increment even if overflow
    
    // Increment (always by 1 if we are not in rendering period)
    _oamAddress += (isInRenderingPeriod()) ? _oamAddressIncrement : 1;
    
    // Check overflow (_oamAddress is uint8_t so it returns to its value - 255 after 255)
    if (_oamAddress < _oamAddressIncrement) {
        // Set overflow flag
        _oamAddressOverflow = true;
    }
    
    // Align if necessary
    if (_oamAddressIncrement == 4) {
        _oamAddress &= ~0x3;
    }
    
    // Reset increment OAM address increment flag
    _needIncrementOAMAddress = false;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::resetSecondOAMAddress() {
    // Reset second OAM address (because it can be overflow and it can be lower than 0x1F, no matter its value, it is reset to 0)
    _secondOAMAddressOverflow = false;
    _secondOAMAddress = 0;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::incrementSecondOAMAddress() {
    // Don't increment if overflow
    if (_secondOAMAddressOverflow == true) {
        return;
    }
    
    // Increment
    ++_secondOAMAddress;
    
    // Check overflow
    if (_secondOAMAddress > 0x1F) {
        // Reset address and set overflow flag
        _secondOAMAddress = 0;
        _secondOAMAddressOverflow = true;
    }
    
    // Reset second increment OAM address increment flag
    _needIncrementSecondOAMAddress = false;
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
    if (_currentScanline < (Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline + Constants::vBlankLengthInScanline + 1)) {
        return;
    }
    
    // Start a new frame, reset scanline counter and toggle even/odd frame
    _currentScanline = 0;
    _oddFrame = !_oddFrame;
    
    // If start a new odd frame, skip first cycle
    _currentPixel += (_oddFrame & isRenderingEnabled());     // TODO: voir si c bon
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::calculatePixel(uint8_t pixelNumber) {
    // Get BG pixel
    uint8_t bgPixel = ((_maskShowBackground == true) && ((_maskShowBackgroundFirst8px == true) || (pixelNumber >= 8))) ? ((_bgHighTileBitOut << 1) | _bgLowTileBitOut) : 0x0;  // TODO: on peut simplifier ce test en ayant une variable qui est le pixel min a commencer a afficher qui sera 0 si showBackground == true et 256 si showBackground = false et 8 si showBackground == true mais maskShowBackgroundFirst8px == false : cette variable devra etre initialisée dans le write du mask register car c'est lui qui contient les données
    
    // Get sprite pixel
    uint8_t spPixel = ((_maskShowSprites == true) && ((_maskShowSpritesFirst8px == true) || (pixelNumber >= 8))) ? ((_spHighTileBitOut << 1) |_spLowTileBitOut) : 0x0; // TODO: pareil qu'au dessus
    
    // Check for sprite 0 hit
    checkSprite0Hit(bgPixel);
    
    // Check if sprite pixel has priority
    if ((spPixel != 0x0) && (((_spAttributeLatchActive & 0x20) == false) || (bgPixel == 0x0))) {
        // 0x10 for sprite palettes
        return 0x10 | ((_spAttributeLatchActive & 0x3) << 2) | spPixel;
    }
    
    // Check if background pixel has priority
    if (bgPixel != 0x0) {
        return (_bgHighAttributeBitOut << 3) | (_bgLowAttributeBitOut << 2) | bgPixel;
    }
    
    // No pixel has priority, use BG ($3F00)
    return _extBackgroundIndex;  // TODO: voir si ok + SI le spPixel ou bgPixel == 0 (si la palette = 0) est ce qu'on retourne 0 ou bien _extBackgroundIndex ??? si on doi retourner _extBackgroundIndex, alors remettre 0 a la place de _extBackgroundIndex partout et dans getColorFromPalette checker si l'index 0 et si oui utiliser l'index _extBackgroundIndex
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::checkSprite0Hit(uint8_t bgPixel) {
    // Don't check if sprite 0 is not active
    if (_sprite0Active == false) {
        return;
    }
    
    // TODO: n'est pas activé au pixel 255 !!! a voir : https://wiki.nesdev.com/w/index.php/PPU_OAM
    
    // A sprite 0 hit occurs if both pixels are non zero, spPixel test is integrated in _sprite0Active (and don't change the flag if no sprite 0 hit occurs on this current pixel)
    if (bgPixel != 0x0) {
        _statusSprite0Hit = true;
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::getColorFromPaletteIndex(uint8_t index) {
    // Get color index from palette index memory
    uint8_t colorIndex = readPaletteIndexMemory(index);
    
    // Get color from graphic hardware
    return _graphicHardware.getColorFromIndex(colorIndex, _maskEmphasizeRed, _maskEmphasizeGreen, _maskEmphasizeBlue);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::checkInterrupt() {
    // See https://wiki.nesdev.com/w/index.php/NMI
    _interruptHardware.ppuInterrupt((_statusVBlankStarted == false) || (_controlGenerateNmiForVBlank == false));
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::readObjectAttributeMemory() {
    // If we are in second OAM period, we must read $FF instead of real data
    _oamData = (isInSecondOAMClearPeriod() == false) ? _objectAttributeMemory[_oamAddress] : 0xFF;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::writeObjectAttributeMemory(uint8_t data) {
    // If we are in second OAM phase and rendering is enabled, we must write $FF instead of data
    _oamData = (isInSecondOAMClearPeriod() == false) ? data : 0xFF;
    
    // Write data to OAM (if attribute byte, need to AND with $E3 because bit 2, 3 and 4 are unused)
    // It's the correct behaviour according to Visual2C02 tests, it could be optimized by doing the IF and AND in the read method instead (it's more likely to be written that to be read) but let it like true behaviour
    _objectAttributeMemory[_oamAddress] = ((_oamAddress & 0x3) == 0x2) ? (_oamData & 0xE3) : _oamData;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::readPaletteIndexMemory(uint8_t address) {
    // Addresses $10/$14/$18/$1C are mirrors of $00/$04/$08/$0C
    uint8_t mirroredAddress = address & (((address & 0x13) == 0x10) ? 0xF : 0x1F);
    
    // Read from palette index memory
    return _paletteIndexMemory[mirroredAddress] & _maskGrayscale;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
void Chip<EModel, TBus, TInterruptHardware, TGraphicHardware>::writePaletteIndexMemory(uint8_t address, uint8_t data) {
    // Addresses $10/$14/$18/$1C are mirrors of $00/$04/$08/$0C
    uint8_t mirroredAddress = address & (((address & 0x13) == 0x10) ? 0xF : 0x1F);
    
    // Write to palette index memory (only 6 bits)
    _paletteIndexMemory[mirroredAddress] = data & 0x3F;
}

#endif /* NESEmu_Ppu_s_hpp */
