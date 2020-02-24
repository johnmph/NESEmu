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
#include "Cpu6502/Chip.hpp"
#include "Nes.hpp"
#include "NESEmu/Cartridge/Mapper0.hpp"


struct Bus {
    uint8_t read(uint16_t address) {
        //std::cout << std::hex << "Read 0x" << static_cast<int>(memory[address & 0xBFFF]) << " at 0x" << address << "\n";
        return memory[address & 0xBFFF];
    }
    
    void write(uint16_t address, uint8_t data) {
        //std::cout << std::hex << "Write 0x" << static_cast<int>(data) << " at 0x" << address << "\n";
        memory[address & 0xBFFF] = data;
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
        _data = memory[_address & 0xBFFF];
    }
    
    void performWrite() {
        //std::cout << std::hex << "Write 0x" << static_cast<int>(_data) << " at 0x" << _address << "\n";
        memory[_address & 0xBFFF] = _data;
    }
    
    std::array<uint8_t, 1024 * 64> memory;
    uint16_t _address;
    uint8_t _data;
};

struct GraphicHardware {
    uint8_t getColorFromIndex(uint8_t color, bool r, bool g, bool b) {
        return 0;
    }
    
    void plotPixel(unsigned int x, unsigned int y, uint8_t color) {
    }
    
    void notifyVBlankStarted() {
    }
};


int main(int argc, const char * argv[]) {
    Bus bus;
    Cpu6502::Chip<Cpu6502::ConfigurationPerformance<Bus>> cpu(bus); // Performance = 4.65sec, Accuracy = 5.3sec (Pour 100 * 1 seconde de cycles)
    
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
    bus.memory.fill(0xEA);
    
    // Read test file
    std::ifstream ifs("../UnitTestFiles/nestest.nes", std::ios::binary);
    //std::ifstream ifs("../UnitTestFiles/official_only.nes", std::ios::binary);
    
    // Check that file exists
    assert(ifs.good());
    
    ifs.seekg(0x10);
    ifs.read(reinterpret_cast<char *>(&bus.memory.data()[0x8000]), 0x4000);
    //ifs.read(reinterpret_cast<char *>(&bus.memory.data()[0x8000]), 0x8000);
    ifs.close();
    
    // Reset vector
    bus.write(0xFFFC, 0x00);
    bus.write(0xFFFD, 0xC0);
    //bus.write(0xFFFD, 0x80);
    
    // Power up cpu
    cpu.powerUp();
    
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
    for (int x = 0; x <= 178977300/*26555*/; ++x) {
        cpu.clock();
        
        //std::cout << std::hex << cpu.getAddressBus() << " " << cpu.getProgramCounter() << " A:" << static_cast<int>(cpu.getAccumulator()) << " X:" << static_cast<int>(cpu.getXIndex()) << " Y:" << static_cast<int>(cpu.getYIndex()) << " P:" << static_cast<int>(cpu.getStatusFlags()) << " SP:" << static_cast<int>(cpu.getStackPointer()) << " Cycle: " << std::dec << x - 1 << "\n";
    }
    
    /*
    std::ofstream ofs("memory.log", std::ios::binary);
    ofs.write(reinterpret_cast<char *>(bus._memory.data()), bus._memory.size());
    ofs.close();*/
    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << std::dec << "Time in milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "\n";
    
    std::ifstream ifs2("../UnitTestFiles/nestest.nes", std::ios::binary);
    GraphicHardware graphicHardware;
    NESEmu::Cartridge::Mapper0<32, 0, NESEmu::Cartridge::MirroringType::Horizontal> mapper0(ifs2);
    NESEmu::Nes<NESEmu::Model::Ntsc, NESEmu::Cartridge::Mapper0<32, 0, NESEmu::Cartridge::MirroringType::Horizontal>, GraphicHardware> nes(mapper0, graphicHardware);
    
    nes.clock();
    
    return 0;
}
