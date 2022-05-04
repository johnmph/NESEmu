//
//  Cpu.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 23/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cpu_hpp
#define NESEmu_Cpu_hpp

// See https://wiki.nesdev.com/w/index.php/2A03
// See https://wiki.nesdev.com/w/index.php/CPU_pin_out_and_signal_description

#include <cstdint>
#include "../Cpu6502/Chip.hpp"
#include "Dma.hpp"
#include "Apu/Chip.hpp"


namespace NESEmu { namespace Cpu {
    
    enum class Model {
        Ricoh2A03,
        Ricoh2A07
    };
    
    template <Model EModel>
    struct Constants;
    
    template <Model EModel, class TBus, class TControllerHardware, class TSoundManager>
    struct Chip : protected Cpu6502::Chip<Cpu6502::ConfigurationPerformance<Chip<EModel, TBus, TControllerHardware, TSoundManager>, false>> {
        
        Chip(TBus &bus, TControllerHardware &controllerHardware, TSoundManager &soundManager);
        
        void powerUp(uint16_t programCounter = 0x100F, uint8_t stackPointer = 0x0, uint8_t accumulator = 0x0, uint8_t xIndex = 0x0, uint8_t yIndex = 0x0, uint8_t statusFlags = 0x24);  // TODO: d'apres la rom de tests ce sont les valeurs que ca doit avoir au power up, a verifier (pour le pc je ne sais pas)
        
        void clock();
        void clockPhi1();
        void clockPhi2();
        void startPhi1();
        void endPhi1();
        void startPhi2();
        void endPhi2();
        
        void reset(bool high);
        void nmi(bool high);
        void irq(bool high);
        //void test(bool high);
        
        // Ad1/Ad2 are analogic signals
        float getAd1Signal() const;
        float getAd2Signal() const;
        Cpu6502::ReadWrite getReadWriteSignal() const;
        uint8_t getOutSignal() const;
        bool getOe1Signal() const;
        bool getOe2Signal() const;
        bool getM2Signal() const;
        
    //private:
    protected:  // TODO: needed protected for unit test, else we can set it private
        
        using ModelConstants = Constants<EModel>;
        using InternalCpu = Cpu6502::Chip<Cpu6502::ConfigurationPerformance<Chip, false>>;
        
        // Set Cpu as friend to keep data bus methods private
        friend InternalCpu;
        
        // Set Dma as friend to keep members private
        friend Dma<Chip>;
        
        // Set Apu as friend to keep apuIrq and apuRequestDmcSample methods private
        friend Apu::Chip<Chip, TSoundManager>;
        
        
        // Bus intermediate
        uint16_t getAddressBus() const;
        void setAddressBus(uint16_t address);
        
        uint8_t getDataBus() const;
        void setDataBus(uint8_t data);
        void setDataBus(uint8_t data, uint8_t mask);
        
        void performRead();
        void performWrite();
        
        // APU
        void apuIrq(bool high);
        void apuDmcRequestSample(uint16_t address);
        void apuDmcSampleFetched();
        
        // Internal
        Apu::Chip<Chip, TSoundManager> _apu;
        Dma<Chip> _dma;
        TBus &_bus;
        TControllerHardware &_controllerHardware;
        
        // Bus of 6502
        uint16_t _6502BusAddress;        // TODO: voir si mettre ainsi ou renommer ou avoir une classe InternalBus, ou le mettre dans le Dma ? a voir : pas dans le Dma car il est utilisé aussi lors de l'accès a l'APU
        uint8_t _6502BusData;
        uint8_t _last4015DataWritten;
        
        Cpu6502::ReadWrite _readWrite;
        
        // Out latch
        uint8_t _outLatch;
        
        // Irq
        bool _irqLine;
        bool _apuIrqLine;
    };
    
    #include "Cpu_s.hpp"
    
} }

#endif /* NESEmu_Cpu_hpp */
