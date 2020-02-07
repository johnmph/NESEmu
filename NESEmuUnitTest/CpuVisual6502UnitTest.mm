//
//  CpuVisual6502UnitTest.mm
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
#include "Cpu.hpp"
#include "Cpu6502FullAccess.hpp"
#include "Visual6502.hpp"


@interface CpuVisual6502UnitTest : XCTestCase

@end

@implementation CpuVisual6502UnitTest

namespace {
    
    struct Bus {
        uint8_t read(uint16_t address) {
            //std::cout << std::hex << "Read 0x" << static_cast<int>(memory[address]) << " at 0x" << address << "\n";
            return memory[address];
        }
        
        void write(uint16_t address, uint8_t data) {
            //std::cout << std::hex << "Write 0x" << static_cast<int>(data) << " at 0x" << address << "\n";
            memory[address] = data;
        }
        
        std::array<uint8_t, 1024 * 64> memory;
    };
    
    
    Bus bus;
    NESEmu::Cpu::Chip<NESEmu::Cpu::Model::Ricoh2A03, Bus> cpu2A03(bus);
    Cpu6502FullAccess<Bus> cpu6502(bus);
    
}

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
    
    // Initialize with BRK
    bus.memory.fill(0x00);
    
    // Power up cpu
    cpu6502.powerUp();
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
    Visual6502::Analyzer<Cpu6502FullAccess<Bus>> visual6502Analyzer(ifsLog, [](uint16_t address, uint8_t data) { bus.write(address, data); });
    
    // Release reset to start cpu
    cpu2A03.reset(true);
    
    // We need to sync with Visual6502 by adding nine first clocks to exit the reset state
    for (int i = 0; i < 9; ++i) {
        cpu2A03.clock();
    }
    
    // Analyze
    visual6502Analyzer.analyze(cpu2A03._cpu, [self](bool result) { XCTAssertTrue(result); });
    
    // Close file
    ifsLog.close();
}

- (void)testDma {
    [self testFile:@"Dma.txt"];
}

@end
