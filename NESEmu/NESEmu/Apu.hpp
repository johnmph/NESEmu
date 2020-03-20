//
//  Apu.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 18/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_hpp
#define NESEmu_Apu_hpp

// See https://wiki.nesdev.com/w/index.php/APU
// See http://nesdev.com/apu_ref.txt

#include <cstdint>


namespace NESEmu { namespace Apu {
    
    template <class TInterruptHardware, class TSoundHardware>
    struct Chip {
        
        Chip(TInterruptHardware &interruptHardware, TSoundHardware &soundHardware);
        
        // Power up
        void powerUp();
        
        // Clock
        void clock();
        
        // Reset
        void reset(bool high);
        
        // Channels
        void setChannelRegister(uint16_t address, uint8_t data);
        
        // Status
        uint8_t getStatusRegister();
        void setStatusRegister(uint8_t data);
        
        // Frame counter
        void setFrameCounterRegister(uint8_t data);
        
    private:
        
        // See https://wiki.nesdev.com/w/index.php/APU_Envelope
        
        struct EnvelopeUnit {
            
            void clock();
            
            void start();
            
            // Output
            uint8_t getVolume() const;
            
            // Parameters
            void setDividerPeriodOrConstantVolume(uint8_t value);
            void setLoopMode(bool loopMode);
            void setConstantVolumeMode(bool constantVolumeMode);
            
        private:
            
            uint8_t _dividerPeriodOrConstantVolume;
            uint8_t _counter;
            uint8_t _decayCounter;
            bool _startFlag;
            bool _loopMode;
            bool _constantVolumeMode;
        };
        
        // See https://wiki.nesdev.com/w/index.php/APU_Sweep
        
        //template <bool BNegateOneComplement>
        struct SweepUnit {
            
            SweepUnit(bool oneComplementMode, uint16_t &channelPeriod);
            
            void clock();
            
            void reload();
            
            // Output
            bool getOutput() const;
            uint16_t getTargetPeriod() const;
            
            // Properties
            void setPeriod(uint8_t period);
            void setShiftCount(uint8_t shiftCount);
            void setEnabled(bool enabled);
            void setNegate(bool negate);
            
        private:
            
            uint16_t &_channelPeriod;
            uint8_t _counter;
            uint8_t _period;
            uint8_t _shiftCount;
            bool const _oneComplementMode;
            bool _enabled;
            bool _reloadFlag;
            bool _negate;
        };
        
        // See https://wiki.nesdev.com/w/index.php/APU_Length_Counter
        
        struct LengthCounter {
            
            void clock();
            
            // Output
            bool getOutput() const;
            
            // Properties
            void setEnabled(bool enabled);
            void setHalt(bool halt);
            void setValueIndex(uint8_t index);
            
        private:
            
            static uint8_t const _lengthValues[32];
            
            uint8_t _counter;
            bool _enabled;
            bool _halt;
        };
        
        // See https://wiki.nesdev.com/w/index.php/APU_Frame_Counter
        
        struct FrameCounter {
            // TODO: voir pour reset et power up
            FrameCounter(Chip &chip);
            
            void clock();
            
            void requestReset();
            
            // Properties
            bool getInterrupt() const;
            void resetInterrupt();
            
            void setDisableInterrupt(bool disableInterrupt);
            void setSequence5StepMode(bool sequence5StepMode);
            
        private:
            
            void clock4StepSequence();
            void clock5StepSequence();
            
            void clockQuarterFrame();
            void clockHalfFrame();
            
            
            Chip &_chip;
            uint16_t _counter;
            bool _interrupt;
            bool _disableInterrupt;
            bool _sequence5StepMode;
            uint8_t _resetDelay;
            bool _needToReset;
        };
        
        // See https://wiki.nesdev.com/w/index.php/APU_Pulse
        
        struct Pulse {  // TODO: dans des classes en dehors ? dans d'autres fichiers ? (Avec un dossier Apu)
            
            Pulse(bool sweepOneComplementMode);
            
            void clock();
            
            void clockFrameCounterQuarterFrame();
            void clockFrameCounterHalfFrame();
            
            void reset();
            
            // Output
            uint8_t getOutput() const;
            
            // Register
            void setRegister(uint8_t registerNumber, uint8_t data);
            
            bool getLengthCounterOutput() const;
            void setLengthCounterEnabled(bool enabled);
            
        private:
            
            static uint8_t const _dutyWaveforms[4];
            
            EnvelopeUnit _envelopeUnit;
            SweepUnit _sweepUnit;
            LengthCounter _lengthCounter;
            uint16_t _timer;
            uint16_t _counter;
            uint8_t _currentStep;  // TODO: voir par apres pour l'implementer avec un shift register plutot (a chaque sequence on shift a droite et on recupere le bit sorti comme etant la sequence courante)
            uint8_t _waveform;
        };
        
        // See https://wiki.nesdev.com/w/index.php/APU_Triangle
        
        struct Triangle {
            // Output
            uint8_t getOutput() const;
            
            LengthCounter lengthCounter;
        };
        
        // See https://wiki.nesdev.com/w/index.php/APU_Noise
        
        struct Noise {
            // Output
            uint8_t getOutput() const;
            
            EnvelopeUnit envelopeUnit;
            LengthCounter lengthCounter;
        };
        
        // See https://wiki.nesdev.com/w/index.php/APU_DMC
        
        struct Dmc {
            
            // Output
            uint8_t getOutput() const;
            
        };
        
        // See https://wiki.nesdev.com/w/index.php/APU_Mixer
        float getMixedOutput() const;
        
        void setPulseChannelRegister(uint8_t pulseChannelNumber, uint8_t registerNumber, uint8_t data);
        void setTriangleChannelRegister(uint8_t registerNumber, uint8_t data);
        void setNoiseChannelRegister(uint8_t registerNumber, uint8_t data);
        void setDmcChannelRegister(uint8_t registerNumber, uint8_t data);
        
        void clockFrameCounterQuarterFrame();
        void clockFrameCounterHalfFrame();
        
        void checkInterrupt();
        
        
        // Channels
        Pulse _pulse[2];
        Triangle _triangle;
        Noise _noise;
        Dmc _dmc;
        
        // Status
        bool _statusEnableDmc;//TODO: a voir
        
        // Frame counter
        FrameCounter _frameCounter;
        
        // Internals
        TInterruptHardware &_interruptHardware;
        TSoundHardware &_soundHardware;
        bool _oddCycle;
        
    };
    
    #include "Apu_s.hpp"
    
} }

#endif /* NESEmu_Apu_hpp */
