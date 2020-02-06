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
        
        void clock();
        
        void reset(bool high);
        
        // TODO: il y a un int (interrupt) qui appelle le nmi du cpu, donc avoir une methode setInterruptFunction qui recoit un lambda que la nes configurera pour appeler le nmi, peut etre que c'est possible de le faire au compile-time (passer une lambda en template parameter ?) -> surement pas car on appelle ca de l'objet cpu !!! -> un std function car il faut le sauver ? c'est lourd, peut etre un simple pointeur de fonction si possible
        
        // TODO: avoir une methode setPixelPlotterFunction() qui prend un template parameter plotPixel qui est une fonction (lambda ou autre) et cette methode appelera la fonction plotPixel ainsi : plotPixel(_currentPixel, _currentScanline, calculatedColor); ainsi on peut découpler le PPU et donc la NES du systeme graphique de l'OS de l'émulateur
        // TODO: il faudra donc aussi une methode setStartingVBlankNotifierFunction() pour notifier que le vblank commence pour par exemple afficher l'image (qui a été calculée via un buffer de pixel setté par plotPixel)
        
        // Memory (for CPU accessing)
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        
        // Ext pins
        void exts(uint8_t data);
        uint8_t getExts() const;
        
    private:
        
        using Constants = Constants<EModel>;
        
        static constexpr int totalPixelsPerScanlineCount = Constants::visiblePixelsPerScanlineCount + Constants::hBlankLengthInPixel;
        static constexpr int totalScanlinePerFrameCount = Constants::visibleScanlinePerFrameCount + Constants::postRenderLengthInScanline + Constants::vBlankLengthInScanline;
        
        struct ObjectAttribute {    // TODO: voir si besoin
            uint8_t yPosition;
            uint8_t tileIndex;
            uint8_t attributes;
            uint8_t xPosition;
        };
        
        void processVisibleFrameLine();
        void processPostRenderLine();
        void processVBlankLine();
        void processPreRenderLine();
        
        void incrementPositionCounters();
        
        void fetchTilesData();
        void evaluateSprites();
        
        void incrementXOnAddress();
        void incrementYOnAddress();
        
        void checkInterrupt();
        
        bool isRenderingEnabled() const;
        
        // OAM (64 x 4 bytes of sprites informations)
        //std::vector<ObjectAttribute> _objectAttributeMemory;
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
        bool _controlSpriteSize8x16px;
        bool _controlMasterSlaveSelect;
        bool _controlGenerateNmiForVBlank;
        
        bool _maskGrayscale;
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
        
        int _currentPixel;
        int _currentScanline;
        bool _oddFrame;
        
        uint16_t _temporaryAddress;
        uint8_t _fineXScroll;
        bool _writeToggle;
        
        uint16_t _addressBus;
        
        uint8_t _ntByte;
        uint8_t _atByte;
        uint8_t _lowBGTileByte;
        uint8_t _highBGTileByte;
        
        uint16_t _lowBGTileShiftRegister;
        uint16_t _highBGTileShiftRegister;
        
        //uint8_t _ // 2 shift register de 8 bits pour les attributes, a voir
        
        uint8_t _clearSecondOAMReadOverwrite;
        
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