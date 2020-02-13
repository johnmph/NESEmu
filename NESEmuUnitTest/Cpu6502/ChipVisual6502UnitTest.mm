//
//  ChipVisual6502UnitTest.mm
//  NESEmuUnitTest
//
//  Created by Jonathan Baliko on 29/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#import <XCTest/XCTest.h>
#include <iostream>
#include <array>
#include <fstream>
#include <memory>
#include "Cpu6502FullAccess.hpp"
#include "Visual6502.hpp"


@interface ChipVisual6502UnitTest : XCTestCase

@end

@implementation ChipVisual6502UnitTest

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
    Cpu6502FullAccess<Cpu6502::ConfigurationAccurate<Bus>> cpu6502(bus);
    
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
    Visual6502::Analyzer<Cpu6502FullAccess<Cpu6502::ConfigurationAccurate<Bus>>> visual6502Analyzer(ifsLog, [](uint16_t address, uint8_t data) { bus.write(address, data); });
    
    // Release reset to start cpu
    cpu6502.reset(true);
    
    // We need to sync with Visual6502 by adding nine first clocks to exit the reset state
    for (int i = 0; i < 9; ++i) {
        cpu6502.clock();
    }
    
    // Analyze
    visual6502Analyzer.analyze(cpu6502, [self](bool result) { XCTAssertTrue(result); });
    
    // Close file
    ifsLog.close();
}

- (void)testProgramCounter {
    [self testFile:@"ProgramCounter.txt"];
}

- (void)testInterruptDisableFlag {
    [self testFile:@"InterruptDisableFlag.txt"];
}

- (void)testSetOverflow1 {
    [self testFile:@"SetOverflow1.txt"];
}

- (void)testSetOverflow2 {
    [self testFile:@"SetOverflow2.txt"];
}

- (void)testNmi1 {
    [self testFile:@"Nmi1.txt"];
}

- (void)testNmi2 {
    [self testFile:@"Nmi2.txt"];
}

- (void)testNmiInNmi1 {
    [self testFile:@"NmiInNmi1.txt"];
}

- (void)testNmiInNmi2 {
    [self testFile:@"NmiInNmi2.txt"];
}

- (void)testNmiInNmi3 {
    [self testFile:@"NmiInNmi3.txt"];
}

- (void)testNmiInNmi4 {
    [self testFile:@"NmiInNmi4.txt"];
}

- (void)testNmiInNmi5 {
    [self testFile:@"NmiInNmi5.txt"];
}

- (void)testNmiInNmi6 {
    [self testFile:@"NmiInNmi6.txt"];
}

- (void)testNmiInNmi7 {
    [self testFile:@"NmiInNmi7.txt"];
}

- (void)testNmiInBranchNotTaken1 {
    [self testFile:@"NmiInBranchNotTaken1.txt"];
}

- (void)testNmiInBranchNotTaken2 {
    [self testFile:@"NmiInBranchNotTaken2.txt"];
}

- (void)testNmiInBranchTakenWithNoPageCrossing1 {
    [self testFile:@"NmiInBranchTakenWithNoPageCrossing1.txt"];
}

- (void)testNmiInBranchTakenWithNoPageCrossing2 {
    [self testFile:@"NmiInBranchTakenWithNoPageCrossing2.txt"];
}

- (void)testNmiInBranchTakenWithNoPageCrossing3 {
    [self testFile:@"NmiInBranchTakenWithNoPageCrossing3.txt"];
}

- (void)testNmiInBranchTakenWithPageCrossing1 {
    [self testFile:@"NmiInBranchTakenWithPageCrossing1.txt"];
}

- (void)testNmiInBranchTakenWithPageCrossing2 {
    [self testFile:@"NmiInBranchTakenWithPageCrossing2.txt"];
}

- (void)testNmiInBranchTakenWithPageCrossing3 {
    [self testFile:@"NmiInBranchTakenWithPageCrossing3.txt"];
}

- (void)testNmiInBranchTakenWithPageCrossing4 {
    [self testFile:@"NmiInBranchTakenWithPageCrossing4.txt"];
}

- (void)testIrqInBranchNotTaken1 {
    [self testFile:@"IrqInBranchNotTaken1.txt"];
}

- (void)testIrqInBranchNotTaken2 {
    [self testFile:@"IrqInBranchNotTaken2.txt"];
}

- (void)testIrqInBranchTakenWithNoPageCrossing1 {
    [self testFile:@"IrqInBranchTakenWithNoPageCrossing1.txt"];
}

- (void)testIrqInBranchTakenWithNoPageCrossing2 {
    [self testFile:@"IrqInBranchTakenWithNoPageCrossing2.txt"];
}

- (void)testIrqInBranchTakenWithNoPageCrossing3 {
    [self testFile:@"IrqInBranchTakenWithNoPageCrossing3.txt"];
}

- (void)testIrqInBranchTakenWithPageCrossing1 {
    [self testFile:@"IrqInBranchTakenWithPageCrossing1.txt"];
}

- (void)testIrqInBranchTakenWithPageCrossing2 {
    [self testFile:@"IrqInBranchTakenWithPageCrossing2.txt"];
}

- (void)testIrqInBranchTakenWithPageCrossing3 {
    [self testFile:@"IrqInBranchTakenWithPageCrossing3.txt"];
}

- (void)testIrqInBranchTakenWithPageCrossing4 {
    [self testFile:@"IrqInBranchTakenWithPageCrossing4.txt"];
}

- (void)testIrqInInstrImplied1 {
    [self testFile:@"IrqInInstrImplied1.txt"];
}

- (void)testIrqInInstrImplied2 {
    [self testFile:@"IrqInInstrImplied2.txt"];
}

- (void)testIrqInInstrImmediate1 {
    [self testFile:@"IrqInInstrImmediate1.txt"];
}

- (void)testIrqInInstrImmediate2 {
    [self testFile:@"IrqInInstrImmediate2.txt"];
}

- (void)testIrqInInstrZeroPageRead1 {
    [self testFile:@"IrqInInstrZeroPageRead1.txt"];
}

- (void)testIrqInInstrZeroPageRead2 {
    [self testFile:@"IrqInInstrZeroPageRead2.txt"];
}

- (void)testIrqInInstrZeroPageRead3 {
    [self testFile:@"IrqInInstrZeroPageRead3.txt"];
}

- (void)testIrqInInstrZeroPageXRead1 {
    [self testFile:@"IrqInInstrZeroPageXRead1.txt"];
}

- (void)testIrqInInstrZeroPageXRead2 {
    [self testFile:@"IrqInInstrZeroPageXRead2.txt"];
}

- (void)testIrqInInstrZeroPageXRead3 {
    [self testFile:@"IrqInInstrZeroPageXRead3.txt"];
}

- (void)testIrqInInstrZeroPageXRead4 {
    [self testFile:@"IrqInInstrZeroPageXRead4.txt"];
}

- (void)testIrqInInstrAbsoluteRead1 {
    [self testFile:@"IrqInInstrAbsoluteRead1.txt"];
}

- (void)testIrqInInstrAbsoluteRead2 {
    [self testFile:@"IrqInInstrAbsoluteRead2.txt"];
}

- (void)testIrqInInstrAbsoluteRead3 {
    [self testFile:@"IrqInInstrAbsoluteRead3.txt"];
}

- (void)testIrqInInstrAbsoluteRead4 {
    [self testFile:@"IrqInInstrAbsoluteRead4.txt"];
}

- (void)testIrqInInstrAbsoluteXRead1 {
    [self testFile:@"IrqInInstrAbsoluteXRead1.txt"];
}

- (void)testIrqInInstrAbsoluteXRead2 {
    [self testFile:@"IrqInInstrAbsoluteXRead2.txt"];
}

- (void)testIrqInInstrAbsoluteXRead3 {
    [self testFile:@"IrqInInstrAbsoluteXRead3.txt"];
}

- (void)testIrqInInstrAbsoluteXRead4 {
    [self testFile:@"IrqInInstrAbsoluteXRead4.txt"];
}

- (void)testIrqInInstrAbsoluteXWithPageCrossingRead1 {
    [self testFile:@"IrqInInstrAbsoluteXWithPageCrossingRead1.txt"];
}

- (void)testIrqInInstrAbsoluteXWithPageCrossingRead2 {
    [self testFile:@"IrqInInstrAbsoluteXWithPageCrossingRead2.txt"];
}

- (void)testIrqInInstrAbsoluteXWithPageCrossingRead3 {
    [self testFile:@"IrqInInstrAbsoluteXWithPageCrossingRead3.txt"];
}

- (void)testIrqInInstrAbsoluteXWithPageCrossingRead4 {
    [self testFile:@"IrqInInstrAbsoluteXWithPageCrossingRead4.txt"];
}

- (void)testIrqInInstrAbsoluteXWithPageCrossingRead5 {
    [self testFile:@"IrqInInstrAbsoluteXWithPageCrossingRead5.txt"];
}

- (void)testIrqInInstrIndXRead1 {
    [self testFile:@"IrqInInstrIndXRead1.txt"];
}

- (void)testIrqInInstrIndXRead2 {
    [self testFile:@"IrqInInstrIndXRead2.txt"];
}

- (void)testIrqInInstrIndXRead3 {
    [self testFile:@"IrqInInstrIndXRead3.txt"];
}

- (void)testIrqInInstrIndXRead4 {
    [self testFile:@"IrqInInstrIndXRead4.txt"];
}

- (void)testIrqInInstrIndXRead5 {
    [self testFile:@"IrqInInstrIndXRead5.txt"];
}

- (void)testIrqInInstrIndXRead6 {
    [self testFile:@"IrqInInstrIndXRead6.txt"];
}

- (void)testIrqInInstrIndYRead1 {
    [self testFile:@"IrqInInstrIndYRead1.txt"];
}

- (void)testIrqInInstrIndYRead2 {
    [self testFile:@"IrqInInstrIndYRead2.txt"];
}

- (void)testIrqInInstrIndYRead3 {
    [self testFile:@"IrqInInstrIndYRead3.txt"];
}

- (void)testIrqInInstrIndYRead4 {
    [self testFile:@"IrqInInstrIndYRead4.txt"];
}

- (void)testIrqInInstrIndYRead5 {
    [self testFile:@"IrqInInstrIndYRead5.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingRead1 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingRead1.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingRead2 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingRead2.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingRead3 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingRead3.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingRead4 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingRead4.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingRead5 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingRead5.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingRead6 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingRead6.txt"];
}

- (void)testIrqInInstrZeroPageWrite1 {
    [self testFile:@"IrqInInstrZeroPageWrite1.txt"];
}

- (void)testIrqInInstrZeroPageWrite2 {
    [self testFile:@"IrqInInstrZeroPageWrite2.txt"];
}

- (void)testIrqInInstrZeroPageWrite3 {
    [self testFile:@"IrqInInstrZeroPageWrite3.txt"];
}

- (void)testIrqInInstrZeroPageXWrite1 {
    [self testFile:@"IrqInInstrZeroPageXWrite1.txt"];
}

- (void)testIrqInInstrZeroPageXWrite2 {
    [self testFile:@"IrqInInstrZeroPageXWrite2.txt"];
}

- (void)testIrqInInstrZeroPageXWrite3 {
    [self testFile:@"IrqInInstrZeroPageXWrite3.txt"];
}

- (void)testIrqInInstrZeroPageXWrite4 {
    [self testFile:@"IrqInInstrZeroPageXWrite4.txt"];
}

- (void)testIrqInInstrAbsoluteWrite1 {
    [self testFile:@"IrqInInstrAbsoluteWrite1.txt"];
}

- (void)testIrqInInstrAbsoluteWrite2 {
    [self testFile:@"IrqInInstrAbsoluteWrite2.txt"];
}

- (void)testIrqInInstrAbsoluteWrite3 {
    [self testFile:@"IrqInInstrAbsoluteWrite3.txt"];
}

- (void)testIrqInInstrAbsoluteWrite4 {
    [self testFile:@"IrqInInstrAbsoluteWrite4.txt"];
}

- (void)testIrqInInstrAbsoluteXWrite1 {
    [self testFile:@"IrqInInstrAbsoluteXWrite1.txt"];
}

- (void)testIrqInInstrAbsoluteXWrite2 {
    [self testFile:@"IrqInInstrAbsoluteXWrite2.txt"];
}

- (void)testIrqInInstrAbsoluteXWrite3 {
    [self testFile:@"IrqInInstrAbsoluteXWrite3.txt"];
}

- (void)testIrqInInstrAbsoluteXWrite4 {
    [self testFile:@"IrqInInstrAbsoluteXWrite4.txt"];
}

- (void)testIrqInInstrAbsoluteXWithPageCrossingWrite1 {
    [self testFile:@"IrqInInstrAbsoluteXWithPageCrossingWrite1.txt"];
}

- (void)testIrqInInstrAbsoluteXWithPageCrossingWrite2 {
    [self testFile:@"IrqInInstrAbsoluteXWithPageCrossingWrite2.txt"];
}

- (void)testIrqInInstrAbsoluteXWithPageCrossingWrite3 {
    [self testFile:@"IrqInInstrAbsoluteXWithPageCrossingWrite3.txt"];
}

- (void)testIrqInInstrAbsoluteXWithPageCrossingWrite4 {
    [self testFile:@"IrqInInstrAbsoluteXWithPageCrossingWrite4.txt"];
}

- (void)testIrqInInstrAbsoluteXWithPageCrossingWrite5 {
    [self testFile:@"IrqInInstrAbsoluteXWithPageCrossingWrite5.txt"];
}

- (void)testIrqInInstrIndXWrite1 {
    [self testFile:@"IrqInInstrIndXWrite1.txt"];
}

- (void)testIrqInInstrIndXWrite2 {
    [self testFile:@"IrqInInstrIndXWrite2.txt"];
}

- (void)testIrqInInstrIndXWrite3 {
    [self testFile:@"IrqInInstrIndXWrite3.txt"];
}

- (void)testIrqInInstrIndXWrite4 {
    [self testFile:@"IrqInInstrIndXWrite4.txt"];
}

- (void)testIrqInInstrIndXWrite5 {
    [self testFile:@"IrqInInstrIndXWrite5.txt"];
}

- (void)testIrqInInstrIndXWrite6 {
    [self testFile:@"IrqInInstrIndXWrite6.txt"];
}

- (void)testIrqInInstrIndYWrite1 {
    [self testFile:@"IrqInInstrIndYWrite1.txt"];
}

- (void)testIrqInInstrIndYWrite2 {
    [self testFile:@"IrqInInstrIndYWrite2.txt"];
}

- (void)testIrqInInstrIndYWrite3 {
    [self testFile:@"IrqInInstrIndYWrite3.txt"];
}

- (void)testIrqInInstrIndYWrite4 {
    [self testFile:@"IrqInInstrIndYWrite4.txt"];
}

- (void)testIrqInInstrIndYWrite5 {
    [self testFile:@"IrqInInstrIndYWrite5.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingWrite1 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingWrite1.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingWrite2 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingWrite2.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingWrite3 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingWrite3.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingWrite4 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingWrite4.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingWrite5 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingWrite5.txt"];
}

- (void)testIrqInInstrIndYWithPageCrossingWrite6 {
    [self testFile:@"IrqInInstrIndYWithPageCrossingWrite6.txt"];
}

- (void)testIrqInInstrZeroPageRMW1 {
    [self testFile:@"IrqInInstrZeroPageRMW1.txt"];
}

- (void)testIrqInInstrZeroPageRMW2 {
    [self testFile:@"IrqInInstrZeroPageRMW2.txt"];
}

- (void)testIrqInInstrZeroPageRMW3 {
    [self testFile:@"IrqInInstrZeroPageRMW3.txt"];
}

- (void)testIrqInInstrZeroPageRMW4 {
    [self testFile:@"IrqInInstrZeroPageRMW4.txt"];
}

- (void)testIrqInInstrZeroPageRMW5 {
    [self testFile:@"IrqInInstrZeroPageRMW5.txt"];
}

- (void)testIrqInInstrZeroPageXRMW1 {
    [self testFile:@"IrqInInstrZeroPageXRMW1.txt"];
}

- (void)testIrqInInstrZeroPageXRMW2 {
    [self testFile:@"IrqInInstrZeroPageXRMW2.txt"];
}

- (void)testIrqInInstrZeroPageXRMW3 {
    [self testFile:@"IrqInInstrZeroPageRMW3.txt"];
}

- (void)testIrqInInstrZeroPageXRMW4 {
    [self testFile:@"IrqInInstrZeroPageXRMW4.txt"];
}

- (void)testIrqInInstrZeroPageXRMW5 {
    [self testFile:@"IrqInInstrZeroPageXRMW5.txt"];
}

- (void)testIrqInInstrZeroPageXRMW6 {
    [self testFile:@"IrqInInstrZeroPageXRMW6.txt"];
}

- (void)testIrqInInstrAbsoluteRMW1 {
    [self testFile:@"IrqInInstrAbsoluteRMW1.txt"];
}

- (void)testIrqInInstrAbsoluteRMW2 {
    [self testFile:@"IrqInInstrAbsoluteRMW2.txt"];
}

- (void)testIrqInInstrAbsoluteRMW3 {
    [self testFile:@"IrqInInstrAbsoluteRMW3.txt"];
}

- (void)testIrqInInstrAbsoluteRMW4 {
    [self testFile:@"IrqInInstrAbsoluteRMW4.txt"];
}

- (void)testIrqInInstrAbsoluteRMW5 {
    [self testFile:@"IrqInInstrAbsoluteRMW5.txt"];
}

- (void)testIrqInInstrAbsoluteRMW6 {
    [self testFile:@"IrqInInstrAbsoluteRMW6.txt"];
}

- (void)testIrqInInstrAbsoluteXRMW1 {
    [self testFile:@"IrqInInstrAbsoluteXRMW1.txt"];
}

- (void)testIrqInInstrAbsoluteXRMW2 {
    [self testFile:@"IrqInInstrAbsoluteXRMW2.txt"];
}

- (void)testIrqInInstrAbsoluteXRMW3 {
    [self testFile:@"IrqInInstrAbsoluteXRMW3.txt"];
}

- (void)testIrqInInstrAbsoluteXRMW4 {
    [self testFile:@"IrqInInstrAbsoluteXRMW4.txt"];
}

- (void)testIrqInInstrAbsoluteXRMW5 {
    [self testFile:@"IrqInInstrAbsoluteXRMW5.txt"];
}

- (void)testIrqInInstrAbsoluteXRMW6 {
    [self testFile:@"IrqInInstrAbsoluteXRMW6.txt"];
}

- (void)testIrqInInstrAbsoluteXRMW7 {
    [self testFile:@"IrqInInstrAbsoluteXRMW7.txt"];
}

- (void)testIrqInInstrDelayedImmediate1 {
    [self testFile:@"IrqInInstrDelayedImmediate1.txt"];
}

- (void)testIrqInInstrDelayedImmediate2 {
    [self testFile:@"IrqInInstrDelayedImmediate2.txt"];
}

- (void)testIrqInInstrDelayedZeroPage1 {
    [self testFile:@"IrqInInstrDelayedZeroPage1.txt"];
}

- (void)testIrqInInstrDelayedZeroPage2 {
    [self testFile:@"IrqInInstrDelayedZeroPage2.txt"];
}

- (void)testIrqInInstrDelayedZeroPage3 {
    [self testFile:@"IrqInInstrDelayedZeroPage3.txt"];
}

- (void)testIrqInInstrDelayedZeroPageX1 {
    [self testFile:@"IrqInInstrDelayedZeroPageX1.txt"];
}

- (void)testIrqInInstrDelayedZeroPageX2 {
    [self testFile:@"IrqInInstrDelayedZeroPageX2.txt"];
}

- (void)testIrqInInstrDelayedZeroPageX3 {
    [self testFile:@"IrqInInstrDelayedZeroPageX3.txt"];
}

- (void)testIrqInInstrDelayedZeroPageX4 {
    [self testFile:@"IrqInInstrDelayedZeroPageX4.txt"];
}

- (void)testIrqInInstrDelayedAbsolute1 {
    [self testFile:@"IrqInInstrDelayedAbsolute1.txt"];
}

- (void)testIrqInInstrDelayedAbsolute2 {
    [self testFile:@"IrqInInstrDelayedAbsolute2.txt"];
}

- (void)testIrqInInstrDelayedAbsolute3 {
    [self testFile:@"IrqInInstrDelayedAbsolute3.txt"];
}

- (void)testIrqInInstrDelayedAbsolute4 {
    [self testFile:@"IrqInInstrDelayedAbsolute4.txt"];
}

- (void)testIrqInInstrDelayedAbsoluteX1 {
    [self testFile:@"IrqInInstrDelayedAbsoluteX1.txt"];
}

- (void)testIrqInInstrDelayedAbsoluteX2 {
    [self testFile:@"IrqInInstrDelayedAbsoluteX2.txt"];
}

- (void)testIrqInInstrDelayedAbsoluteX3 {
    [self testFile:@"IrqInInstrDelayedAbsoluteX3.txt"];
}

- (void)testIrqInInstrDelayedAbsoluteX4 {
    [self testFile:@"IrqInInstrDelayedAbsoluteX4.txt"];
}

- (void)testIrqInInstrDelayedIndX1 {
    [self testFile:@"IrqInInstrDelayedIndX1.txt"];
}

- (void)testIrqInInstrDelayedIndX2 {
    [self testFile:@"IrqInInstrDelayedIndX2.txt"];
}

- (void)testIrqInInstrDelayedIndX3 {
    [self testFile:@"IrqInInstrDelayedIndX3.txt"];
}

- (void)testIrqInInstrDelayedIndX4 {
    [self testFile:@"IrqInInstrDelayedIndX4.txt"];
}

- (void)testIrqInInstrDelayedIndX5 {
    [self testFile:@"IrqInInstrDelayedIndX5.txt"];
}

- (void)testIrqInInstrDelayedIndY1 {
    [self testFile:@"IrqInInstrDelayedIndY1.txt"];
}

- (void)testIrqInInstrDelayedIndY2 {
    [self testFile:@"IrqInInstrDelayedIndY2.txt"];
}

- (void)testIrqInInstrDelayedIndY3 {
    [self testFile:@"IrqInInstrDelayedIndY3.txt"];
}

- (void)testIrqInInstrDelayedIndY4 {
    [self testFile:@"IrqInInstrDelayedIndY4.txt"];
}

- (void)testIrqInInstrDelayedIndY5 {
    [self testFile:@"IrqInInstrDelayedIndY5.txt"];
}

- (void)testIrqInInstrDelayedIndYWithPageCrossing1 {
    [self testFile:@"IrqInInstrDelayedIndYWithPageCrossing1.txt"];
}

- (void)testIrqInInstrDelayedIndYWithPageCrossing2 {
    [self testFile:@"IrqInInstrDelayedIndYWithPageCrossing2.txt"];
}

- (void)testIrqInInstrDelayedIndYWithPageCrossing3 {
    [self testFile:@"IrqInInstrDelayedIndYWithPageCrossing3.txt"];
}

- (void)testIrqInInstrDelayedIndYWithPageCrossing4 {
    [self testFile:@"IrqInInstrDelayedIndYWithPageCrossing4.txt"];
}

- (void)testIrqInInstrDelayedIndYWithPageCrossing5 {
    [self testFile:@"IrqInInstrDelayedIndYWithPageCrossing5.txt"];
}

- (void)testIrqInInstrDelayedIndYWithPageCrossing6 {
    [self testFile:@"IrqInInstrDelayedIndYWithPageCrossing6.txt"];
}

- (void)testIrqInJmp1 {
    [self testFile:@"IrqInJmp1.txt"];
}

- (void)testIrqInJmp2 {
    [self testFile:@"IrqInJmp2.txt"];
}

- (void)testIrqInJmp3 {
    [self testFile:@"IrqInJmp3.txt"];
}

- (void)testIrqInJmpInd1 {
    [self testFile:@"IrqInJmpInd1.txt"];
}

- (void)testIrqInJmpInd2 {
    [self testFile:@"IrqInJmpInd2.txt"];
}

- (void)testIrqInJmpInd3 {
    [self testFile:@"IrqInJmpInd3.txt"];
}

- (void)testIrqInJmpInd4 {
    [self testFile:@"IrqInJmpInd4.txt"];
}

- (void)testIrqInJmpInd5 {
    [self testFile:@"IrqInJmpInd5.txt"];
}

- (void)testIrqInJsr1 {
    [self testFile:@"IrqInJsr1.txt"];
}

- (void)testIrqInJsr2 {
    [self testFile:@"IrqInJsr2.txt"];
}

- (void)testIrqInJsr3 {
    [self testFile:@"IrqInJsr3.txt"];
}

- (void)testIrqInJsr4 {
    [self testFile:@"IrqInJsr4.txt"];
}

- (void)testIrqInJsr5 {
    [self testFile:@"IrqInJsr5.txt"];
}

- (void)testIrqInJsr6 {
    [self testFile:@"IrqInJsr6.txt"];
}

- (void)testIrqInRts1 {
    [self testFile:@"IrqInRts1.txt"];
}

- (void)testIrqInRts2 {
    [self testFile:@"IrqInRts2.txt"];
}

- (void)testIrqInRts3 {
    [self testFile:@"IrqInRts3.txt"];
}

- (void)testIrqInRts4 {
    [self testFile:@"IrqInRts4.txt"];
}

- (void)testIrqInRts5 {
    [self testFile:@"IrqInRts5.txt"];
}

- (void)testIrqInRts6 {
    [self testFile:@"IrqInRts6.txt"];
}

- (void)testIrqInRti1 {
    [self testFile:@"IrqInRti1.txt"];
}

- (void)testIrqInRti2 {
    [self testFile:@"IrqInRti2.txt"];
}

- (void)testIrqInRti3 {
    [self testFile:@"IrqInRti3.txt"];
}

- (void)testIrqInRti4 {
    [self testFile:@"IrqInRti4.txt"];
}

- (void)testIrqInRti5 {
    [self testFile:@"IrqInRti5.txt"];
}

- (void)testIrqInRti6 {
    [self testFile:@"IrqInRti6.txt"];
}

- (void)testIrqInCli1 {
    [self testFile:@"IrqInCli1.txt"];
}

- (void)testIrqInCli2 {
    [self testFile:@"IrqInCli2.txt"];
}

- (void)testIrqInCliCli1 {
    [self testFile:@"IrqInCliCli1.txt"];
}

- (void)testIrqInCliCli2 {
    [self testFile:@"IrqInCliCli2.txt"];
}

- (void)testIrqInSei1 {
    [self testFile:@"IrqInSei1.txt"];
}

- (void)testIrqInSei2 {
    [self testFile:@"IrqInSei2.txt"];
}

- (void)testIrqInPha1 {
    [self testFile:@"IrqInPha1.txt"];
}

- (void)testIrqInPha2 {
    [self testFile:@"IrqInPha2.txt"];
}

- (void)testIrqInPha3 {
    [self testFile:@"IrqInPha3.txt"];
}

- (void)testIrqInPhp1 {
    [self testFile:@"IrqInPhp1.txt"];
}

- (void)testIrqInPhp2 {
    [self testFile:@"IrqInPhp2.txt"];
}

- (void)testIrqInPhp3 {
    [self testFile:@"IrqInPhp3.txt"];
}

- (void)testIrqInPla1 {
    [self testFile:@"IrqInPla1.txt"];
}

- (void)testIrqInPla2 {
    [self testFile:@"IrqInPla2.txt"];
}

- (void)testIrqInPla3 {
    [self testFile:@"IrqInPla3.txt"];
}

- (void)testIrqInPla4 {
    [self testFile:@"IrqInPla4.txt"];
}

- (void)testIrqInPlp1 {
    [self testFile:@"IrqInPlp1.txt"];
}

- (void)testIrqInPlp2 {
    [self testFile:@"IrqInPla2.txt"];
}

- (void)testIrqInPlp3 {
    [self testFile:@"IrqInPlp3.txt"];
}

- (void)testIrqInPlp4 {
    [self testFile:@"IrqInPlp4.txt"];
}

- (void)testIrqInNmi1 {
    [self testFile:@"IrqInNmi1.txt"];
}

- (void)testIrqInNmi2 {
    [self testFile:@"IrqInNmi2.txt"];
}

- (void)testIrqInNmi3 {
    [self testFile:@"IrqInNmi3.txt"];
}

- (void)testIrqInNmi4 {
    [self testFile:@"IrqInNmi4.txt"];
}

- (void)testIrqInNmi5 {
    [self testFile:@"IrqInNmi5.txt"];
}

- (void)testIrqInNmi6 {
    [self testFile:@"IrqInNmi6.txt"];
}

- (void)testIrqInNmi7 {
    [self testFile:@"IrqInNmi7.txt"];
}

- (void)testIrqInNmi8 {
    [self testFile:@"IrqInNmi8.txt"];
}

- (void)testIrqInNmi9 {
    [self testFile:@"IrqInNmi9.txt"];
}

- (void)testIrqInNmi10 {
    [self testFile:@"IrqInNmi10.txt"];
}

- (void)testIrqInInstrUndocumentedImmediateRead1 {
    [self testFile:@"IrqInInstrUndocumentedImmediateRead1.txt"];
}

- (void)testIrqInInstrUndocumentedImmediateRead2 {
    [self testFile:@"IrqInInstrUndocumentedImmediateRead2.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageRead1 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageRead1.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageRead2 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageRead2.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageRead3 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageRead3.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageRead4 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageRead4.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageRead5 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageRead5.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageXRead1 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageXRead1.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageXRead2 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageXRead2.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageXRead3 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageXRead3.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageXRead4 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageXRead4.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageXRead5 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageXRead5.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageXRead6 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageXRead6.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteRead1 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteRead1.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteRead2 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteRead2.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteRead3 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteRead3.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteRead4 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteRead4.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteRead5 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteRead5.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteRead6 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteRead6.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRead1 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRead1.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRead2 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRead2.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRead3 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRead3.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRead4 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRead4.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRead5 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRead5.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRead6 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRead6.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRead7 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRead7.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRead1 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRead1.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRead2 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRead2.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRead3 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRead3.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRead4 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRead4.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRead5 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRead5.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRead6 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRead6.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRead7 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRead7.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRead1 {
    [self testFile:@"IrqInInstrUndocumentedIndXRead1.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRead2 {
    [self testFile:@"IrqInInstrUndocumentedIndXRead2.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRead3 {
    [self testFile:@"IrqInInstrUndocumentedIndXRead3.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRead4 {
    [self testFile:@"IrqInInstrUndocumentedIndXRead4.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRead5 {
    [self testFile:@"IrqInInstrUndocumentedIndXRead5.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRead6 {
    [self testFile:@"IrqInInstrUndocumentedIndXRead6.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRead7 {
    [self testFile:@"IrqInInstrUndocumentedIndXRead7.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRead8 {
    [self testFile:@"IrqInInstrUndocumentedIndXRead8.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRead1 {
    [self testFile:@"IrqInInstrUndocumentedIndYRead1.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRead2 {
    [self testFile:@"IrqInInstrUndocumentedIndYRead2.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRead3 {
    [self testFile:@"IrqInInstrUndocumentedIndYRead3.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRead4 {
    [self testFile:@"IrqInInstrUndocumentedIndYRead4.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRead5 {
    [self testFile:@"IrqInInstrUndocumentedIndYRead5.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRead6 {
    [self testFile:@"IrqInInstrUndocumentedIndYRead6.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRead7 {
    [self testFile:@"IrqInInstrUndocumentedIndYRead7.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRead8 {
    [self testFile:@"IrqInInstrUndocumentedIndYRead8.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRead1 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRead1.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRead2 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRead2.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRead3 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRead3.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRead4 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRead4.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRead5 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRead5.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRead6 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRead6.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRead7 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRead7.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRead8 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRead8.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageRMW1 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageRMW1.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageRMW2 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageRMW2.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageRMW3 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageRMW3.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageRMW4 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageRMW4.txt"];
}

- (void)testIrqInInstrUndocumentedZeroPageRMW5 {
    [self testFile:@"IrqInInstrUndocumentedZeroPageRMW5.txt"];
}

- (void)testIrqInUndocumentedZeroPageXRMW1 {
    [self testFile:@"IrqInUndocumentedZeroPageXRMW1.txt"];
}

- (void)testIrqInUndocumentedZeroPageXRMW2 {
    [self testFile:@"IrqInUndocumentedZeroPageXRMW2.txt"];
}

- (void)testIrqInUndocumentedZeroPageXRMW3 {
    [self testFile:@"IrqInUndocumentedZeroPageXRMW3.txt"];
}

- (void)testIrqInUndocumentedZeroPageXRMW4 {
    [self testFile:@"IrqInUndocumentedZeroPageXRMW4.txt"];
}

- (void)testIrqInUndocumentedZeroPageXRMW5 {
    [self testFile:@"IrqInUndocumentedZeroPageXRMW5.txt"];
}

- (void)testIrqInUndocumentedZeroPageXRMW6 {
    [self testFile:@"IrqInUndocumentedZeroPageXRMW6.txt"];
}

- (void)testIrqInUndocumentedAbsoluteRMW1 {
    [self testFile:@"IrqInUndocumentedAbsoluteRMW1.txt"];
}

- (void)testIrqInUndocumentedAbsoluteRMW2 {
    [self testFile:@"IrqInUndocumentedAbsoluteRMW2.txt"];
}

- (void)testIrqInUndocumentedAbsoluteRMW3 {
    [self testFile:@"IrqInUndocumentedAbsoluteRMW3.txt"];
}

- (void)testIrqInUndocumentedAbsoluteRMW4 {
    [self testFile:@"IrqInUndocumentedAbsoluteRMW4.txt"];
}

- (void)testIrqInUndocumentedAbsoluteRMW5 {
    [self testFile:@"IrqInUndocumentedAbsoluteRMW5.txt"];
}

- (void)testIrqInUndocumentedAbsoluteRMW6 {
    [self testFile:@"IrqInUndocumentedAbsoluteRMW6.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRMW1 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRMW1.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRMW2 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRMW2.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRMW3 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRMW3.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRMW4 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRMW4.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRMW5 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRMW5.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRMW6 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRMW6.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXRMW7 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXRMW7.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW1 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW1.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW2 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW2.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW3 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW3.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW4 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW4.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW5 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW5.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW6 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW6.txt"];
}

- (void)testIrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW7 {
    [self testFile:@"IrqInInstrUndocumentedAbsoluteXWithPageCrossingRMW7.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRMW1 {
    [self testFile:@"IrqInInstrUndocumentedIndXRMW1.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRMW2 {
    [self testFile:@"IrqInInstrUndocumentedIndXRMW2.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRMW3 {
    [self testFile:@"IrqInInstrUndocumentedIndXRMW3.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRMW4 {
    [self testFile:@"IrqInInstrUndocumentedIndXRMW4.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRMW5 {
    [self testFile:@"IrqInInstrUndocumentedIndXRMW5.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRMW6 {
    [self testFile:@"IrqInInstrUndocumentedIndXRMW6.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRMW7 {
    [self testFile:@"IrqInInstrUndocumentedIndXRMW7.txt"];
}

- (void)testIrqInInstrUndocumentedIndXRMW8 {
    [self testFile:@"IrqInInstrUndocumentedIndXRMW8.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRMW1 {
    [self testFile:@"IrqInInstrUndocumentedIndYRMW1.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRMW2 {
    [self testFile:@"IrqInInstrUndocumentedIndYRMW2.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRMW3 {
    [self testFile:@"IrqInInstrUndocumentedIndYRMW3.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRMW4 {
    [self testFile:@"IrqInInstrUndocumentedIndYRMW4.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRMW5 {
    [self testFile:@"IrqInInstrUndocumentedIndYRMW5.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRMW6 {
    [self testFile:@"IrqInInstrUndocumentedIndYRMW6.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRMW7 {
    [self testFile:@"IrqInInstrUndocumentedIndYRMW7.txt"];
}

- (void)testIrqInInstrUndocumentedIndYRMW8 {
    [self testFile:@"IrqInInstrUndocumentedIndYRMW8.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRMW1 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRMW1.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRMW2 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRMW2.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRMW3 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRMW3.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRMW4 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRMW4.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRMW5 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRMW5.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRMW6 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRMW6.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRMW7 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRMW7.txt"];
}

- (void)testIrqInInstrUndocumentedIndYWithPageCrossingRMW8 {
    [self testFile:@"IrqInInstrUndocumentedIndYWithPageCrossingRMW8.txt"];
}

- (void)testInstrALURegister {
    [self testFile:@"InstrALURegister.txt"];
}

- (void)testInstrALUMemory {
    [self testFile:@"InstrALUMemory.txt"];
}

- (void)testInstrUndocumentedALR {
    [self testFile:@"InstrUndocumentedALR.txt"];
}

- (void)testInstrUndocumentedASO {
    [self testFile:@"InstrUndocumentedASO.txt"];
}

- (void)testInstrUndocumentedAXS {
    [self testFile:@"InstrUndocumentedAXS.txt"];
}

- (void)testInstrUndocumentedDCM {
    [self testFile:@"InstrUndocumentedDCM.txt"];
}

- (void)testInstrUndocumentedINS {
    [self testFile:@"InstrUndocumentedINS.txt"];
}

- (void)testInstrUndocumentedLAX {
    [self testFile:@"InstrUndocumentedLAX.txt"];
}

- (void)testInstrUndocumentedLSE {
    [self testFile:@"InstrUndocumentedLSE.txt"];
}

- (void)testInstrUndocumentedRLA {
    [self testFile:@"InstrUndocumentedRLA.txt"];
}

- (void)testInstrUndocumentedRRA {
    [self testFile:@"InstrUndocumentedRRA.txt"];
}

/*
 
 Reset conclusions :
 
 - Il n'est détecté qu'au phi2
 - On dirait qu'une fois détecté, il laisse un cycle complet de l'instruction en cours puis le cycle suivant de l'instruction en cours mais avec le flag interrupt mis (pour etre exact apres le low detecté, il laisse un cycle complet sans le flag puis il met le flag, meme si la ligne reste low) (donc des read a la place des writes, des vecteurs d'interruptions de reset a la place des vraies interruptions) et si le cycle suivant de l'instruction en cours est le fetchOpcode alors il fait un cycle vide (il fait un readdatabus), puis le fetchOpcode (sans incrémentation du PC et avec forcage du BRK comme une interruption normale), suivi du BRK0, ...
 - S'il est détecté juste au brk4 alors il n'est pas executé (surement car dans ce step, les flags d'interruptions sont remis a 0)
 
 */

- (void)testResetInHalfClock1 {
    [self testFile:@"ResetInHalfClock1.txt"];
}

- (void)testResetInHalfClock2 {
    [self testFile:@"ResetInHalfClock2.txt"];
}

- (void)testResetForManyHalfClock1 {
    [self testFile:@"ResetForManyHalfClock1.txt"];
}

- (void)testResetForManyHalfClock2 {
    [self testFile:@"ResetForManyHalfClock2.txt"];
}

- (void)testResetForManyHalfClock3 {
    [self testFile:@"ResetForManyHalfClock3.txt"];
}

- (void)testResetForManyHalfClock4 {
    [self testFile:@"ResetForManyHalfClock4.txt"];
}

- (void)testResetInInstrSteps1 {
    //[self testFile:@"ResetInInstrSteps1.txt"];  // TODO: il y a 2 extra steps avant le debut du reset, voir pourquoi
}

- (void)testResetInInstrSteps2 {
    [self testFile:@"ResetInInstrSteps2.txt"];
}

- (void)testResetInInstrSteps3 {
    [self testFile:@"ResetInInstrSteps3.txt"];
}

- (void)testResetInInstrSteps4 {
    [self testFile:@"ResetInInstrSteps4.txt"];
}

- (void)testResetInInstrSteps5 {
    [self testFile:@"ResetInInstrSteps5.txt"];
}

- (void)testResetInInterrupt1 {
    [self testFile:@"ResetInInterrupt1.txt"];
}

- (void)testResetInInterrupt2 {
    [self testFile:@"ResetInInterrupt2.txt"];
}

- (void)testResetInInterrupt3 {
    [self testFile:@"ResetInInterrupt3.txt"];
}

- (void)testResetInInterrupt4 {
    //[self testFile:@"ResetInInterrupt4.txt"]; // TODO: il y a le rw qui est a 0 pendant le reset, voir pourquoi
}

- (void)testResetInInterrupt5 {
    [self testFile:@"ResetInInterrupt5.txt"];
}

- (void)testResetInInterrupt6 {
    [self testFile:@"ResetInInterrupt6.txt"];
}

- (void)testResetInInterrupt7 {
    [self testFile:@"ResetInInterrupt7.txt"];
}

- (void)testCheckRegistersValueWhenReset {
    [self testFile:@"CheckRegistersValueWhenReset.txt"];
}

- (void)testCheckBreakFlagInBrk {
    [self testFile:@"CheckBreakFlagInBrk.txt"];
}

- (void)testCheckBreakFlagInNmi {
    [self testFile:@"CheckBreakFlagInNmi.txt"];
}

- (void)testNmiInBrk1 {
    [self testFile:@"NmiInBrk1.txt"];
}

- (void)testNmiInBrk2 {
    [self testFile:@"NmiInBrk2.txt"];
}

- (void)testNmiInBrk3 {
    [self testFile:@"NmiInBrk3.txt"];
}

- (void)testNmiInBrk4 {
    [self testFile:@"NmiInBrk4.txt"];
}

- (void)testNmiInBrk5 {
    [self testFile:@"NmiInBrk5.txt"];
}

- (void)testNmiInBrk6 {
    [self testFile:@"NmiInBrk6.txt"];
}

- (void)testNmiInBrk7 {
    [self testFile:@"NmiInBrk7.txt"];
}

- (void)testNmiInIrq1 {
    [self testFile:@"NmiInIrq1.txt"];
}

- (void)testNmiInIrq2 {
    [self testFile:@"NmiInIrq2.txt"];
}

- (void)testNmiInIrq3 {
    [self testFile:@"NmiInIrq3.txt"];
}

- (void)testNmiInIrq4 {
    [self testFile:@"NmiInIrq4.txt"];
}

- (void)testNmiInIrq5 {
    [self testFile:@"NmiInIrq5.txt"];
}

- (void)testNmiInIrq6 {
    [self testFile:@"NmiInIrq6.txt"];
}

- (void)testNmiInIrq7 {
    [self testFile:@"NmiInIrq7.txt"];
}

- (void)testNmiInIrq8 {
    [self testFile:@"NmiInIrq8.txt"];
}

- (void)testIrqInBrk1 {
    [self testFile:@"IrqInBrk1.txt"];
}

- (void)testIrqInBrk2 {
    [self testFile:@"IrqInBrk2.txt"];
}

- (void)testIrqInBrk3 {
    [self testFile:@"IrqInBrk3.txt"];
}

- (void)testIrqInBrk4 {
    [self testFile:@"IrqInBrk4.txt"];
}

- (void)testIrqInBrk5 {
    [self testFile:@"IrqInBrk5.txt"];
}

- (void)testIrqInBrk6 {
    [self testFile:@"IrqInBrk6.txt"];
}

- (void)testIrqInBrk7 {
    [self testFile:@"IrqInBrk7.txt"];
}
/*
- (void)testRdyLowManyCycleInstrALUAND1 {
 [self testFile:@"RdyLowManyCycleInstrALUAND1.txt"];    // TODO : jusqu'a 8 mais probleme avec AND car il se comporte differement, voir : https://retrocomputing.stackexchange.com/questions/13672/6502-and-instruction-updates-flags-differently-than-other-logic-operations
}*/

- (void)testRdyLowManyCycleInstrALU1 {
    [self testFile:@"RdyLowManyCycleInstrALU1.txt"];
}

- (void)testRdyLowManyCycleInstrALU2 {
    [self testFile:@"RdyLowManyCycleInstrALU2.txt"];
}

- (void)testRdyLowManyCycleInstrALU3 {
    [self testFile:@"RdyLowManyCycleInstrALU3.txt"];
}

- (void)testRdyLowManyCycleInstrALU4 {
    [self testFile:@"RdyLowManyCycleInstrALU4.txt"];
}

- (void)testRdyLowManyCycleInstrALU5 {
    [self testFile:@"RdyLowManyCycleInstrALU5.txt"];
}

- (void)testRdyLowManyCycleInstrALU6 {
    [self testFile:@"RdyLowManyCycleInstrALU6.txt"];
}

- (void)testRdyLowManyCycleInstrALU7 {
    [self testFile:@"RdyLowManyCycleInstrALU7.txt"];
}

- (void)testRdyLowManyCycleInstrALU8 {
    [self testFile:@"RdyLowManyCycleInstrALU8.txt"];
}

- (void)testRdyLowManyCycleInstrRead1 {
    [self testFile:@"RdyLowManyCycleInstrRead1.txt"];
}

- (void)testRdyLowManyCycleInstrRead2 {
    [self testFile:@"RdyLowManyCycleInstrRead2.txt"];
}

- (void)testRdyLowManyCycleInstrRead3 {
    [self testFile:@"RdyLowManyCycleInstrRead3.txt"];
}

- (void)testRdyLowManyCycleInstrRead4 {
    [self testFile:@"RdyLowManyCycleInstrRead4.txt"];
}

- (void)testRdyLowManyCycleInstrRead5 {
    [self testFile:@"RdyLowManyCycleInstrRead5.txt"];
}

- (void)testRdyLowManyCycleInstrRead6 {
    [self testFile:@"RdyLowManyCycleInstrRead6.txt"];
}

- (void)testRdyLowManyCycleInstrRead7 {
    [self testFile:@"RdyLowManyCycleInstrRead7.txt"];   // TODO: ajouté don't check pour a et p a certains cycles car le lda charge le resultat si rdy low sur le cycle d'alu
}

- (void)testRdyLowManyCycleInstrRead8 {
    [self testFile:@"RdyLowManyCycleInstrRead8.txt"];   // TODO: ajouté don't check pour a et p a certains cycles car le lda charge le resultat si rdy low sur le cycle d'alu
}

- (void)testRdyLowManyCycleInstrRMW1 {
    [self testFile:@"RdyLowManyCycleInstrRMW1.txt"];
}

- (void)testRdyLowManyCycleInstrRMW2 {
    [self testFile:@"RdyLowManyCycleInstrRMW2.txt"];
}

- (void)testRdyLowManyCycleInstrRMW3 {
    [self testFile:@"RdyLowManyCycleInstrRMW3.txt"];
}

- (void)testRdyLowManyCycleInstrRMW4 {
    [self testFile:@"RdyLowManyCycleInstrRMW4.txt"];
}

- (void)testRdyLowManyCycleInstrRMW5 {
    [self testFile:@"RdyLowManyCycleInstrRMW5.txt"];
}

- (void)testRdyLowManyCycleInstrRMW6 {
    [self testFile:@"RdyLowManyCycleInstrRMW6.txt"];
}

- (void)testRdyLowManyCycleInstrRMW7 {
    [self testFile:@"RdyLowManyCycleInstrRMW7.txt"];
}

- (void)testRdyLowManyCycleInstrRMW8 {
    [self testFile:@"RdyLowManyCycleInstrRMW8.txt"];
}

- (void)testRdyLowManyCycleInstrRMW9 {
    [self testFile:@"RdyLowManyCycleInstrRMW9.txt"];
}

- (void)testRdyLowManyCycleInstrRMW10 {
    [self testFile:@"RdyLowManyCycleInstrRMW10.txt"];
}

- (void)testRdyLowManyCycleInstrRMW11 {
    [self testFile:@"RdyLowManyCycleInstrRMW11.txt"];
}

- (void)testRdyLowManyCycleInstrRMW12 {
    [self testFile:@"RdyLowManyCycleInstrRMW12.txt"];
}

- (void)testRdyLowManyCycleInstrWrite1 {
    [self testFile:@"RdyLowManyCycleInstrWrite1.txt"];
}

- (void)testRdyLowManyCycleInstrWrite2 {
    [self testFile:@"RdyLowManyCycleInstrWrite2.txt"];
}

- (void)testRdyLowManyCycleInstrWrite3 {
    [self testFile:@"RdyLowManyCycleInstrWrite3.txt"];
}

- (void)testRdyLowManyCycleInstrWrite4 {
    [self testFile:@"RdyLowManyCycleInstrWrite4.txt"];
}

- (void)testRdyLowManyCycleInstrWrite5 {
    [self testFile:@"RdyLowManyCycleInstrWrite5.txt"];
}

- (void)testRdyLowManyCycleInstrWrite6 {
    [self testFile:@"RdyLowManyCycleInstrWrite6.txt"];
}

- (void)testRdyLowManyCycleInstrWrite7 {
    [self testFile:@"RdyLowManyCycleInstrWrite7.txt"];
}

- (void)testRdyLowManyCycleInstrWrite8 {
    [self testFile:@"RdyLowManyCycleInstrWrite8.txt"];
}

- (void)testRdyLowManyCycleInstrASO1 {
    [self testFile:@"RdyLowManyCycleInstrASO1.txt"];
}

- (void)testRdyLowManyCycleInstrASO2 {
    [self testFile:@"RdyLowManyCycleInstrASO2.txt"];
}

- (void)testRdyLowManyCycleInstrASO3 {
    [self testFile:@"RdyLowManyCycleInstrASO3.txt"];
}

- (void)testRdyLowManyCycleInstrASO4 {
    [self testFile:@"RdyLowManyCycleInstrASO4.txt"];
}

- (void)testRdyLowManyCycleInstrASO5 {
    [self testFile:@"RdyLowManyCycleInstrASO5.txt"];
}

- (void)testRdyLowManyCycleInstrASO6 {
    [self testFile:@"RdyLowManyCycleInstrASO6.txt"];
}

- (void)testRdyLowManyCycleInstrASO7 {
    [self testFile:@"RdyLowManyCycleInstrASO7.txt"];
}

- (void)testRdyLowManyCycleInstrASO8 {
    [self testFile:@"RdyLowManyCycleInstrASO8.txt"];
}

- (void)testRdyLowManyCycleInstrASO9 {
    [self testFile:@"RdyLowManyCycleInstrASO9.txt"];    // TODO: ajouté don't check pour a et p a certains cycles car aso charge le resultat si rdy low sur le cycle d'alu
}

- (void)testRdyLowManyCycleInstrASO10 {
    [self testFile:@"RdyLowManyCycleInstrASO10.txt"];    // TODO: ajouté don't check pour a et p a certains cycles car aso charge le resultat si rdy low sur le cycle d'alu
}

- (void)testRdyLowManyCycleInstrASO11 {
    [self testFile:@"RdyLowManyCycleInstrASO11.txt"];    // TODO: ajouté don't check pour a et p a certains cycles car aso charge le resultat si rdy low sur le cycle d'alu
}

- (void)testRdyLowManyCycleInstrASO12 {
    [self testFile:@"RdyLowManyCycleInstrASO12.txt"];    // TODO: ajouté don't check pour a et p a certains cycles car aso charge le resultat si rdy low sur le cycle d'alu
}

/*
- (void)testRdyLowOneHalfCycleInstrALUAND1 {
    [self testFile:@"RdyLowOneHalfCycleInstrALUAND1.txt"];  // TODO : jusqu'a 8 mais probleme avec AND car il se comporte differement, voir : https://retrocomputing.stackexchange.com/questions/13672/6502-and-instruction-updates-flags-differently-than-other-logic-operations
}*/

- (void)testRdyLowOneHalfCycleInstrALU1 {
    [self testFile:@"RdyLowOneHalfCycleInstrALU1.txt"];
}

- (void)testRdyLowOneHalfCycleInstrALU2 {
    [self testFile:@"RdyLowOneHalfCycleInstrALU2.txt"];
}

- (void)testRdyLowOneHalfCycleInstrALU3 {
    [self testFile:@"RdyLowOneHalfCycleInstrALU3.txt"];
}

- (void)testRdyLowOneHalfCycleInstrALU4 {
    [self testFile:@"RdyLowOneHalfCycleInstrALU4.txt"];
}

- (void)testRdyLowOneHalfCycleInstrALU5 {
    [self testFile:@"RdyLowOneHalfCycleInstrALU5.txt"];
}

- (void)testRdyLowOneHalfCycleInstrALU6 {
    [self testFile:@"RdyLowOneHalfCycleInstrALU6.txt"];
}

- (void)testRdyLowOneHalfCycleInstrALU7 {
    [self testFile:@"RdyLowOneHalfCycleInstrALU7.txt"];
}

- (void)testRdyLowOneHalfCycleInstrALU8 {
    [self testFile:@"RdyLowOneHalfCycleInstrALU8.txt"];
}

- (void)testRdyLowHalfCycleInstrRead1 {
    [self testFile:@"RdyLowHalfCycleInstrRead1.txt"];
}

- (void)testRdyLowHalfCycleInstrRead2 {
    [self testFile:@"RdyLowHalfCycleInstrRead2.txt"];
}

- (void)testRdyLowHalfCycleInstrRead3 {
    [self testFile:@"RdyLowHalfCycleInstrRead3.txt"];
}

- (void)testRdyLowHalfCycleInstrRead4 {
    [self testFile:@"RdyLowHalfCycleInstrRead4.txt"];
}

- (void)testRdyLowHalfCycleInstrRead5 {
    [self testFile:@"RdyLowHalfCycleInstrRead5.txt"];
}

- (void)testRdyLowHalfCycleInstrRead6 {
    [self testFile:@"RdyLowHalfCycleInstrRead6.txt"];
}

- (void)testRdyLowHalfCycleInstrRead7 {
    [self testFile:@"RdyLowHalfCycleInstrRead7.txt"];   // TODO: ajouté don't check pour a et p a certains cycles car le lda charge le resultat si rdy low sur le cycle d'alu
}

- (void)testRdyLowHalfCycleInstrRead8 {
    [self testFile:@"RdyLowHalfCycleInstrRead8.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW1 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW1.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW2 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW2.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW3 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW3.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW4 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW4.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW5 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW5.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW6 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW6.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW7 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW7.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW8 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW8.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW9 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW9.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW10 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW10.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW11 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW11.txt"];
}

- (void)testRdyLowOneHalfCycleInstrRMW12 {
    [self testFile:@"RdyLowOneHalfCycleInstrRMW12.txt"];
}

- (void)testRdyLowOneHalfCycleInstrWrite1 {
    [self testFile:@"RdyLowOneHalfCycleInstrWrite1.txt"];
}

- (void)testRdyLowOneHalfCycleInstrWrite2 {
    [self testFile:@"RdyLowOneHalfCycleInstrWrite2.txt"];
}

- (void)testRdyLowOneHalfCycleInstrWrite3 {
    [self testFile:@"RdyLowOneHalfCycleInstrWrite3.txt"];
}

- (void)testRdyLowOneHalfCycleInstrWrite4 {
    [self testFile:@"RdyLowOneHalfCycleInstrWrite4.txt"];
}

- (void)testRdyLowOneHalfCycleInstrWrite5 {
    [self testFile:@"RdyLowOneHalfCycleInstrWrite5.txt"];
}

- (void)testRdyLowOneHalfCycleInstrWrite6 {
    [self testFile:@"RdyLowOneHalfCycleInstrWrite6.txt"];
}

- (void)testRdyLowOneHalfCycleInstrWrite7 {
    [self testFile:@"RdyLowOneHalfCycleInstrWrite7.txt"];
}

- (void)testRdyLowOneHalfCycleInstrWrite8 {
    [self testFile:@"RdyLowOneHalfCycleInstrWrite8.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO1 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO1.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO2 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO2.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO3 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO3.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO4 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO4.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO5 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO5.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO6 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO6.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO7 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO7.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO8 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO8.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO9 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO9.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO10 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO10.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO11 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO11.txt"];
}

- (void)testRdyLowOneHalfCycleInstrASO12 {
    [self testFile:@"RdyLowOneHalfCycleInstrASO12.txt"];
}

- (void)testResetInRdyLow {
    [self testFile:@"ResetInRdyLow.txt"];
}

@end
