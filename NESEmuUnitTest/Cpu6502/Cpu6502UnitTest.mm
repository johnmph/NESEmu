//
//  Cpu6502UnitTest.m
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
#include "Cpu6502/Cpu6502.hpp"


@interface Cpu6502UnitTest : XCTestCase

@end

@implementation Cpu6502UnitTest

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
    Cpu6502<Bus> cpu6502(bus);
    
}

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
    
    // Initialize with NOP
    bus.memory.fill(0xEA);
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testNestestLog {
    // nestest.nes and nestest.log must be in /private/tmp path
    
    // Open nestest ROM
    std::ifstream ifsRom("nestest.nes", std::ios::binary);
    
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
    
    // We need to sync with nestest.log by adding two first clocks to exit the reset state, we also reset the accumulator because it is loaded with 0xAA on startup like visual6502
    cpu6502.clock();
    cpu6502.clock();
    cpu6502._accumulator = 0;
    
    // Open nestest Log
    std::ifstream ifsLog("nestest.log");
    
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
        XCTAssertTrue(cpu6502.getAddressBus() == state.pc);         // We don't use PC because it can be incremented before testing it)
        XCTAssertTrue(cpu6502.getDataBus() == state.data[0]);
        
        // There are some instructions which delay the result on the next cycle because they use the decodeOpcode cycle to write the result back, for these instructions, we need to process one extra cycle to have correct result
        if ((cpu6502._accumulator != state.a) || (cpu6502._xIndex != state.x) || (cpu6502._yIndex != state.y) || (cpu6502._statusFlags != state.p) || (cpu6502._stackPointer != state.sp)) {
            cpu6502.clock();
            cycle += 1;
        }
        
        XCTAssertTrue(cpu6502._accumulator == state.a);
        XCTAssertTrue(cpu6502._xIndex == state.x);
        XCTAssertTrue(cpu6502._yIndex == state.y);
        XCTAssertTrue(cpu6502._statusFlags == state.p);
        XCTAssertTrue(cpu6502._stackPointer == state.sp);
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
