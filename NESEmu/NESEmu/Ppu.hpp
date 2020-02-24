//
//  Ppu.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 21/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Ppu_hpp
#define NESEmu_Ppu_hpp

#include <cstdint>
#include <vector>
#include "Common.hpp"


namespace NESEmu { namespace Ppu {
    
    enum class Model {
        Ricoh2C02,
        Ricoh2C07
    };
    
    template <Model EModel>
    struct Constants;
    
    template <Model EModel, class TBus, class TInterruptHardware, class TGraphicHardware>
    struct Chip {    // TODO: par cycle de PPU, un pixel est calculé
        
        Chip(TBus &bus, TInterruptHardware &interruptHardware, TGraphicHardware &graphicHardware);
        
        void powerUp();
        
        void clock();
        
        void reset(bool high);
        
        // IO access
        template <class TConnectedBus>
        void readPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void writePerformed(TConnectedBus &connectedBus);
        
        // Ext pins
        void exts(uint8_t data);
        void getExts(uint8_t &data) const;
        
    private:
        
        using Constants = Constants<EModel>;
        
        static constexpr int totalPixelsPerScanlineCount = Constants::visiblePixelsPerScanlineCount + Constants::hBlankLengthInPixel;
        static constexpr int totalScanlinePerFrameCount = Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline + Constants::vBlankLengthInScanline;
        
        struct ObjectAttribute {    // TODO: voir si besoin, peut etre plutot un enum avec des indexs?
            uint8_t yPosition;
            uint8_t tileIndex;
            uint8_t attributes;
            uint8_t xPosition;
        };
        
        void checkReset();
        
        // VRAM access
        uint8_t read(/*uint16_t address*/);             // TODO: renommer readFromBus et writeToBus et pareil dans le cpu ? (et renommer ioRead/write en read/write) ?
        void write(/*uint16_t address, */uint8_t data);
        
        bool isInRenderScanline() const;
        bool isInPostRenderScanline() const;
        bool isInVBlankScanline() const;
        bool isInPreRenderScanline() const;
        bool isRenderingEnabled() const;
        bool isInRenderingPeriod() const;
        bool isInSecondOAMClearPeriod() const;
        
        void processLine();
        void updateState();
        
        void processRenderLine();
        void processPostRenderLine();
        void processVBlankLine();
        void processPreRenderLine();
        
        void processPixel();
        
        void processTiles(uint8_t dataType);
        void processSprites(uint8_t dataType);
        
        void fetchTiles(uint8_t dataType);
        void updateTileShiftRegisters(uint8_t dataType);
        
        void startClearSecondOAM();
        void clearSecondOAM();
        void startEvaluateSprites();
        void evaluateSprites();
        void clearSecondOAMAndEvaluateSprites();
        void startFetchSprites();
        void fetchSprites(uint8_t dataType);
        uint16_t getAddressForFetchSprites(uint8_t spriteNumber);
        uint8_t getDataForFetchSprites(uint8_t spriteNumber);
        void updateSpriteShiftRegisters();
        
        void incrementXOnAddress();
        void incrementYOnAddress();
        
        void incrementOAMAddress();
        void resetSecondOAMAddress();
        void incrementSecondOAMAddress();
        
        void incrementPositionCounters();
        
        uint8_t calculatePixel(uint8_t pixelNumber);
        void checkSprite0Hit(uint8_t bgPixel);
        
        uint8_t getColorFromPaletteIndex(uint8_t index);
        
        void readObjectAttributeMemory();
        void writeObjectAttributeMemory(uint8_t data);
        
        uint8_t readPaletteIndexMemory(uint8_t address);
        void writePaletteIndexMemory(uint8_t address, uint8_t data);
        
        void checkInterrupt();
        
        // OAM (64 x 4 bytes of sprites informations)
        std::vector<uint8_t> _objectAttributeMemory;
        
        // Second OAM (8 x 4 bytes of sprites informations)
        std::vector<uint8_t> _secondObjectAttributeMemory;  // TODO: comme petit, voir si pas en stack plutot que dynamic
        
        // Palette indexes
        std::vector<uint8_t> _paletteIndexMemory;   // TODO: comme petit, voir si pas en stack plutot que dynamic
        
        // TODO: la nes a 2ko de VRAM mais le PPU n'y accede pas directement mais via un controlleur dans la cartouche, ce qu'il fait que la cartouche peut mapper les adresses VRAM sur d'autres mémoires donc ne pas mettre la vram ici
        // Registers
        uint8_t _oamAddress;
        uint16_t _address;
        
        uint8_t _controlAddressIncrementPerCpuAccess;       // TODO: par apres reorganiser les variables pour l'alignement
        uint16_t _controlSpritePatternTableAddress;
        uint16_t _controlBackgroundPatternTableAddress;
        uint8_t _controlSpriteSize;
        bool _controlMasterSlaveSelect;
        bool _controlGenerateNmiForVBlank;
        
        uint8_t _maskGrayscale;
        bool _maskShowBackgroundFirst8px;
        bool _maskShowSpritesFirst8px;
        bool _maskShowBackground;
        bool _maskShowSprites;
        bool _maskEmphasizeRed;
        bool _maskEmphasizeGreen;
        bool _maskEmphasizeBlue;
        
        bool _statusSpriteOverflow;
        bool _statusSprite0Hit;
        bool _statusVBlankStarted;
        
        // Internal
        TBus &_bus;
        TInterruptHardware &_interruptHardware;
        TGraphicHardware &_graphicHardware;
        
        bool _resetLine;
        bool _resetRequested;
        
        unsigned int _currentPixel;
        unsigned int _currentScanline;
        bool _oddFrame;
        
        bool _vBlankStartedLatch;
        
        uint16_t _temporaryAddress;
        uint8_t _fineXScroll;
        bool _writeToggle;
        
        //uint16_t _addressBus;
        // Data bus not really exists on the PPU but on a external octal latch
        //uint8_t _externalOctalLatch;  // TODO: surement mettre ca dans le PPUBUS de Nes
        
        uint8_t _ntByte;
        uint8_t _atByte;
        uint8_t _lowTileByte;
        uint8_t _highTileByte;
        
        uint16_t _bgLowTileShiftRegister;
        uint16_t _bgHighTileShiftRegister;
        uint8_t _bgLowTileBitOut;   // TODO : voir si bool ou uint8_t pour l'optimisation
        uint8_t _bgHighTileBitOut;
        uint8_t _bgLowAttributeShiftRegister;
        uint8_t _bgHighAttributeShiftRegister;
        uint8_t _bgLowAttributeLatch;   // TODO : voir si bool ou uint8_t pour l'optimisation
        uint8_t _bgHighAttributeLatch;
        uint8_t _bgLowAttributeBitOut;
        uint8_t _bgHighAttributeBitOut;
        
        uint8_t _spLowTileShiftRegisters[8];
        uint8_t _spHighTileShiftRegisters[8];
        uint8_t _spAttributeLatches[8];
        uint8_t _spXPositionCounters[8];
        uint8_t _spriteY;
        uint8_t _spriteTileIndex;
        uint8_t _spLowTileBitOut;
        uint8_t _spHighTileBitOut;
        uint8_t _spAttributeLatchActive;
        
        bool _sprite0OnNextScanline;
        bool _sprite0OnCurrentScanline;
        bool _sprite0Active;
        
        uint8_t _currentPixelIndex;
        uint8_t _extBackgroundIndex;
        
        // OAM
        uint8_t _oamData;
        uint8_t _secondOAMAddress;
        uint8_t _oamAddressIncrement;
        uint8_t _spriteEvaluationCopyByteCount;
        bool _oamAddressOverflow;
        bool _secondOAMAddressOverflow;
        bool _needIncrementOAMAddress;
        bool _needIncrementSecondOAMAddress;
        
        // Dynamic latch due to capacitance of very long traces of data bus that run to various parts of the PPU
        // See https://wiki.nesdev.com/w/index.php/PPU_registers
        // And https://wiki.nesdev.com/w/index.php/Open_bus_behavior
        uint8_t _dataLatch; // TODO: emuler le decay aussi ? (apres une frame et depend du pattern)
        
        // Data read buffer
        // See https://wiki.nesdev.com/w/index.php/PPU_registers#PPUDATA
        uint8_t _dataReadBuffer;
    };
    
    #include "Ppu_s.hpp"
    
} }

#endif /* NESEmu_Ppu_hpp */
