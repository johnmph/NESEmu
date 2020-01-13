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
        std::cout << std::hex << "Read 0x" << static_cast<int>(_memory[address & 0xBFFF]) << " at 0x" << address << "\n";
        return _memory[address & 0xBFFF];
    }
    
    void write(uint16_t address, uint8_t data) {
        std::cout << std::hex << "Write 0x" << static_cast<int>(data) << " at 0x" << address << "\n";
        _memory[address & 0xBFFF] = data;
    }
    
//private:
    std::array<uint8_t, 1024 * 64> _memory;
};
/*
uint8_t _adderHold;
uint8_t _programCounterLow, _programCounterHigh;
uint8_t _aInput, _bInput;
bool _aluOverflow, _aluCarry;


void aluInvertBInput() {
    _bInput = ~_bInput;
}

void aluPerformSum(bool decimalEnable, bool carryIn) {//TODO: terminer (halfCarry, decimalEnable)
    uint16_t add = _aInput + _bInput + carryIn;
    _adderHold = static_cast<uint8_t>(add);
    
    _aluOverflow = (_aInput ^ _adderHold) & (_bInput ^ _adderHold) & 0x80;
    _aluCarry = add & 0x100;
}


void relativeBranch0() {    // TODO: a voir (surtout pour le test du ++PC et --PC)
    _programCounterLow = _adderHold;
    
    // bInput must be programCounterLow and aInput the offset
    if (_aluOverflow == false) { // if ((_bInput ^ _aInput) & (_bInput ^ _adderHold) & 0x80) {
        // Correct programCounterHigh
        if (_aluCarry == true) { // if (_bInput & 0x80) {
            ++_programCounterHigh;
        }
        else {
            --_programCounterHigh;
        }
    }
}
*/
// + + No PCrossed -> true, false
// + + PCrossed    -> false, true
// + - No PCrossed -> true, true
// + - PCrossed    -> false, false

int main(int argc, const char * argv[]) {
    /*_programCounterHigh = 0x1;
    _programCounterLow = 0x82;
    
    // Adding offset with programCounterLow using ALU
    _aInput = 0x70;
    _bInput = _programCounterLow;
    aluPerformSum(false, false);
    
    relativeBranch0();
    
    return 0;
    */
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
    //std::ifstream ifs("official_only.nes", std::ios::binary);
    ifs.seekg(0x10);
    ifs.read(reinterpret_cast<char *>(&bus._memory.data()[0x8000]), 0x4000);
    //ifs.read(reinterpret_cast<char *>(&bus._memory.data()[0x8000]), 0x8000);
    ifs.close();
    
    // Reset vector
    bus.write(0xFFFC, 0x00);
    bus.write(0xFFFD, 0xC0);
    //bus.write(0xFFFD, 0x80);
    
    // Release reset to start cpu
    cpu.reset(true);
    /*
    bus.write(0xC000, 0xEA);
    bus.write(0xC001, 0xEA);
    bus.write(0xC002, 0xEA);
    bus.write(0xC003, 0xEA);
    bus.write(0xC004, 0xEA);
    bus.write(0xC005, 0xEA);
    bus.write(0xC006, 0xEA);
    bus.write(0xC007, 0xEA);
    bus.write(0xC008, 0xEA);
    
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu.clock();
    cpu._statusFlags &= cpu.getStatusFlagsDisableMask({ Cpu6502<Bus>::Flags::InterruptDisable });
    cpu.irq(false);*/
    
    // NTSC = 1789773 hz = 1sec / 1789773 = 0.0000005587 sec par cycle CPU
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    
    // TODO: check avec http://www.qmtpro.com/~nes/misc/nestest.log et http://www.qmtpro.com/~nes/misc/nestest.txt
    for (int x = 0; x <= /*1789773*/26555; ++x) {
        cpu.clock();
        
        std::cout << std::hex << cpu.getProgramCounter() << " A:" << static_cast<int>(cpu._accumulator) << " X:" << static_cast<int>(cpu._xIndex) << " Y:" << static_cast<int>(cpu._yIndex) << " P:" << static_cast<int>(cpu._statusFlags) << " SP:" << static_cast<int>(cpu._stackPointer) << " Cycle: " << std::dec << x << "\n";
    }
    
    std::cout << std::hex << static_cast<int>(bus._memory[0x0002]) << ", " << static_cast<int>(bus._memory[0x0003]) << "\n";
    
    std::ofstream ofs("memory.log", std::ios::binary);
    ofs.write(reinterpret_cast<char *>(bus._memory.data()), bus._memory.size());
    ofs.close();
    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << std::dec << "Time in milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "\n";
    
    return 0;
}
