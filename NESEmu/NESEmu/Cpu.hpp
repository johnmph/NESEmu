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
#include "Cpu6502/Chip.hpp"
#include "Apu/Chip.hpp"


namespace NESEmu { namespace Cpu {
    
    enum class Model {
        Ricoh2A03,
        Ricoh2A07
    };
    
    template <Model EModel>
    struct Constants;
    
    template <Model EModel, class TBus, class TSoundHardware>
    struct Chip : protected Cpu6502::Chip<Cpu6502::ConfigurationPerformance<Chip<EModel, TBus, TSoundHardware>, false>> {
        
        Chip(TBus &bus, TSoundHardware &soundHardware);
        
        void powerUp(uint16_t programCounter = 0x100F, uint8_t stackPointer = 0x0, uint8_t accumulator = 0x0, uint8_t xIndex = 0x0, uint8_t yIndex = 0x0, uint8_t statusFlags = 0x34);  // TODO: d'apres la rom de tests ce sont les valeurs que ca doit avoir au power up, a verifier (pour le pc je ne sais pas)
        
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
        
        // TODO: exposer tous ce qu'il y a dans les pins
        //bool getAd1Signal() const;
        //bool getAd2Signal() const;
        Cpu6502::ReadWrite getReadWriteSignal() const;
        uint8_t getOutSignal() const;
        bool getOe1Signal() const;
        bool getOe2Signal() const;
        bool getM2Signal() const;
        
    //private:
    protected:  // TODO: needed protected for unit test, else we can set it private
        
        using Constants = Constants<EModel>;
        using InternalCpu = Cpu6502::Chip<Cpu6502::ConfigurationPerformance<Chip, false>>;
        
        // Set Cpu as friend to keep data bus methods private
        friend InternalCpu;
        
        // Set Apu as friend to keep apuIrq method private
        friend Apu::Chip<Chip, TSoundHardware>;
        
        
        // Bus intermediate
        uint16_t getAddressBus() const;
        void setAddressBus(uint16_t address);
        
        uint8_t getDataBus() const;
        void setDataBus(uint8_t data);
        void setDataBus(uint8_t data, uint8_t mask);
        
        void performRead();
        void performWrite();
        
        // DMA
        bool checkDmaPhi1();
        bool checkDmaPhi2();
        void startDma(uint8_t address);
        void stopDma();
        
        // APU IRQ
        void apuIrq(bool high);
        
        // Internal
        Apu::Chip<Chip, TSoundHardware> _apu;
        TBus &_bus;
        
        bool _dmaStarted;
        int _dmaCount;
        uint8_t _dmaAddress;
        bool _dmaToggle;
        
        uint8_t _outLatch;
        
        bool _irqLine;
        bool _apuIrqLine;
    };
    
    #include "Cpu_s.hpp"
    
} }

#endif /* NESEmu_Cpu_hpp */
