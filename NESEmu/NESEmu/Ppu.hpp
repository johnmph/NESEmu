//
//  Ppu.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 21/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Ppu_hpp
#define NESEmu_Ppu_hpp

// See https://wiki.nesdev.com/w/index.php/PPU
// See https://wiki.nesdev.com/w/index.php/PPU_pin_out_and_signal_description
// See http://www.qmtpro.com/~nes/chipimages/visual2c02
// For palette generation, see https://bisqwit.iki.fi/utils/nespalette.php and http://drag.wootest.net/misc/palgen.html

#include <cstdint>
#include <vector>
#include "Common.hpp"


namespace NESEmu { namespace Ppu {
    
    enum class Model {
        Ricoh2C02,
        Ricoh2C07
    };
    
    template <Model EModel>
    struct ModelConstants;
    
    template <Model EModel>
    struct Constants {
        static constexpr unsigned int visiblePixelsPerScanlineCount = ModelConstants<EModel>::visiblePixelsPerScanlineCount;
        static constexpr unsigned int hBlankLengthInPixel = ModelConstants<EModel>::hBlankLengthInPixel;
        static constexpr unsigned int visibleScanlinePerFrameCount = ModelConstants<EModel>::visibleScanlinePerFrameCount;
        static constexpr unsigned int postRenderLengthInScanline = ModelConstants<EModel>::postRenderLengthInScanline;
        static constexpr unsigned int vBlankLengthInScanline = ModelConstants<EModel>::vBlankLengthInScanline;
        static constexpr unsigned int preRenderLengthInScanline = ModelConstants<EModel>::preRenderLengthInScanline;
        
        static constexpr unsigned int firstRenderPeriodScanline = 0;
        static constexpr unsigned int lastRenderPeriodScanline = visibleScanlinePerFrameCount - 1;
        
        static constexpr unsigned int firstPostRenderPeriodScanline = lastRenderPeriodScanline + 1;
        static constexpr unsigned int lastPostRenderPeriodScanline = (firstPostRenderPeriodScanline + postRenderLengthInScanline) - 1;
        
        static constexpr unsigned int firstVBlankPeriodScanline = lastPostRenderPeriodScanline + 1;
        static constexpr unsigned int lastVBlankPeriodScanline = (firstVBlankPeriodScanline + vBlankLengthInScanline) - 1;
        
        static constexpr unsigned int firstPreRenderPeriodScanline = lastVBlankPeriodScanline + 1;
        static constexpr unsigned int lastPreRenderPeriodScanline = (firstPreRenderPeriodScanline + preRenderLengthInScanline) - 1;
        
        static constexpr unsigned int totalPixelsPerScanlineCount = visiblePixelsPerScanlineCount + hBlankLengthInPixel;
        static constexpr unsigned int totalScanlinePerFrameCount = lastPreRenderPeriodScanline + 1;
        
        static constexpr unsigned int idlePixel = 0;
        static constexpr unsigned int firstActivePixel = 1;
        
        static constexpr unsigned int firstTileFetchFirstPeriodPixel = firstActivePixel;
        static constexpr unsigned int lastTileFetchFirstPeriodPixel = 256;
        
        static constexpr unsigned int firstTileFetchSecondPeriodPixel = 321;
        static constexpr unsigned int lastTileFetchSecondPeriodPixel = 340;
        
        static constexpr unsigned int firstSecondClearOamPeriodPixel = firstActivePixel;
        static constexpr unsigned int lastSecondClearOamPeriodPixel = 64;
        
        static constexpr unsigned int firstEvaluateSpritesPeriodPixel = lastSecondClearOamPeriodPixel + 1;
        static constexpr unsigned int lastEvaluateSpritesPeriodPixel = 256;
        
        static constexpr unsigned int firstSpritesFetchPeriodPixel = lastEvaluateSpritesPeriodPixel + 1;
        static constexpr unsigned int lastSpritesFetchPeriodPixel = 320;
        
        static constexpr unsigned int firstProcessPixelPeriodPixel = 2;
        static constexpr unsigned int lastProcessPixelPeriodPixel = 257;
        
        static constexpr unsigned int firstYCopyAddressPeriodPixel = 280;
        static constexpr unsigned int lastYCopyAddressPeriodPixel = 304;
        
        static constexpr unsigned int firstUnusedNTFetchPeriodPixel = 337;
        static constexpr unsigned int incrementYAddressPixel = 256;
    };
    
    namespace IORegisters {
        enum : uint8_t {
            Control,
            Mask,
            Status,
            OamAddress,
            OamData,
            Scroll,
            Address,
            Data
        };
    }
    
    namespace FetchStep {
        enum : uint8_t {
            HighTileByteReadData,
            NTByteSetAddress,
            NTByteReadData,
            ATByteSetAddress,
            ATByteReadData,
            LowTileByteSetAddress,
            LowTileByteReadData,
            HighTileByteSetAddress,
        };
    }
    
    template <Model EModel, class TBus, class TInterruptHardware, class TGraphicManager>
    struct Chip {
        
        Chip(TBus &bus, TInterruptHardware &interruptHardware, TGraphicManager &graphicManager);
        
        // Power pins
        void powerUp();
        
        // Clock pin
        void clock();
        
        // Reset pin
        void reset(bool high);
        
        // Ext pins
        void exts(uint8_t data);
        void getExts(uint8_t &data) const;
        
        // IO access
        template <class TConnectedBus>
        void readPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void writePerformed(TConnectedBus &connectedBus);
        
    private:
        
        // Constants
        using Constants = Constants<EModel>;
        
        
        // Helper
        bool isInRenderScanline() const;
        bool isInPostRenderScanline() const;
        bool isInVBlankScanline() const;
        bool isInPreRenderScanline() const;
        bool isRenderingEnabled() const;
        bool isInRenderingPeriod() const;
        bool isInSecondOAMClearPeriod() const;
        
        // Main
        void processRendering();
        void processIO();
        void checkInterrupt();
        void updateState();
        void checkReset();
        
        // Rendering
        void processRenderLine();
        void processPostRenderLine();
        void processVBlankLine();
        void processPreRenderLine();
        
        // Tiles
        void processTiles(uint8_t dataType);
        void fetchTiles(uint8_t dataType);
        void updateTileShiftRegisters(uint8_t dataType);
        
        // Sprites
        void processSprites(uint8_t dataType);
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
        
        // Pixel rendering
        void processPixel();
        uint8_t calculatePixel(uint8_t pixelNumber);
        void checkSprite0Hit(uint8_t bgPixel, uint8_t spPixel);
        
        // State
        void incrementXOnAddress();
        void incrementYOnAddress();
        void copyXOnAddress();
        void copyYOnAddress();
        void incrementOAMAddress();
        void resetSecondOAMAddress();
        void incrementSecondOAMAddress();
        void incrementPositionCounters();
        
        // IO
        template <class TConnectedBus>
        void ioRead();
        
        template <class TConnectedBus>
        void ioWrite();
        
        template <class TConnectedBus>
        TConnectedBus &ioGetBus() const;
        
        // External memory
        uint8_t read() const;
        void write(uint8_t data) const;
        
        // Internal memory
        void readObjectAttributeMemory();   // TODO: voir pour le mettre const mais alors il faut sortir _oamData de la, voir comment generaliser pour les read write en differentes periodes (clear , evaluation, fetch), io, rendering on off, ...
        void writeObjectAttributeMemory(uint8_t data);
        uint8_t readPaletteIndexMemory(uint8_t address) const;
        void writePaletteIndexMemory(uint8_t address, uint8_t data);
        
        
        // OAM (64 x 4 bytes of sprites informations)
        std::vector<uint8_t> _objectAttributeMemory;
        
        // Second OAM (8 x 4 bytes of sprites informations)
        std::vector<uint8_t> _secondObjectAttributeMemory;  // TODO: comme petit, voir si pas en stack plutot que dynamic
        
        // Palette indexes
        std::vector<uint8_t> _paletteIndexMemory;   // TODO: comme petit, voir si pas en stack plutot que dynamic
        
        // Registers
        uint8_t _oamAddress;
        uint8_t _oamData;
        uint16_t _address;
        
        uint8_t _controlAddressIncrementPerCpuAccess;
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
        TGraphicManager &_graphicManager;
        
        bool _resetLine;
        bool _resetRequested;
        
        uint8_t _currentPixelIndex;
        uint8_t _extBackgroundIndex;
        
        unsigned int _currentScanline;
        unsigned int _currentPixel;
        bool _oddFrame;
        
        uint16_t _temporaryAddress;
        uint8_t _fineXScroll;
        bool _writeToggle;
        
        bool _vBlankStartedLatch;
        
        // Tiles
        uint8_t _ntByte;
        uint8_t _atByte;
        uint8_t _lowTileByte;
        uint8_t _highTileByte;
        
        uint16_t _bgLowTileShiftRegister;
        uint16_t _bgHighTileShiftRegister;
        uint8_t _bgLowTileBitOut;
        uint8_t _bgHighTileBitOut;
        uint8_t _bgLowAttributeShiftRegister;
        uint8_t _bgHighAttributeShiftRegister;
        uint8_t _bgLowAttributeLatch;
        uint8_t _bgHighAttributeLatch;
        uint8_t _bgLowAttributeBitOut;
        uint8_t _bgHighAttributeBitOut;
        
        // Sprites
        uint8_t _spLowTileShiftRegisters[8];
        uint8_t _spHighTileShiftRegisters[8];
        uint8_t _spAttributeLatches[8];
        uint8_t _spXPositionCounters[8];
        uint8_t _spriteY;
        uint8_t _spriteTileIndex;
        uint8_t _spLowTileBitOut;
        uint8_t _spHighTileBitOut;
        uint8_t _spAttributeLatchActive;
        
        bool _spriteOverflow;
        bool _sprite0OnNextScanline;
        bool _sprite0OnCurrentScanline;
        bool _sprite0Active;
        
        // OAM
        uint8_t _secondOAMAddress;
        uint8_t _oamAddressIncrement;
        uint8_t _spriteEvaluationCopyByteCount;
        bool _oamAddressOverflow;
        bool _secondOAMAddressOverflow;
        bool _needIncrementOAMAddress;
        bool _needIncrementSecondOAMAddress;
        
        // IO
        void *_ioBus;
        void (Chip::*_ioMethod)();
        uint8_t _ioPending;
        //bool _ioReadMode;   // TODO: seulement besoin si on veut mettre le write avant processRendering sinon pas besoin de différencier car on a _ioMethod
        
        // Dynamic latch due to capacitance of very long traces of data bus that run to various parts of the PPU
        // See https://wiki.nesdev.com/w/index.php/PPU_registers
        // And https://wiki.nesdev.com/w/index.php/Open_bus_behavior
        uint8_t _dataBusCapacitanceLatch; // TODO: emuler le decay aussi ? (apres une frame et depend du pattern)
        
        // Data read buffer
        // See https://wiki.nesdev.com/w/index.php/PPU_registers#PPUDATA
        uint8_t _dataReadBuffer;
    };
    
    #include "Ppu_s.hpp"
    
} }

#endif /* NESEmu_Ppu_hpp */
