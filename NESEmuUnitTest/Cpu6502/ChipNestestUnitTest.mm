//
//  ChipNestestUnitTest.mm
//  NESEmuUnitTest
//
//  Created by Jonathan Baliko on 14/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#import <XCTest/XCTest.h>
#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include "Cpu6502/Chip.hpp"
#include "Cpu6502FullAccess.hpp"


@interface ChipNestestUnitTest : XCTestCase

@end

@implementation ChipNestestUnitTest

namespace {
    
    struct Bus {
        uint8_t read(uint16_t address) {
            //std::cout << std::hex << "Read 0x" << static_cast<int>(memory[address & 0xBFFF]) << " at 0x" << address << "\n";
            return memory[address & 0xBFFF];
        }
        
        void write(uint16_t address, uint8_t data) {
            //std::cout << std::hex << "Write 0x" << static_cast<int>(data) << " at 0x" << address << "\n";
            memory[address & 0xBFFF] = data;
        }
        
        std::array<uint8_t, 1024 * 64> memory;
    };
    
    struct State {
        uint16_t pc;
        uint8_t data[3];
        int dataSize;
        uint8_t a;
        uint8_t x;
        uint8_t y;
        uint8_t p;
        uint8_t sp;
        int cycle;
    };
    
    
    State decodeCurrentState(std::string const &line) {
        State state;
        
        // Extract data, not very efficient because a new sub string is created each time we parse data, it's better to use string_view but for simplicity we keep this code (not need to be efficient)
        
        std::size_t currentIndex = 0;
        
        // Extract PC
        state.pc = std::stoi(line.substr(currentIndex, 4), 0, 16);
        
        // Skip PC and 2 spaces
        currentIndex += 6;
        
        // Extract data
        for (state.dataSize = 0; state.dataSize < 3; ++state.dataSize) {
            if (line[currentIndex] == ' ') {
                break;
            }
            
            state.data[state.dataSize] = std::stoi(line.substr(currentIndex, 2), 0, 16);
            
            // Skip data and 1 space
            currentIndex += 3;
        }
        
        // Extract accumulator
        currentIndex = 48/*line.find("A:", currentIndex)*/ + 2;      // No need to search, it will always at the same position
        state.a = std::stoi(line.substr(currentIndex, 2), 0, 16);
        
        // Skip accumulator, 1 space and x label
        currentIndex += 5;
        
        // Extract x index
        state.x = std::stoi(line.substr(currentIndex, 2), 0, 16);
        
        // Skip x index, 1 space and y label
        currentIndex += 5;
        
        // Extract y index
        state.y = std::stoi(line.substr(currentIndex, 2), 0, 16);
        
        // Skip y index, 1 space and p label
        currentIndex += 5;
        
        // Extract status
        state.p = std::stoi(line.substr(currentIndex, 2), 0, 16);
        
        // Skip status, 1 space and sp label
        currentIndex += 6;
        
        // Extract stack pointer
        state.sp = std::stoi(line.substr(currentIndex, 2), 0, 16);
        
        // Extract cycles
        currentIndex = 86/*line.find("CYC:", currentIndex)*/ + 4;    // No need to search, it will always at the same position
        state.cycle = std::stoi(line.substr(currentIndex));
        
        return state;
    }
    
    
    Bus bus;
    Cpu6502FullAccess<Bus> cpu6502(bus);
    
}

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
    
    // Initialize with NOP
    bus.memory.fill(0xEA);
    
    // Power up cpu (Nestest needs an accumulator starting at 0 and status flag without Z flag)
    cpu6502.powerUp(0x00FF, 0x0, 0x0, 0x0, 0x0, 0x20);
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testNestestLog {
    // By default nestest.nes and nestest.log must be in /private/tmp path, so a BUILD_DIR Other C flag is added to get the build dir
    
    // Open nestest ROM
    std::ifstream ifsRom(std::string(BUILD_DIR) + "/UnitTestFiles/nestest.nes", std::ios::binary);
    
    // Check that file exists
    XCTAssertTrue(ifsRom.good());
    
    // Skip INES header
    ifsRom.seekg(0x10);
    
    // Read ROM
    ifsRom.read(reinterpret_cast<char *>(&bus.memory.data()[0x8000]), 0x4000);
    
    // Close file
    ifsRom.close();
    
    // Set reset vector
    bus.write(0xFFFC, 0x00);
    bus.write(0xFFFD, 0xC0);
    
    // Release reset to start cpu
    cpu6502.reset(true);
    
    // We need to sync with nestest.log by adding three first clocks to exit the reset state (2 clocks to exit early reset and 1 clock for the fetchOpcode (the last 7 clocks for reset are in the first line of log))
    cpu6502.clock();
    cpu6502.clock();
    cpu6502.clock();
    
    // Open nestest Log
    std::ifstream ifsLog(std::string(BUILD_DIR) + "/UnitTestFiles/nestest.log");
    
    // Check that file exists
    XCTAssertTrue(ifsLog.good());
    
    // Instructions check loop
    int cycle = 0;
    for (std::string line; std::getline(ifsLog, line);) {
        // Decode current state in log
        auto state = decodeCurrentState(line);
        
        // Execute CPU until all cycles of current state are done
        for (; cycle < state.cycle; ++cycle) {
            cpu6502.clock();
        }
        
        // Check state with current Cpu state
        XCTAssertTrue(cpu6502.getProgramCounter() == state.pc);
        XCTAssertTrue(cpu6502.getDataBus() == state.data[0]);
        
        // There are some instructions which delay the result on the next cycle because they use the decodeOpcodeAndExecuteInstruction cycle to write the result back, for these instructions, we need to process one extra cycle to have correct result
        if ((cpu6502.getAccumulator() != state.a) || (cpu6502.getXIndex() != state.x) || (cpu6502.getYIndex() != state.y) || (cpu6502.getStatusFlags() != state.p) || (cpu6502.getStackPointer() != state.sp)) {
            cpu6502.clock();
            cycle += 1;
        }
        
        XCTAssertTrue(cpu6502.getAccumulator() == state.a);
        XCTAssertTrue(cpu6502.getXIndex() == state.x);
        XCTAssertTrue(cpu6502.getYIndex() == state.y);
        XCTAssertTrue(cpu6502.getStatusFlags() == state.p);
        XCTAssertTrue(cpu6502.getStackPointer() == state.sp);
    }
    
    // Close file
    ifsLog.close();
}

/*
- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}*/

@end
