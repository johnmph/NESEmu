//
//  Cpu2A03VisualUnitTest.mm
//  NESEmuUnitTest
//
//  Created by Jonathan Baliko on 7/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#import <XCTest/XCTest.h>
#include <iostream>
#include <array>
#include <fstream>
#include <memory>
#include "Cpu2A03FullAccess.hpp"
#include "VisualUnitTest/Analyzer.hpp"
#include "VisualUnitTest/Cpu2A03/Attributes.hpp"


@interface Cpu2A03VisualUnitTest : XCTestCase

@end

@implementation Cpu2A03VisualUnitTest

namespace {
    
    struct ControllerHardware {
        
        void clock(uint16_t address, Cpu6502::ReadWrite readWrite) {
        }
        
        uint8_t getOutLatch() const {
            return 0;
        }
        
        void setOutLatch(uint8_t data) {
        }
        
        uint8_t readControllerPort(unsigned int number, uint8_t dataBus) {
            return 0;
        }
        
    };
    
    struct SoundManager {
        
        bool askForAddingSample() const {
            return false;
        }
        
        void addSample(uint8_t value) {
        }
        
    };
    
    struct Bus {
        uint8_t read(uint16_t address) {
            //std::cout << std::hex << "Read 0x" << static_cast<int>(memory[address]) << " at 0x" << address << "\n";
            return memory[address];
        }
        
        void write(uint16_t address, uint8_t data) {
            //std::cout << std::hex << "Write 0x" << static_cast<int>(data) << " at 0x" << address << "\n";
            memory[address] = data;
        }
        
        uint16_t getAddressBus() const {
            return _address;
        }
        
        void setAddressBus(uint16_t address) {
            _address = address;
        }
        
        uint8_t getDataBus() const {
            return _data;
        }
        
        void setDataBus(uint8_t data) {
            _data = data;
        }
        
        void setDataBus(uint8_t data, uint8_t mask) {
            _data = (_data & ~mask) | (data & mask);
        }
        
        void performRead() {
            //std::cout << std::hex << "Read 0x" << static_cast<int>(memory[_address]) << " at 0x" << _address << "\n";
            _data = memory[_address];
        }
        
        void performWrite() {
            //std::cout << std::hex << "Write 0x" << static_cast<int>(_data) << " at 0x" << _address << "\n";
            memory[_address] = _data;
        }
        
        void readControllerPort(unsigned int index) {
        }
        
        std::array<uint8_t, 1024 * 64> memory;
        uint16_t _address;
        uint8_t _data;
    };
    
    
    Bus bus;
    ControllerHardware controllerHardware;
    SoundManager soundManager;
    Cpu2A03FullAccess<NESEmu::Cpu::Model::Ricoh2A03, Bus, ControllerHardware, SoundManager> cpu2A03(bus, controllerHardware, soundManager);
    
}

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
    
    // Initialize with BRK
    bus.memory.fill(0x00);
    
    // Power up cpu
    cpu2A03.powerUp(0x100F, 0xC0, 0x0, 0xC0, 0x0, 0x22);
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testFile:(NSString *)filename {
    // By default filename must be in /private/tmp path, so a BUILD_DIR Other C flag is added to get the build dir
    
    // Open Visual6502 Log
    std::ifstream ifsLog(std::string(BUILD_DIR) + "/UnitTestFiles/" + [filename UTF8String]);
    
    // Check that file exists
    XCTAssertTrue(ifsLog.good());
    
    // Create analyzer
    VisualUnitTest::Analyzer<Cpu2A03FullAccess<NESEmu::Cpu::Model::Ricoh2A03, Bus, ControllerHardware, SoundManager>> analyzer(ifsLog, [](std::string const &name) { return VisualUnitTest::Cpu2A03::attributeFactory<decltype(cpu2A03)>(name); }, [](uint16_t address, uint8_t data) { bus.write(address, data); });
    
    // Release reset to start cpu
    cpu2A03.reset(true);
    
    // We need to sync with Visual6502 by adding nine first clocks to exit the reset state
    for (int i = 0; i < 9; ++i) {
        cpu2A03.clock();
    }
    
    // Analyze
    analyzer.analyze(cpu2A03, [self](bool result, std::string const &name) { if (!result) { std::cout << name << "\n"; } XCTAssertTrue(result); }, 18);
    
    // Close file
    ifsLog.close();
}

- (void)testDma {
    [self testFile:@"Dma.txt"];
}

- (void)testDmaWithASO {
    [self testFile:@"DmaWithASO.txt"];
}

- (void)testEarlyNmiOnDma {
    [self testFile:@"EarlyNmiOnDma.txt"];   // TODO: ajouté don't check pour db pendant les phi1 des push stack du nmi car le db devrait etre ea ???
}

- (void)testLateNmiOnDma {
    [self testFile:@"LateNmiOnDma.txt"];    // TODO: ajouté don't check pour db pendant les phi1 des push stack du nmi car le db devrait etre ea ???
}

- (void)testNmiInDma {
    [self testFile:@"NmiInDma.txt"];    // TODO: ajouté don't check pour db pendant les phi1 des push stack du nmi car le db devrait etre ea ???
}

- (void)testResetInDma {
    [self testFile:@"ResetInDma.txt"];    // TODO: ajouté don't check pour db pendant les phi1 des push stack du nmi car le db devrait etre ea ???
}

- (void)testDmaOddCycle {
    [self testFile:@"DmaOddCycle.txt"];
}

- (void)testDmaEvenCycle {
    [self testFile:@"DmaEvenCycle.txt"];
}

- (void)testDmcDmaOddCycle {
    [self testFile:@"DmcDmaOddCycle.txt"];
}

- (void)testDmcDmaEvenCycle {
    [self testFile:@"DmcDmaEvenCycle.txt"]; // TODO: ne passe pas car le meme que DMA ASO (execution de l'instruction secondaire au decodeopcode)
}

- (void)testDmcDmaOnWriteOddCycle {
    [self testFile:@"DmcDmaOnWriteOddCycle.txt"];
}

- (void)testDmcDmaOnWriteEvenCycle {
    [self testFile:@"DmcDmaOnWriteEvenCycle.txt"];
}

- (void)testDmcDmaOnDoubleWriteOddCycle {
    [self testFile:@"DmcDmaOnDoubleWriteOddCycle.txt"]; // TODO: sur le phi1 apres le DMC dma, le bus a deja la valeur a écrire, désactivé le check du db pour le moment a ce cycle
}

- (void)testDmcDmaOnDoubleWriteEvenCycle {
    [self testFile:@"DmcDmaOnDoubleWriteEvenCycle.txt"];     // TODO: sur le phi1 apres le DMC dma, le bus a deja la valeur a écrire, désactivé le check du db pour le moment a ce cycle
}

- (void)testBlarggSyncDmc {
    [self testFile:@"BlarggSyncDmc.txt"];
}

@end
