//
//  main.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 31/12/19.
//  Copyright © 2019 Jonathan Baliko. All rights reserved.
//

#include <iostream>
#include <array>
#include <fstream>
#include <chrono>
#include "Cpu6502.hpp"


struct Bus {
    uint8_t read(uint16_t address) {
        //std::cout << std::hex << "Read 0x" << static_cast<int>(_memory[address & 0xBFFF]) << " at 0x" << address << "\n";
        return _memory[address & 0xBFFF];
    }
    
    void write(uint16_t address, uint8_t data) {
        //std::cout << std::hex << "Write 0x" << static_cast<int>(data) << " at 0x" << address << "\n";
        _memory[address & 0xBFFF] = data;
    }
    
//private:
    std::array<uint8_t, 1024 * 64> _memory;
};

int main(int argc, const char * argv[]) {
    Bus bus;
    Cpu6502<Bus> cpu(bus);
    /*
    cpu._aInput = 0x50;
    cpu._bInput = 0x10;
    cpu.aluPerformSum(false, false);
    
    cpu._aInput = 0x50;
    cpu._bInput = 0x50;
    cpu.aluPerformSum(false, false);
    
    cpu._aInput = 0x50;
    cpu._bInput = 0x90;
    cpu.aluPerformSum(false, false);
    
    cpu._aInput = 0x50;
    cpu._bInput = 0xd0;
    cpu.aluPerformSum(false, false);
    
    cpu._aInput = 0xd0;
    cpu._bInput = 0x10;
    cpu.aluPerformSum(false, false);
    
    cpu._aInput = 0xd0;
    cpu._bInput = 0x50;
    cpu.aluPerformSum(false, false);
    
    cpu._aInput = 0xd0;
    cpu._bInput = 0x90;
    cpu.aluPerformSum(false, false);
    
    cpu._aInput = 0xd0;
    cpu._bInput = 0xd0;
    cpu.aluPerformSum(false, false);*/
    
    /*
    bus._memory[0xFFFC] = 0;
    bus._memory[0xFFFD] = 0;
    
    int pc = 0;
    bus._memory[pc++] = 0xB8;   // CLV
    bus._memory[pc++] = 0xA9;   // LDA #$80
    bus._memory[pc++] = 0x80;
    bus._memory[pc++] = 0xAD;   // LDA $0123 (0x90)
    bus._memory[pc++] = 0x23;
    bus._memory[pc++] = 0x01;
    bus._memory[pc++] = 0x8D;   // STA $0124 (0x90)
    bus._memory[pc++] = 0x24;
    bus._memory[pc++] = 0x01;
    bus._memory[pc++] = 0xA5;   // LDA $40 (0x22)
    bus._memory[pc++] = 0x40;
    bus._memory[pc++] = 0x85;   // STA $41 (0x22)
    bus._memory[pc++] = 0x41;
    bus._memory[pc++] = 0xBD;   // LDA $1234, X (0xFF)
    bus._memory[pc++] = 0x34;
    bus._memory[pc++] = 0x12;
    bus._memory[pc++] = 0x99;   // STA $1235, Y (2)
    bus._memory[pc++] = 0x35;
    bus._memory[pc++] = 0x12;
    
    
    bus._memory[0x0040] = 0x22;
    bus._memory[0x0123] = 0x90;
    bus._memory[0x1235] = 0xAB;
    bus._memory[0x1237] = 0x0;
    bus._memory[0x1333] = 0xCA;
    
    // Some clock to check reset loop
    cpu.clock();
    cpu.clock();
    cpu.clock();
    
    // Release reset to start cpu
    cpu.reset(true);
    
    // Start sequence (7 cycles + 2 cycles to read first opcode)
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    
    cpu.clock();        // CLV
    cpu.clock();
    cpu.clock();        // LDA #$80
    cpu.clock();
    cpu.clock();        // LDA $0123
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();        // STA $0124
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();        // LDA $40
    cpu.clock();
    cpu.clock();
    cpu.clock();        // STA $41
    cpu.clock();
    cpu.clock();
    cpu.clock();        // LDA $1234, X (0xFF)
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();        // STA $1235, Y (2)
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();*/
    
    // Initialize with NOP
    bus._memory.fill(0xEA);
    
    // Read test file
    std::ifstream ifs("nestest.nes", std::ios::binary);
    ifs.seekg(0x10);
    ifs.read(reinterpret_cast<char *>(&bus._memory.data()[0x8000]), 0x4000);
    
    // Reset vector
    bus.write(0xFFFC, 0x00);
    bus.write(0xFFFD, 0xC0);
    
    // Release reset to start cpu
    cpu.reset(true);
    
    // NTSC = 1789773 hz = 1sec / 1789773 = 0.0000005587 sec par cycle CPU
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    
    for (int x = 0; x <= 1789773; ++x) {    // 26555
        //std::cout << std::hex << (cpu.getProgramCounter() - 1) << " A:" << static_cast<int>(cpu._accumulator) << " X:" << static_cast<int>(cpu._xIndex) << " Y:" << static_cast<int>(cpu._yIndex) << " P:" << static_cast<int>(cpu._statusFlags) << " SP:" << static_cast<int>(cpu._stackPointer) << " Cycle: " << std::dec << x << "\n";
        
        cpu.clock();
    }
    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time in milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "\n";
    
    return 0;
}
