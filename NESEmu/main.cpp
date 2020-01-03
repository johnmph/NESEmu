//
//  main.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 31/12/19.
//  Copyright © 2019 Jonathan Baliko. All rights reserved.
//

#include <iostream>
#include <array>
#include "Cpu6502.hpp"


struct Bus {
    uint8_t read(uint16_t address) {
        return _memory[address];
    }
    
    void write(uint16_t address, uint8_t data) {
        _memory[address] = data;
    }
    
//private:
    std::array<uint8_t, 1024 * 32> _memory;
};

int main(int argc, const char * argv[]) {
    Bus bus;
    Cpu6502<Bus> cpu(bus);
    
    int pc = 0;
    bus._memory[pc++] = 0;      // CLV
    bus._memory[pc++] = 1;      // LDA $80
    bus._memory[pc++] = 0x80;
    bus._memory[pc++] = 2;      // LDA $0123 (0x90)
    bus._memory[pc++] = 0x23;
    bus._memory[pc++] = 0x01;
    bus._memory[pc++] = 3;      // STA $0124 (0x90)
    bus._memory[pc++] = 0x24;
    bus._memory[pc++] = 0x01;
    bus._memory[pc++] = 4;      // LDA $40 (0x22)
    bus._memory[pc++] = 0x40;
    bus._memory[pc++] = 5;      // STA $41 (0x22)
    bus._memory[pc++] = 0x41;
    bus._memory[pc++] = 7;      // LDA $1234, X
    bus._memory[pc++] = 0x34;
    bus._memory[pc++] = 0x12;
    bus._memory[pc++] = 8;      // STA $1235; Y
    bus._memory[pc++] = 0x35;
    bus._memory[pc++] = 0x12;
    
    
    bus._memory[0x0040] = 0x22;
    bus._memory[0x0123] = 0x90;
    bus._memory[0x1235] = 0xAB;
    bus._memory[0x1237] = 0x0;
    
    cpu.reset(true);
    cpu.clock();
    cpu.clock();
    cpu.clock();        // LDA $80
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
    cpu.clock();        // LDA $1234, X
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();        // STA $1235, Y
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    
    return 0;
}
