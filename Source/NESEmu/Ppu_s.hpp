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
    static constexpr unsigned int visibleScanlinePerFrameCount = 240;
    static constexpr unsigned int postRenderLengthInScanline = 1;
    static constexpr unsigned int vBlankLengthInScanline = 20;
    static constexpr unsigned int preRenderLengthInScanline = 1;
};//TODO: Early revisions cannot read back sprite or palette memory voir : https://wiki.nesdev.com/w/index.php/Cycle_reference_chart#Clock_rates

template <>
struct Constants<Model::Ricoh2C07> {
    static constexpr unsigned int visiblePixelsPerScanlineCount = 256;
    static constexpr unsigned int hBlankLengthInPixel = 85;
    static constexpr unsigned int visibleScanlinePerFrameCount = 240;
    static constexpr unsigned int postRenderLengthInScanline = 1;
    static constexpr unsigned int vBlankLengthInScanline = 70;
    static constexpr unsigned int preRenderLengthInScanline = 1;
};


template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::Chip(TBus &bus, TInterruptHardware &interruptHardware, TGraphicManager &graphicManager) : _bus(bus), _interruptHardware(interruptHardware), _graphicManager(graphicManager), _objectAttributeMemory(256), _secondObjectAttributeMemory(32), _paletteIndexMemory(32) {
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::powerUp() {
    // See https://wiki.nesdev.com/w/index.php/PPU_power_up_state
    
    // Reset PPUSTATUS
    _statusSpriteOverflow = true;
    _statusSprite0Hit = false;
    
    // This flag is specified as often set in the doc but there is a problem with Donkey Kong if it is set because of the PPU warm up and the fact that Donkey Kong doesn't wait two VBlank in case of first VBlank wait loop passed immediatly due to the VBlank status flag set at power up
    // See http://forums.nesdev.com/viewtopic.php?f=3&t=19792
    _statusVBlankStarted = false;
    _vBlankStartedLatch = false;
    
    // Reset OAMADDR
    _oamAddress = 0x0;
    
    // Reset PPUADDR
    _address = 0x0;
    
    // Reset internals
    _currentScanline = 0;
    _currentPixel = 0;
    
    _sprite0OnNextScanline = false;
    _extBackgroundIndex = 0;
    
    _oamAddressIncrement = 1;   // TODO: voir pour les internals
    _oamAddressOverflow = false;
    _needIncrementOAMAddress = false;
    _needIncrementSecondOAMAddress = false;
    _copyAddressCycleDelay = 0;
    _read2007RaceConditionCycle = 0;
    
    _ioPending = 0;
    
    // Reset by pulling line down
    reset(false);
    
    // Release reset
    reset(true);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::clock() {
    // Process rendering
    processRendering();
    
    // Process IO
    processIO();
    
    // Update state
    updateState();
    
    // Check interrupt
    checkInterrupt();
    
    // Check reset
    checkReset();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::reset(bool high) {
    // Save reset line
    _resetLine = high;
    
    // If reset line is low, set reset requested
    if (!_resetLine) {
        _resetRequested = true;
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::exts(uint8_t data) {
    // Don't get anything if PPU is in master mode
    if (_controlMasterSlaveSelect) {
        return;
    }
    
    // Save exts indexed color
    _extBackgroundIndex = data;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::getExts(uint8_t &data) const {
    // Don't drive anything if PPU is in slave mode
    if (!_controlMasterSlaveSelect) {
        return;
    }
    
    // Set data with current pixel index
    data = _currentPixelIndex;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
template <class TConnectedBus>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::readPerformed(TConnectedBus &connectedBus) {
    // Get bus
    _ioBus = &connectedBus;
    
    // Get read method
    _ioMethod = &Chip::ioRead<TConnectedBus>;
    
    // Set mode to read
    //_ioReadMode = true;
    
    // Set io pending
    _ioPending = 1;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
template <class TConnectedBus>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::writePerformed(TConnectedBus &connectedBus) {
    // Get bus
    _ioBus = &connectedBus;
    
    // Get write method
    _ioMethod = &Chip::ioWrite<TConnectedBus>;
    
    // Set mode to write
    //_ioReadMode = false;
    
    // Set io pending
    _ioPending = 1;
}


// *** Helper ***

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::isInRenderScanline() const {
    return (_currentScanline >= ModelConstants::firstRenderPeriodScanline) && (_currentScanline <= ModelConstants::lastRenderPeriodScanline);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::isInPostRenderScanline() const {
    return (_currentScanline >= ModelConstants::firstPostRenderPeriodScanline) && (_currentScanline <= ModelConstants::lastPostRenderPeriodScanline);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::isInVBlankScanline() const {
    return (_currentScanline >= ModelConstants::firstVBlankPeriodScanline) && (_currentScanline <= ModelConstants::lastVBlankPeriodScanline);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::isInPreRenderScanline() const {
    return (_currentScanline >= ModelConstants::firstPreRenderPeriodScanline) && (_currentScanline <= ModelConstants::lastPreRenderPeriodScanline);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::isRenderingEnabled() const {
    return _maskShowBackground || _maskShowSprites;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::isInRenderingPeriod() const {
    // To be in rendering period, render must be enabled and the current scanline must be in render or pre-render line)
    return isRenderingEnabled() && (isInRenderScanline() || isInPreRenderScanline());
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
bool Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::isInSecondOAMClearPeriod() const {
    // To be in second OAM clear period, it must be in rendering period and current pixel must be between 1 and 64 (pixel 0 is already skipped on the clock method) //TODO: le pixel 0 n'est pas skipé si on appelle read ou write OAM et que ca tombe exactement au pixel 0 (dans ce cas retourner FF ou non ???) a tester sur Visual2C02 (comme c'est ici pour l'instant ca le fait)
    return isInRenderingPeriod() && (_currentPixel <= ModelConstants::lastSecondClearOamPeriodPixel);
}


// *** Main ***

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::processRendering() {// TODO: a la place de faire ca (checker le current scanline), utiliser les methodes isInXXXPhase !!! : le meilleur serait d'avoir une methode qui rafraichirait tous les signaux en fonction du currentPixel, du currentScanline et d'autres choses et ensuite (ou avant le rafraichissement des signaux ?) appeler la methode qui gere les io et qui elle aussi modifierai certains signaux et ensuite faire les operations d'apres ces signaux
    
    // Render line
    if (_currentScanline <= ModelConstants::lastRenderPeriodScanline) {
        processRenderLine();
    }
    // Post-render line
    else if (_currentScanline <= ModelConstants::lastPostRenderPeriodScanline) {
        processPostRenderLine();
    }
    // VBlank line
    else if (_currentScanline <= ModelConstants::lastVBlankPeriodScanline) {
        processVBlankLine();
    }
    // Pre-render line
    else {
        processPreRenderLine();
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::processIO() {
    // Decrement read 2007 race condition cycle counter if necessary
    if (_read2007RaceConditionCycle > 0) {
        --_read2007RaceConditionCycle;
    }
    
    // If no pending IO, exit
    if (_ioPending == 0) {
        return;
    }
    
    // If time to process IO
    if (_ioPending == 1) {
        // Perform IO
        (this->*_ioMethod)();
    }
    
    // Count down time for processing IO
    _ioPending >>= 1;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::checkInterrupt() {
    // See https://wiki.nesdev.com/w/index.php/NMI
    _interruptHardware.interrupt(!(_statusVBlankStarted && _controlGenerateNmiForVBlank));
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::updateState() {
    // Update status VBlank flag
    _statusVBlankStarted = _vBlankStartedLatch;
    
    // Increment OAM address if necessary
    if (_needIncrementOAMAddress) {
        incrementOAMAddress();
    }
    
    // Increment second OAM address if necessary
    if (_needIncrementSecondOAMAddress) {
        incrementSecondOAMAddress();
    }
    
    // TODO: 2a. If n has overflowed back to zero (all 64 sprites evaluated), go to 4 : Mais dans le cas ou oamAddress ne valait pas 0 au début, que se passe t'il ? a simuler avec visual2C02 : ca n'incremente plus second oam address et ca incremente oam address de 4 (de facon alignée) et il ne fait plus de write second OAM meme s'il y a un Y byte valid : OK géré avec le code ainsi
    
    // TODO: attention : quand on incremente de 4, on doit etre aligné sur une adresse de 4, donc si par exemple l'addr est a 5, apres l'incrementation de 4 il sera a 8 et non a 9, faire _oamAddress = (_oamAddress & ~0x3) + 4; // Si incrementation == 4
    
    // TODO: dans l'incrémentation : si incrementation de 4 et que l'adresse n'est pas alignée on l'aligne avant et on fait l'incrementation SI _statusSpriteOverflow == false, sinon on ne fait l'incrementation de 4 que si l'adresse etait alignée
    
    // If need to copy temporary address to address
    if (_copyAddressCycleDelay > 0) {
        // Decrement cycle delay counter
        --_copyAddressCycleDelay;
        
        // If reached 0
        if (_copyAddressCycleDelay == 0) {
            // Copy temporary address to address
            _address = _temporaryAddress;
            
            // Set address bus
            _bus.setAddressBus(_address);
        }
    }
    
    // Increment position counters
    incrementPositionCounters();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::checkReset() {
    // See https://wiki.nesdev.com/w/index.php/PPU_power_up_state
    
    // If no need to reset, exit
    if (!_resetRequested) {
        return;
    }
    
    _resetRequested = false;  // TODO: pour desactiver le PPU warmup
    
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
}


// *** Rendering ***

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::processRenderLine() {
    // Only VRAM access if rendering is enabled
    if (isRenderingEnabled()) {
        // Complete tile data is 8 cycles
        uint8_t dataType = _currentPixel & 0x7;
        
        // Process tiles
        processTiles(dataType);
        
        // Process sprites
        processSprites(dataType);
    }
    
    // Process pixel
    processPixel();
    
    // Update sprite shift registers only in visible pixels (1-256)
    if ((_currentPixel >= ModelConstants::firstActivePixel) && (_currentPixel <= ModelConstants::visiblePixelsPerScanlineCount)) {
        updateSpriteShiftRegisters();
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::processPostRenderLine() {
    // Nothing happens in post-render line
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::processVBlankLine() {
    if ((_currentScanline == ModelConstants::firstVBlankPeriodScanline) && (_currentPixel == ModelConstants::firstActivePixel)) {
        // Set VBlank flag
        _vBlankStartedLatch = true;
        
        // Emulate open bus decay (it must be at 0 after 1 second max, the maximum value is 255 and this method is called 60 times per second)
        // 255 / 60 = 4.25 so it must be at least 5 to ensure that is zero after 1 second
        _dataBusCapacitanceLatch = (_dataBusCapacitanceLatch >= 5) ? (_dataBusCapacitanceLatch - 5) : 0;
        
        // Notify manager that the VBlank has started
        _graphicManager.notifyVBlankStarted();
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::processPreRenderLine() {
    // Only VRAM access if rendering is enabled
    if (isRenderingEnabled()) {
        // Complete tile data is 8 cycles
        uint8_t dataType = _currentPixel & 0x7;
        
        // Process tiles
        processTiles(dataType);
        
        // Process sprites
        processSprites(dataType);
        
        // Check for Y address copy
        if ((_currentPixel >= ModelConstants::firstYCopyAddressPeriodPixel) && (_currentPixel <= ModelConstants::lastYCopyAddressPeriodPixel)) {
            copyYOnAddress();
        }
    }
    
    // Clear flags if necessary
    if (_currentPixel == ModelConstants::firstActivePixel) {
        _statusSpriteOverflow = false;
        _statusSprite0Hit = false;
        _vBlankStartedLatch = false;
        
        // Signal which is set when reset line is low and reset at the end of VBlank
        // See https://wiki.nesdev.com/w/index.php/PPU_power_up_state
        _resetRequested = !_resetLine;
        
        // Toggle odd / even frame (in Visual2C02 it is toggled on scanline 256 pixel 0 !!! but it's the same here and we don't lose performance for another check for 256, 0)
        _oddFrame = !_oddFrame;
    }
    // On odd frame, PPU skip scanline 261 pixel 340 cycle
    else if (_currentPixel == (ModelConstants::totalPixelsPerScanlineCount - 2)) {//TODO: 338 si le iowrite est avant processRendering:pas sur maintenant
        // Possibly skip last cycle (safe to convert bool to int, false = 0, true = 1)
        _currentPixel += _oddFrame && isRenderingEnabled();
    }
}


// *** Tiles ***

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::processTiles(uint8_t dataType) {
    // Fetch tiles if on the period (no need to check for first tile fetch first period and last tile fetch second period because there are in the boundary of _currentPixel)
    if ((_currentPixel <= ModelConstants::lastTileFetchFirstPeriodPixel) || (_currentPixel >= ModelConstants::firstTileFetchSecondPeriodPixel)) {
        fetchTiles(dataType);
    }
    
    // Period of tile shift registers is the same that process tiles period but 1 cycle later except that it stops after first unused NT fetch
    if (((_currentPixel >= (ModelConstants::firstTileFetchFirstPeriodPixel + 1)) && (_currentPixel <= (ModelConstants::lastTileFetchFirstPeriodPixel + 1))) || ((_currentPixel >= (ModelConstants::firstTileFetchSecondPeriodPixel + 1)) && (_currentPixel <= ModelConstants::firstUnusedNTFetchPeriodPixel))) {
        updateTileShiftRegisters(dataType);
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::fetchTiles(uint8_t dataType) {
    // Correct dataType for first cycle (Unused low tile BG set address)
    if (_currentPixel == ModelConstants::idlePixel) {
        dataType = FetchStep::LowTileByteSetAddress;
    }
    // Correct dataType for last cycles (Unused NT fetches)
    else if (_currentPixel >= ModelConstants::firstUnusedNTFetchPeriodPixel) {
        dataType = ((dataType - 1) & 0x1) + 1;    // TODO: c'est a cause de ca que le test mmc3 scanline_timing foire, car en lisant ici a 0x2000 pour les cycles apres 336 ca refait un A12 = 0 assez longtemps pour que le MMC le prenne en compte, voir dans Visual2C02 ce que ca fait exactement durant ces cycles : en fait on dirait qu'il fait bien ca mais qu'apres au pixel 0 il fait le FetchStep::LowTileByteSetAddress, mais a confirmer et voir si ca le fait aussi en odd / even
    }
    // Increment Y address
    else if (_currentPixel == ModelConstants::incrementYAddressPixel) {
        incrementYOnAddress();
    }
    
    // Nametable set address
    if (dataType == FetchStep::NTByteSetAddress) {
        _bus.setAddressBus(0x2000 | (_address & 0x0FFF));
    }
    // Nametable fetch (NT byte)
    else if (dataType == FetchStep::NTByteReadData) {
        _ntByte = read();
    }
    // Attribute table set address
    else if (dataType == FetchStep::ATByteSetAddress) {
        _bus.setAddressBus(0x23C0 | (_address & 0x0C00) | ((_address >> 4) & 0x38) | ((_address >> 2) & 0x7));
    }
    // Attribute table fetch (AT byte)
    else if (dataType == FetchStep::ATByteReadData) {
        _atByte = read();
    }
    // First pattern table set address
    else if (dataType == FetchStep::LowTileByteSetAddress) {
        // One tile is 16 bytes (ntByte << 4), one byte is a 8 pixels row (fineY = _address >> 12)
        _bus.setAddressBus(_controlBackgroundPatternTableAddress | (_ntByte << 4) | (_address >> 12));
    }
    // First pattern table fetch (Low tile byte)
    else if (dataType == FetchStep::LowTileByteReadData) {
        _lowTileByte = read();
    }
    // Second pattern table set address
    else if (dataType == FetchStep::HighTileByteSetAddress) {
        // Same than low BG tile address but 8 bytes after
        _bus.setAddressBus(_controlBackgroundPatternTableAddress | (_ntByte << 4) | 0x8 | (_address >> 12));
    }
    // Second pattern table fetch (High tile byte)
    else {
        _highTileByte = read();
        
        // Current tile fetch is over, go to next tile
        incrementXOnAddress();
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::updateTileShiftRegisters(uint8_t dataType) {
    // Get tile out bits
    _bgLowTileBitOut = (_bgLowTileShiftRegister >> _fineXScroll) & 0x1;
    _bgHighTileBitOut = (_bgHighTileShiftRegister >> _fineXScroll) & 0x1;
    
    // Get attribute out bits
    _bgLowAttributeBitOut = (_bgLowAttributeShiftRegister >> _fineXScroll) & 0x1;
    _bgHighAttributeBitOut = (_bgHighAttributeShiftRegister >> _fineXScroll) & 0x1;
    
    // Shift right tile registers and inject 1 on the MSB
    _bgLowTileShiftRegister >>= 1;
    _bgHighTileShiftRegister >>= 1;
    
    // Shift right attribute registers and inject latch on the MSB
    _bgLowAttributeShiftRegister = (_bgLowAttributeLatch << 7) | (_bgLowAttributeShiftRegister >> 1);
    _bgHighAttributeShiftRegister = (_bgHighAttributeLatch << 7) | (_bgHighAttributeShiftRegister >> 1);
    
    // Check if need to update registers
    if (dataType == 0x1) {
        // Update tile registers (need to reverse patterns byte because registers are shifted right and first pixel is left)
        _bgLowTileShiftRegister = (Common::reverseBits(_lowTileByte) << 8) | (_bgLowTileShiftRegister & 0xFF);
        _bgHighTileShiftRegister = (Common::reverseBits(_highTileByte) << 8) | (_bgHighTileShiftRegister & 0xFF);
        
        // Get two bits AT for current tile (be careful, no AND 0x3 is performed here because the AND is performed on the next lines for latchs)
        uint8_t atForCurrentTile = _atByte >> (((_address - 1) & 0x2) | ((_address >> 4) & 0x4)); // TODO: voir si bon par rapport a l'adresse comme on lit pour les prochains tiles : TODO: PQ - 1 ???
        
        // Update attribute latches
        _bgLowAttributeLatch = atForCurrentTile & 0x1;
        _bgHighAttributeLatch = (atForCurrentTile >> 1) & 0x1;
    }
}


// *** Sprites ***

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::processSprites(uint8_t dataType) {
    // Exit if we are no more in process sprites period
    if ((_currentPixel < ModelConstants::firstSecondClearOamPeriodPixel) || (_currentPixel > ModelConstants::lastSpritesFetchPeriodPixel)) {
        return;
    }
    
    // If we start second OAM clear period
    if (_currentPixel == ModelConstants::firstSecondClearOamPeriodPixel) {
        startClearSecondOAM();
    }
    
    // If we are in second OAM clear period
    if (_currentPixel <= ModelConstants::lastSecondClearOamPeriodPixel) {
        clearSecondOAM();
        return;
    }
    
    // If we start sprite evaluation period
    if (_currentPixel == ModelConstants::firstEvaluateSpritesPeriodPixel) {
        startEvaluateSprites();
    }
    
    // If we are in sprite evaluation period
    if (_currentPixel <= ModelConstants::lastEvaluateSpritesPeriodPixel) {
        evaluateSprites();
        return;
    }
    
    // If we start sprite fetch period
    if (_currentPixel == ModelConstants::firstSpritesFetchPeriodPixel) {
        startFetchSprites();
    }
    
    // We are in sprite fetch period
    fetchSprites(dataType);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::startClearSecondOAM() {
    // We use the same code that evaluate sprites to clear OAM, simply do a 32 bytes copy (_oamAddress doesn't change and read to OAM returns $FF in this period)
    _spriteEvaluationCopyByteCount = 32;
    
    // Normally it happen on pixel 0 and not 1 but due to the even/odd pixel 0 jump, it is put here
    // Reset second OAM address
    resetSecondOAMAddress();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::clearSecondOAM() {
    // Reset OAM address overflow
    _oamAddressOverflow = false;
    
    // Reset sprite overflow
    _spriteOverflow = false;// TODO: voir si dessous, je l'ai mis ici a la place car apparement dans visual2C02 ca le fait a pixel 1 mais je ne sais pas si ca le fait toute la clear period ou juste au pixel 1 ! : // TODO: besoin de ca car j'utilisais _statusSpriteOverflow dans evaluate pour checker si le scanline courant avait un overflow de sprites mais _statusSpriteOverflow est par frame (reseté au vblank) et pas par scanline !!! TODO voir via visual 2C02 si ce signal existe : peut etre end_of_oam_or_sec_oam_overflow, il est reseté au pixel 1 et pas ici et il est set apres les 8 sprites en evaluation plus exactement des que sec_oam_overflow est set
    
    // Clear second OAM using same circuit than evaluate sprites
    clearSecondOAMAndEvaluateSprites();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::startEvaluateSprites() {
    // Bug in 2C02 which cause copy of some portion of OAM when OAMADDR > 7 // TODO: et faire des tests visual2C02, surement moyen d'emuler ca exactement en faisant le process de refresh OAM (voir ci dessous)
    // See https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation
    // See https://wiki.nesdev.com/w/index.php/Errata#OAM_and_Sprites
    // See http://forums.nesdev.com/viewtopic.php?f=3&t=12407
    if (_oamAddress >= 0x8) {
        for (int i = 0; i < 8; i++) {
            _objectAttributeMemory[i] = _objectAttributeMemory[(_oamAddress & 0xF8) + i];
        }
    }
    
    // Reset second OAM address
    resetSecondOAMAddress();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::evaluateSprites() {
    // Evalute sprites
    clearSecondOAMAndEvaluateSprites();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::clearSecondOAMAndEvaluateSprites() {
    // TODO : Because OAM is implemented with dynamic RAM instead of static RAM, the data stored in OAM memory will quickly begin to decay into random bits if it is not being refreshed. The OAM memory is refreshed once per scanline while rendering is enabled (if either the sprite or background bit is enabled via the register at $2001), but on an NTSC PPU this refresh is prevented whenever rendering is disabled.
    // TODO: quand et comment fait il ca ? 256 read suivi de write ? de 1 a 256 (inclus) ??
    
    // TODO: moyen de simplifier tout ca en checkant ici if !isInRenderScanline on fait le read from second OAM (en read et en write cycle) sinon on fait le traitement normal et on peut retirer tout les isInRenderScanline check (mais attention ca ne fait pas exactement la meme chose que le code actuel, il faut voir pour le _statusSpriteOverflow set, aussi pour le _needIncrementOAMAddress (non car il est reset quand increment donc ok), il faut voir aussi pour _sprite0OnNextScanline, _oamAddressIncrement
    
    // ** Read cycle **
    if (_currentPixel & 0x1) {
        // Read OAM at current OAM address
        if (isInRenderScanline()) {
            readObjectAttributeMemory();
        } else {
            // Read from second OAM
            _oamData = _secondObjectAttributeMemory[_secondOAMAddress];
        }
        
        // Status sprite overflow is set on the read cycle if overflow found on the previous write cycle
        if (_spriteOverflow) {//TODO: voir si dans isInRenderScanline ou non (spriteOverflow ne peut etre set que quand il est en render scanline mais imaginons qu'on desactive le rendu exactement a ce cycle, est ce que le status flag va etre set quand meme ?
            _statusSpriteOverflow = true;
        }
        
        return;
    }
    
    // ** Write cycle **
    
    // Only increment OAM address if we are in render scanline and if we are not in second OAM clear period
    _needIncrementOAMAddress = isInRenderScanline() && (!isInSecondOAMClearPeriod());
    
    // If Y byte (We can't rely on _currentPixel because there is not in sync with the copy/no copy pattern of evaluation, we can't rely on _oamAddress because it can be misaligned due to a previous write to $2003 and we can't rely on _secondOAMAddress because once it has been overflow, it stays to 0 but we need to perform the "copy" 4 bytes of the ninth sprite)
    if (_spriteEvaluationCopyByteCount == 0) {
        // Check if Y is in scanline range and not in sprite overflow (if ninth sprite has been found, it only increment _oamAddress by 4 until it enters in hblank, same behaviour if _oamAdddress has overflow (in case of _oamAddress != 0 at the beginning of the evaluation))
        if (((_currentScanline - _oamData) < _controlSpriteSize) && (!_spriteOverflow) && (!_oamAddressOverflow) && isInRenderScanline()) {// TODO: j'ai rajouté isInRenderScanline pour eviter de rentrer ici en pre-render car currentScanline - oamData (s'il vaut au moins 246 (261-246 = 15) avec sprite=16pixels) sera true
            // Check for sprite 0 in range ('sprite 0' write cycle)
            if (_currentPixel == (ModelConstants::firstEvaluateSpritesPeriodPixel + 1)) {
                _sprite0OnNextScanline = true;
            }
            
            // Check for sprite overflow
            if (_secondOAMAddressOverflow) {    // TODO: il y a un bug dans le 2C02 qui incremente _oamAddress de 5 si Y n'est pas dans le range, voir si ca incremente aussi _secondOAMAddress de 1 !!! : non _secondOAMAddress ne bouge pas et si le byte checké apres est dans le range ca incremente de 1 et une fois tout lu, _oamAddress n'incremente pas la derniere fois (pour garder la valeur alignée avec 4) pour etre plus précis il aligne la valeur incrémentée avec 4 et ne fait pas l'incrémentation de 4 apres. -> Ensuite il incremente de 4 a chaque fois (que le byte soit dans le range ou non)
                // TODO: si le byte checké par après n'est pas dans le range, ca réincremente de 5 !!!
                _spriteOverflow = true;
            }
            
            // Need to copy sprite bytes, so increment _oamAddress by 1 and increment also _secondOAMAddress by 1
            _spriteEvaluationCopyByteCount = 4;
            _oamAddressIncrement = 1;
            _needIncrementSecondOAMAddress = true;
        } else {
            // Check for sprite 0 in range ('sprite 0' write cycle)
            if (_currentPixel == (ModelConstants::firstEvaluateSpritesPeriodPixel + 1)) {
                // It's not a problem to do that on pre-render line too
                _sprite0OnNextScanline = false;
            }
            
            // Need to go to next sprite, so increment _oamAddress by 4 and don't increment _secondOAMAddress
            // There is a bug in 2C02 that increment by 4 + by 1 (so by 5) _oamAddress when _secondOAMAddress has overflow and while the fetched Y byte is not in range, once one fetched Y byte is in range, it set the status sprite overflow and the increment goes to 1 then 4 once all 3 other bytes of the ninth sprite are "copied"
            _spriteEvaluationCopyByteCount = 1;
            _oamAddressIncrement = ((!_secondOAMAddressOverflow) || _spriteOverflow) ? 4 : 5;
            //_oamAddressIncrement = 4 + (_secondOAMAddressOverflow && (!_spriteOverflow)); // TODO: voir la meilleure ecriture
        }
    } else {
        _needIncrementSecondOAMAddress = true;
    }
    
    // If none of the OAM address overflow
    if (isInRenderScanline() && (!_oamAddressOverflow) && (!_secondOAMAddressOverflow)) {
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

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::startFetchSprites() {
    // Reset second OAM address
    resetSecondOAMAddress();
    
    // This signal is reset on pixel 1 in Visual2C02 but here we using it differently so we reset it here to reproduce the sprite 0 hit bug at 255
    _sprite0Active = false;
    
    // Copy X on address
    copyXOnAddress();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::fetchSprites(uint8_t dataType) {
    // OAMADDR is set to 0 during each of ticks 257-320 (the sprite tile loading interval) of the pre-render and visible scanlines
    _oamAddress = 0;
    
    // Save _sprite0OnNextScanline on _sprite0OnCurrentScanline (during 257-320) because it can be modified during rendering of the current scanline
    _sprite0OnCurrentScanline = _sprite0OnNextScanline;
    
    // Sprite number start at 0 at beginning of sprite fetch and a sprite take 8 cycles, we can't use (_secondOAMAddress >> 2) because if rendering is disabled/reenabled in fetch sprites, _secondOAMAddress is not incremented during render disabled but spriteNumber is incremented
    uint8_t spriteNumber = (_currentPixel - ModelConstants::firstSpritesFetchPeriodPixel) >> 3;
    
    // Garbage nametable set address + load sprite Y byte
    if (dataType == FetchStep::NTByteSetAddress) {
        _bus.setAddressBus(0x2000 | (_address & 0x0FFF));
        
        // TODO: je n'arrive pas a trouver ntByte ni le fetchY fetchIndex signal dans visual2C02 !!!
        _spriteY = _secondObjectAttributeMemory[_secondOAMAddress];
        _needIncrementSecondOAMAddress = true;
    }
    // Garbage nametable fetch (NT byte) + load sprite tile index byte
    else if (dataType == FetchStep::NTByteReadData) {
        _ntByte = read();
        
        _spriteTileIndex = _secondObjectAttributeMemory[_secondOAMAddress];
        _needIncrementSecondOAMAddress = true;
    }
    // Garbage attribute table set address + load sprite attribute byte
    else if (dataType == FetchStep::ATByteSetAddress) {
        _bus.setAddressBus(0x2000 | (_address & 0x0FFF));//TODO: verifier que c bien cette adresse malgré que c un attribute table fetch
        
        _spAttributeLatches[spriteNumber] = _secondObjectAttributeMemory[_secondOAMAddress];
        _needIncrementSecondOAMAddress = true;
    }
    // Garbage attribute table fetch (NT byte) + load sprite X byte
    else if (dataType == FetchStep::ATByteReadData) {
        _ntByte = read();
        
        _spXPositionCounters[spriteNumber] = _secondObjectAttributeMemory[_secondOAMAddress];//TODO: + voir si quand on lit dans le secondObjectAttributeMemory si on change _oamData : oui, vois si possible d'intégrer ca dans readObjectAttributeMemory (et write ?)
        _needIncrementSecondOAMAddress = true;
    }
    // First pattern table set address
    else if (dataType == FetchStep::LowTileByteSetAddress) {
        // Get address for sprite
        _bus.setAddressBus(getAddressForFetchSprites(spriteNumber));
    }
    // First pattern table fetch (Low tile byte)
    else if (dataType == FetchStep::LowTileByteReadData) {
        _spLowTileShiftRegisters[spriteNumber] = getDataForFetchSprites(spriteNumber);
    }
    // Second pattern table set address
    else if (dataType == FetchStep::HighTileByteSetAddress) {
        // Same than low BG tile address but 8 bytes after
        _bus.setAddressBus(getAddressForFetchSprites(spriteNumber) | 0x8);
    }
    // Second pattern table fetch (High tile byte)
    else {
        _spHighTileShiftRegisters[spriteNumber] = getDataForFetchSprites(spriteNumber);
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
uint16_t Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::getAddressForFetchSprites(uint8_t spriteNumber) {
    uint8_t spriteRow = _currentScanline - _spriteY;
    uint8_t tileIndex = _spriteTileIndex;
    
    // Flip vertically if necessary
    if ((_spAttributeLatches[spriteNumber] & 0x80) != 0x0) {
        spriteRow = (_controlSpriteSize - 1) - spriteRow;
    }
    
    uint16_t patternTableAddress;
    
    // If 8x16 sprites
    if (_controlSpriteSize == 16) {
        // Pattern table address is the LSB of tileIndex
        patternTableAddress = (tileIndex & 0x1) << 12;
        tileIndex &= 0xFE;
        
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

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::getDataForFetchSprites(uint8_t spriteNumber) {
    // Read data (even if unused sprite)
    uint8_t data = read();
    
    // If unused sprite, all pixels are transparent
    if ((_currentScanline - _spriteY) >= _controlSpriteSize) {
        data = 0x0;
    }
    
    // Flip horizontally if necessary (in hardware it is reversed if it is NOT flipped because the shift register out the right pixel first)
    if ((_spAttributeLatches[spriteNumber] & 0x40) == 0x0) {
        data = Common::reverseBits(data);
    }
    
    return data;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::updateSpriteShiftRegisters() {
    // Sprites loop
    bool spriteFound = false;
    
    for (int spriteNumber = 0; spriteNumber < 8; ++spriteNumber) {
        // If sprite not ready, decrement its X counter then go to next sprite
        if (_spXPositionCounters[spriteNumber] > 0) {
            --_spXPositionCounters[spriteNumber];
            continue;
        }
        
        // Only shift registers if render enabled
        if (!isRenderingEnabled()) {    // TODO: normalement c'est ainsi d'apres les tests Visual2C02 meme si ce n'est pas coherent car si on commence a shifter apres si on reactive le rendu, les sprites seront desynchronisé par rapport a la position courante du pixel
            continue;
        }
        
        // Shift registers
        uint8_t spLowTileBitOut =_spLowTileShiftRegisters[spriteNumber] & 0x1;
        uint8_t spHighTileBitOut =_spHighTileShiftRegisters[spriteNumber] & 0x1;
        _spLowTileShiftRegisters[spriteNumber] >>= 1;
        _spHighTileShiftRegisters[spriteNumber] >>= 1;
        
        // If sprite already found or pixel transparent, skip this sprite
        if (spriteFound || ((spLowTileBitOut | spHighTileBitOut) == 0x0)) {
            continue;
        }
        
        // Save sprite data
        _spLowTileBitOut = spLowTileBitOut;
        _spHighTileBitOut = spHighTileBitOut;
        _spAttributeLatchActive = _spAttributeLatches[spriteNumber];
        _sprite0Active = _sprite0OnCurrentScanline && (spriteNumber == 0);
        
        // Set flag
        spriteFound = true;
    }
    
    // If no sprite found
    if (!spriteFound) {
        // Initialize sprite data with 0
        _spLowTileBitOut = 0;
        _spHighTileBitOut = 0;
        _spAttributeLatchActive = 0;
        //_sprite0Active = false;   // TODO: je le reset en 257 pour avoir le bug du sprite 0 hit en 255, le reste doit etre reset ici car il est possible de ne pas les mettre a 0 si pas de sprite trouvé dans le cas ou il y avait des sprites trouvés avant
    }
}


// *** Pixel rendering ***

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::processPixel() {
    // The process pixel period is 2-257
    if ((_currentPixel < ModelConstants::firstProcessPixelPeriodPixel) || (_currentPixel > ModelConstants::lastProcessPixelPeriodPixel)) {
        // No output if no in process pixel period
        _currentPixelIndex = _extBackgroundIndex;        // TODO: voir si ca ou background palette hack ou 0 ?
        
        return;
    }
    
    // Current pixel to draw is delayed by two (Shift registers starting to shift at pixel 2, so we can use these bit out informations only starting at _currentPixel == 2)
    unsigned int pixelNumber = _currentPixel - ModelConstants::firstProcessPixelPeriodPixel;
    
    if (isRenderingEnabled()) {
        // Calculate pixel
        _currentPixelIndex = calculatePixel(pixelNumber);
    } else {
        // Rendering disabled, use background color (if VRAM address points to palette during forced vblank, there is the background palette hack)
        _currentPixelIndex = (_address >= 0x3F00) ? (_address & 0xFF) : _extBackgroundIndex;
    }
    
    // Get indexed color from palette index memory
    uint8_t pixelColor = readPaletteIndexMemory(_currentPixelIndex);// TODO: normalement il y a un delai de 2 cycles pour récuperer la couleur et plotter le pixel courant !!!
    
    // Plot pixel
    _graphicManager.plotPixel(pixelNumber, _currentScanline, pixelColor, _maskEmphasizeRed, _maskEmphasizeGreen, _maskEmphasizeBlue);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::calculatePixel(uint8_t pixelNumber) {
    // Get BG pixel
    uint8_t bgPixel = (_maskShowBackground && (_maskShowBackgroundFirst8px || (pixelNumber >= 8))) ? ((_bgHighTileBitOut << 1) | _bgLowTileBitOut) : 0x0;  // TODO: on peut simplifier ce test en ayant une variable qui est le pixel min a commencer a afficher qui sera 0 si showBackground == true et 256 si showBackground = false et 8 si showBackground == true mais maskShowBackgroundFirst8px == false : cette variable devra etre initialisée dans le write du mask register car c'est lui qui contient les données
    
    // Get sprite pixel
    uint8_t spPixel = (_maskShowSprites && (_maskShowSpritesFirst8px || (pixelNumber >= 8))) ? ((_spHighTileBitOut << 1) | _spLowTileBitOut) : 0x0; // TODO: pareil qu'au dessus
    
    // Check for sprite 0 hit
    checkSprite0Hit(bgPixel, spPixel);
    
    // Check if sprite pixel has priority
    if ((spPixel != 0x0) && (((_spAttributeLatchActive & 0x20) == 0x0) || (bgPixel == 0x0))) {
        // 0x10 for sprite palettes
        return 0x10 | ((_spAttributeLatchActive & 0x3) << 2) | spPixel;
    }
    
    // Check if background pixel has priority
    if (bgPixel != 0x0) {
        return (_bgHighAttributeBitOut << 3) | (_bgLowAttributeBitOut << 2) | bgPixel;
    }
    
    // No pixel has priority, use BG ($3F00)
    return _extBackgroundIndex;  // TODO: voir si ok + SI le spPixel ou bgPixel == 0 (si la palette = 0) est ce qu'on retourne 0 ou bien _extBackgroundIndex ??? si on doit retourner _extBackgroundIndex, alors remettre 0 a la place de _extBackgroundIndex partout et dans getColorFromPalette checker si l'index 0 et si oui utiliser l'index _extBackgroundIndex
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::checkSprite0Hit(uint8_t bgPixel, uint8_t spPixel) {
    // Don't check if sprite 0 is not active
    if (!_sprite0Active) {
        return;
    }
    
    // A sprite 0 hit occurs if both pixels are non zero, _sprite0Active assume that current sprite pixel is from sprite 0 and is not transparent but we still need spPixel because spPixel take care of the mask register (show (8 first pixels) sprites)
    if ((bgPixel != 0x0) && (spPixel != 0x0)) {
        _statusSprite0Hit = true;
    }
}


// *** State ***

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::incrementXOnAddress() {
    _address = ((_address & 0x7FE0) | ((_address + 1) & 0x1F)) ^ ((((_address & 0x1F) + 1) & 0x20) << 5);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::incrementYOnAddress() {  // TODO: voir si plus optimisé
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
    _address = (_address & 0x0C1F) | coarseYScrollInAddress;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::copyXOnAddress() {
    _address = (_address & 0x7BE0) | (_temporaryAddress & 0x41F);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::copyYOnAddress() {
    _address = (_address & 0x41F) | (_temporaryAddress & 0x7BE0);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::incrementOAMAddress() {
    // Continue to increment even if overflow
    
    // Increment (always by 1 if we are not in rendering period)
    uint8_t increment = (isInRenderingPeriod()) ? _oamAddressIncrement : 1;
    _oamAddress += increment;
    
    // Check overflow (_oamAddress is uint8_t so it returns to its value - 255 after 255)
    if (_oamAddress < increment) {
        // Set overflow flag
        _oamAddressOverflow = true;
    }
    
    // Align if necessary
    /*if (_oamAddressIncrement == 4) {
        _oamAddress &= ~0x3;
    }*/
    if ((increment > 4) && ((_oamAddress & 0x3) == 0x0)) {//TODO: voir pour ameliorer ca
        _oamAddress -= 4;
    }
    
    // Reset increment OAM address increment flag
    _needIncrementOAMAddress = false;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::resetSecondOAMAddress() {
    // Reset second OAM address (because it can be overflow and it can be lower than 0x1F, no matter its value, it is reset to 0)
    _secondOAMAddressOverflow = false;
    _secondOAMAddress = 0;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::incrementSecondOAMAddress() {
    // Don't increment if overflow
    if (_secondOAMAddressOverflow) {
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

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::incrementPositionCounters() {
    // Increment pixel counter
    ++_currentPixel;
    
    // Check boundary of pixel counter
    if (_currentPixel < ModelConstants::totalPixelsPerScanlineCount) {
        return;
    }
    
    // Start a new line, reset pixel counter and increment scanline counter
    _currentPixel = 0;
    ++_currentScanline;
    
    // Check boundary of scanline counter
    if (_currentScanline < ModelConstants::totalScanlinePerFrameCount) {
        return;
    }
    
    // Start a new frame, reset scanline counter
    _currentScanline = 0;
}


// *** IO ***

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
template <class TConnectedBus>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::ioRead() {
    // Get bus
    TConnectedBus &ioBus = ioGetBus<TConnectedBus>();
    
    // Get address
    uint16_t address = ioBus.getAddressBus() & 0x7;
    
    // Status register
    if (address == IORegisters::Status) {
        // Unused data lines stay at capacitance latch value
        _dataBusCapacitanceLatch &= 0x1F;
        
        // Update register with status
        _dataBusCapacitanceLatch |= _statusSpriteOverflow << 5;
        _dataBusCapacitanceLatch |= _statusSprite0Hit << 6;
        _dataBusCapacitanceLatch |= _statusVBlankStarted << 7;
        
        // Reset VBlank started flag
        _vBlankStartedLatch = false;
        
        // Reset write toggle
        _writeToggle = false;
    }
    // OAM data register
    else if (address == IORegisters::OamData) {
        // Read OAM
        readObjectAttributeMemory();
        
        // Get data
        // After doing some tests on Visual2C02, it seems that PPU drives unused bits low and there is no open bus behaviour implicated
        // Test : testprogram = [ 0x2000, 0x2100, 0x2300, 0x24FF, 0x0018, 0x24FF, 0x0018, 0x24AA, 0x0018, 0x24FF, 0x0018, 0x2302, 0x2514, 0x3400];
        _dataBusCapacitanceLatch = _oamData;
    }
    // Data register
    else if (address == IORegisters::Data) {
        // Get data read buffer if no race condition else read return last data bus (which was address bus low)
        uint8_t oldDataReadBuffer = (_read2007RaceConditionCycle == 0) ? _dataReadBuffer : _read2007LastBusData;
        
        // Update read 2007 race condition informations (3+1 PPU cycles) TODO: comment savoir si 3+1 ou 6 cycles? peut etre via les unofficials opcodes
        _read2007RaceConditionCycle = 4;
        _read2007LastBusData = _bus.getDataBus();
        
        // Set address bus
        _bus.setAddressBus(_address);    // TODO: normalement ne fait pas tout ca sur le meme cycle PPU (il faut un cycle pour setter l'adresse, et un autre pour lire)
        
        // Read data
        _dataReadBuffer = read();
        
        // If palette index address, read to it directly ( see https://wiki.nesdev.com/w/index.php/PPU_registers#PPUDATA )
        if ((_address & 0x3FFF) > 0x3EFF) {
            // Read from palette memory (Only 6 lower bits, 2 upper bits from open bus behaviour)
            _dataBusCapacitanceLatch = (_dataBusCapacitanceLatch & 0xC0) | readPaletteIndexMemory(_address & 0xFF);
        } else {
            // Read return the content of the data read buffer, data read buffer is then updated with the read value
            _dataBusCapacitanceLatch = oldDataReadBuffer;
        }
        
        // Increment address
        _address += _controlAddressIncrementPerCpuAccess;   // TODO: surement changer par un signal needIncrement : TODO est ce qu'on incremente aussi si on lit dans la palette ??? : oui normalement : voir pour uniformiser ca et incrementXOnAddress et incrementYOnAddress si possible
        _address &= 0x7FFF; // A15 unused inside PPU
        
        // Set address bus
        _bus.setAddressBus(_address);//TODO: j'ai besoin de ca pour que le test MMC3 A12_clocking passe, donc peut etre que _address est _addressBus  tout le temps !!! : non car dans Visual2C02 les valeurs sont souvent différentes !!!
    }
    
    // Set data on bus
    ioBus.setDataBus(_dataBusCapacitanceLatch);
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
template <class TConnectedBus>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::ioWrite() {
    // Get bus
    TConnectedBus &ioBus = ioGetBus<TConnectedBus>();
    
    // Get address
    uint16_t address = ioBus.getAddressBus() & 0x7;
    
    // Get data on bus
    uint8_t data = ioBus.getDataBus();
    
    // When data is written, data bus capacitance latch is filled with the value even if the write occurs on a read-only register
    _dataBusCapacitanceLatch = data;
    
    // Control register
    if (address == IORegisters::Control) {
        _temporaryAddress = (_temporaryAddress & 0x73FF) | ((data & 0x3) << 10);
        _controlAddressIncrementPerCpuAccess = ((data & 0x4) != 0x0) ? 32 : 1;//((data & 0x4) << 3) | (((data & 0x4) >> 2) ^ 0x1);
        _controlSpritePatternTableAddress = (data & 0x8) << 9;
        _controlBackgroundPatternTableAddress = (data & 0x10) << 8;
        _controlSpriteSize = ((data & 0x20) != 0x0) ? 16 : 8; // (((data & 0x20) >> 1) | (((data & 0x20) >> 2) ^ 0x8));
        _controlMasterSlaveSelect = (data & 0x40) != 0x0;
        _controlGenerateNmiForVBlank = (data & 0x80) != 0x0;
    }
    // Mask register
    else if (address == IORegisters::Mask) {
        _maskGrayscale = (data & 0x1) ? 0x30 : 0x3F;
        _maskShowBackgroundFirst8px = (data & 0x2) != 0x0;
        _maskShowSpritesFirst8px = (data & 0x4) != 0x0;
        _maskShowBackground = (data & 0x8) != 0x0;
        _maskShowSprites = (data & 0x10) != 0x0;
        _maskEmphasizeRed = (data & 0x20) != 0x0;
        _maskEmphasizeGreen = (data & 0x40) != 0x0;
        _maskEmphasizeBlue = (data & 0x80) != 0x0;
    }
    // OAM address register
    else if (address == IORegisters::OamAddress) {
        _oamAddress = data;
        
        // TODO: voir pour les bugs (2C02 seulement)
    }
    // OAM data register
    else if (address == IORegisters::OamData) {
        // Can only be written during VBlank, see https://wiki.nesdev.com/w/index.php/PPU_registers
        /*if ((isRenderingEnabled()) && (_currentScanline < (ModelConstants::visibleScanlinePerFrameCount + ModelConstants::postRenderLengthInScanline))) {
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
        //_oamAddressIncrement = 1;// TODO: je l'ai mis ici car en DMA il etait = 4 mais peut etre le remettre a 1 en debut de vblank ? en tout cas il ne faut pas le mettre ici car voir les comments ci dessus : c'etait un bug dans incrementOamAddress, voir quand meme avec visual2C02
        _needIncrementOAMAddress = true;// TODO: normalement incrementOamAddress(); mais voir quand reseter _oamAddressIncrement (le mettre a 1)
    }
    // Scroll register
    else if (address == IORegisters::Scroll) {
        // First write
        if (!_writeToggle) {
            // Set scroll X on temporary address
            _temporaryAddress = (_temporaryAddress & 0x7FE0) | (data >> 3);
            _fineXScroll = data & 0x7;
        }
        // Second write
        else {
            // Set scroll Y on temporary address
            _temporaryAddress = (_temporaryAddress & 0xC1F) | ((data & 0x7) << 12) | ((data & 0xF8) << 2);
        }
        
        // Toggle write
        _writeToggle = !_writeToggle;
    }
    // Address register
    else if (address == IORegisters::Address) {
        // First write
        if (!_writeToggle) {
            // Set high byte address on temporary address
            _temporaryAddress = ((data & 0x3F) << 8) | (_temporaryAddress & 0xFF);
        }
        // Second write
        else {
            // Set low byte address on temporary address
            _temporaryAddress = (_temporaryAddress & 0x7F00) | data;
            
            // Copy temporary to address after 3 cycles
            _copyAddressCycleDelay = 3;   //TODO: voir si 1, 2 ou 3 (normalement 3)
        }
        
        // Toggle write
        _writeToggle = !_writeToggle;
    }
    // Data register
    else if (address == IORegisters::Data) {
        // If palette index address, write to it
        if ((_address & 0x3FFF) > 0x3EFF) {
            // Write to palette memory
            writePaletteIndexMemory(_address & 0xFF, data);
        } else {
            // Set address bus
            _bus.setAddressBus(_address);
            
            // Write data
            write(data);
        }
        
        // Increment address
        _address += _controlAddressIncrementPerCpuAccess;   // TODO: surement changer par un signal needIncrement : TODO est ce qu'on incremente aussi si on ecrit dans la palette ??? : oui normalement : voir pour uniformiser ca et incrementXOnAddress et incrementYOnAddress si possible
        _address &= 0x7FFF; // A15 unused
        
        // Set address bus
        _bus.setAddressBus(_address);//TODO: j'ai besoin de ca pour que le test MMC3 A12_clocking passe, donc peut etre que _address est _addressBus  tout le temps !!! : non car dans Visual2C02 les valeurs sont souvent différentes !!!
    }
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
template <class TConnectedBus>
TConnectedBus &Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::ioGetBus() const {
    // Safe conversion because it is called by ioRead / ioWrite which contains the correct TConnectedBus type (because their saved method address are for this type)
    return *static_cast<TConnectedBus *>(_ioBus);
}


// *** External memory ***

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::read() const {
    // Perform read on the bus
    _bus.performRead();
    
    // Get data on the bus
    return _bus.getDataBus();
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::write(uint8_t data) const {
    // Set data on the bus
    _bus.setDataBus(data);
    
    // Perform write on the bus
    _bus.performWrite();
}


// *** Internal memory ***

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::readObjectAttributeMemory() {
    // If we are in second OAM period, we must read $FF instead of real data
    _oamData = (!isInSecondOAMClearPeriod()) ? _objectAttributeMemory[_oamAddress] : 0xFF;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::writeObjectAttributeMemory(uint8_t data) {
    // If we are in second OAM phase and rendering is enabled, we must write $FF instead of data
    _oamData = (!isInSecondOAMClearPeriod()) ? data : 0xFF;
    
    // Write data to OAM (if attribute byte, need to AND with $E3 because bit 2, 3 and 4 are unused)
    // It's the correct behaviour according to Visual2C02 tests, it could be optimized by doing the IF and AND in the read method instead (it's more likely to be written that to be read) but let it like true behaviour
    _objectAttributeMemory[_oamAddress] = ((_oamAddress & 0x3) == 0x2) ? (_oamData & 0xE3) : _oamData;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
uint8_t Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::readPaletteIndexMemory(uint8_t address) const {
    // Addresses $10/$14/$18/$1C are mirrors of $00/$04/$08/$0C
    uint8_t mirroredAddress = address & (((address & 0x13) == 0x10) ? 0xF : 0x1F);
    
    // Read from palette index memory
    return _paletteIndexMemory[mirroredAddress] & _maskGrayscale;
}

template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
void Chip<EModel, TBus, TInterruptHardware, TGraphicManager>::writePaletteIndexMemory(uint8_t address, uint8_t data) {
    // Addresses $10/$14/$18/$1C are mirrors of $00/$04/$08/$0C
    uint8_t mirroredAddress = address & (((address & 0x13) == 0x10) ? 0xF : 0x1F);
    
    // Write to palette index memory (only 6 bits)
    _paletteIndexMemory[mirroredAddress] = data & 0x3F;
}

#endif /* NESEmu_Ppu_s_hpp */
