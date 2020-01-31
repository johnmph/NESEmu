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
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <memory>
#include "Cpu6502/Chip.hpp"


@interface ChipVisual6502UnitTest : XCTestCase

@end

@implementation ChipVisual6502UnitTest

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
    
    struct Cpu6502InternalViewer {
        
        Cpu6502InternalViewer(Cpu6502::Chip<Bus, Cpu6502InternalViewer> const &cpu6502) : _cpu6502(cpu6502) {
        }
        
        Cpu6502::Chip<Bus, Cpu6502InternalViewer> const &getCpu6502() const {
            return _cpu6502;
        }
        
        uint16_t getProgramCounter() const {
            return (_cpu6502._programCounterHigh << 8) | _cpu6502._programCounterLow;
        }
        
        uint8_t getStackPointer() const {
            return _cpu6502._stackPointer;
        }
        
        uint8_t getAccumulator() const {
            return _cpu6502._accumulator;
        }
        
        uint8_t getXIndex() const {
            return _cpu6502._xIndex;
        }
        
        uint8_t getYIndex() const {
            return _cpu6502._yIndex;
        }
        
        uint8_t getStatusFlags() const {
            return _cpu6502._statusFlags;
        }
        
    private:
        Cpu6502::Chip<Bus, Cpu6502InternalViewer> const &_cpu6502;
    };
    
    struct Visual6502UrlCommand {
        Visual6502UrlCommand() : numCyclesToExecute(0) {
        }
        
        std::map<int, std::vector<uint8_t>> data;
        int numCyclesToExecute;
        std::vector<std::pair<int, int>> nmiLine;
        std::vector<std::pair<int, int>> irqLine;
        std::vector<std::pair<int, int>> soLine;
    };
    
    struct Visual6502Attribute {
        virtual ~Visual6502Attribute() {
        }
        
        virtual void setValue(std::string const &value) = 0;
        virtual bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) = 0;
    };
    
    struct Visual6502AttributeDontCheck : Visual6502Attribute {
        void setValue(std::string const &value) override {
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            return true;
        }
    };
    
    struct Visual6502AttributeAb : Visual6502Attribute {
        void setValue(std::string const &value) override {
            _ab = std::stoi(value, 0, 16);
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getCpu6502().getAddressBus() == _ab;
        }
        
    private:
        uint16_t _ab;
    };
    
    struct Visual6502AttributeDb : Visual6502Attribute {
        void setValue(std::string const &value) override {
            _db = std::stoi(value, 0, 16);
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getCpu6502().getDataBus() == _db;
        }
        
    private:
        uint8_t _db;
    };
    
    struct Visual6502AttributeRw : Visual6502Attribute {
        void setValue(std::string const &value) override {
            _rw = std::stoi(value, 0, 10);
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getCpu6502().getReadWriteSignal() == _rw;
        }
        
    private:
        bool _rw;
    };
    
    struct Visual6502AttributePc : Visual6502Attribute {
        void setValue(std::string const &value) override {
            _pc = std::stoi(value, 0, 16);
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getProgramCounter() == _pc;
        }
        
    private:
        uint16_t _pc;
    };
    
    struct Visual6502AttributePcl : Visual6502Attribute {
        void setValue(std::string const &value) override {
            _pcl = std::stoi(value, 0, 16);
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            return (cpu6502InternalViewer.getProgramCounter() & 0xFF) == _pcl;
        }
        
    private:
        uint8_t _pcl;
    };
    
    struct Visual6502AttributePch : Visual6502Attribute {
        void setValue(std::string const &value) override {
            _pch = std::stoi(value, 0, 16);
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            return (cpu6502InternalViewer.getProgramCounter() >> 8) == _pch;
        }
        
    private:
        uint8_t _pch;
    };
    
    struct Visual6502AttributeA : Visual6502Attribute {
        void setValue(std::string const &value) override {
            _a = std::stoi(value, 0, 16);
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getAccumulator() == _a;
        }
        
    private:
        uint8_t _a;
    };
    
    struct Visual6502AttributeX : Visual6502Attribute {
        void setValue(std::string const &value) override {
            _x = std::stoi(value, 0, 16);
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getXIndex() == _x;
        }
        
    private:
        uint8_t _x;
    };
    
    struct Visual6502AttributeY : Visual6502Attribute {
        void setValue(std::string const &value) override {
            _y = std::stoi(value, 0, 16);
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getYIndex() == _y;
        }
        
    private:
        uint8_t _y;
    };
    
    struct Visual6502AttributeS : Visual6502Attribute {
        void setValue(std::string const &value) override {
            _s = std::stoi(value, 0, 16);
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getStackPointer() == _s;
        }
        
    private:
        uint8_t _s;
    };
    
    struct Visual6502AttributeP : Visual6502Attribute {
        void setValue(std::string const &value) override {
            _p = 0x20;
            
            _p |= (value[0] == 'N') << 7;
            _p |= (value[1] == 'V') << 6;
            // Unused flag always set, already set when initialized _p
            _p |= (value[3] == 'B') << 4;
            _p |= (value[4] == 'D') << 3;
            _p |= (value[5] == 'I') << 2;
            _p |= (value[6] == 'Z') << 1;
            _p |= (value[7] == 'C');
        }
        
        bool compareValue(Cpu6502InternalViewer &cpu6502InternalViewer) override {
            // B is not a flag, it doesn't really exists, it's just the value pushed on the stack which differency BRK instruction from interrupts
            // In Visual6502, B is representing the node which conditionally drives the data bus during a push of P
            return (cpu6502InternalViewer.getStatusFlags() & 0xEF) == (_p & 0xEF);
        }
        
    private:
        uint8_t _p;
    };
    
    struct Visual6502Attributes {
        std::vector<std::unique_ptr<Visual6502Attribute>> attributes;
    };
    
    
    std::unique_ptr<Visual6502Attribute> getAttributeFromName(std::string const &name) {
        if (name == "ab") {
            return std::make_unique<Visual6502AttributeAb>();
        }
        
        if (name == "db") {
            return std::make_unique<Visual6502AttributeDb>();
        }
        
        if (name == "rw") {
            return std::make_unique<Visual6502AttributeRw>();
        }
        
        if (name == "pc") {
            return std::make_unique<Visual6502AttributePc>();
        }
        
        if (name == "pcl") {
            return std::make_unique<Visual6502AttributePcl>();
        }
        
        if (name == "pch") {
            return std::make_unique<Visual6502AttributePch>();
        }
        
        if (name == "a") {
            return std::make_unique<Visual6502AttributeA>();
        }
        
        if (name == "x") {
            return std::make_unique<Visual6502AttributeX>();
        }
        
        if (name == "y") {
            return std::make_unique<Visual6502AttributeY>();
        }
        
        if (name == "s") {
            return std::make_unique<Visual6502AttributeS>();
        }
        
        if (name == "p") {
            return std::make_unique<Visual6502AttributeP>();
        }
        
        return std::make_unique<Visual6502AttributeDontCheck>();
    }
    
    void decodeVisual6502UrlParameter(std::string const &name, std::string const &value, Visual6502UrlCommand &command) {
        // Address
        if (name == "a") {
            command.data.insert({ std::stoi(value, 0, 16), std::vector<uint8_t>() });
            return;
        }
        
        // Data
        if (name == "d") {
            if (command.data.rbegin() != command.data.rend()) {
                auto &commandDataBytes = command.data.rbegin()->second;
                
                for (std::string::size_type index = 0; index < value.size(); index += 2) {
                    auto byte = value.substr(index, 2);
                    commandDataBytes.push_back(std::stoi(byte, 0, 16));
                }
            }
            
            return;
        }
        
        // Steps
        if (name == "steps") {
            command.numCyclesToExecute = std::stoi(value, 0, 10);
            return;
        }
        
        // Nmi low
        if (name == "nmi0") {
            command.nmiLine.push_back({ std::stoi(value, 0, 10), 0 });
            return;
        }
        
        // Nmi high
        if (name == "nmi1") {
            if (command.nmiLine.rbegin() != command.nmiLine.rend()) {
                command.nmiLine.rbegin()->second = std::stoi(value, 0, 10);
            }
            
            return;
        }
        
        // Irq low
        if (name == "irq0") {
            command.irqLine.push_back({ std::stoi(value, 0, 10), 0 });
            return;
        }
        
        // Irq high
        if (name == "irq1") {
            if (command.irqLine.rbegin() != command.irqLine.rend()) {
                command.irqLine.rbegin()->second = std::stoi(value, 0, 10);
            }
            
            return;
        }
        
        // So low
        if (name == "so0") {
            command.soLine.push_back({ std::stoi(value, 0, 10), 0 });
            return;
        }
        
        // So high
        if (name == "so1") {
            if (command.soLine.rbegin() != command.soLine.rend()) {
                command.soLine.rbegin()->second = std::stoi(value, 0, 10);
            }
            
            return;
        }
    }
    
    Visual6502UrlCommand decodeVisual6502Url(std::string const &url) {
        Visual6502UrlCommand command;
        
        // Browse url starting GET request
        for (auto index = url.find("?"); index != std::string::npos;) {
            // Skip search char
            ++index;
            
            // Get a parameter
            auto currentIndex = index;
            index = url.find("&", currentIndex);
            auto parameter = url.substr(currentIndex, index - currentIndex);
            
            // Decode parameter
            auto splitIndex = parameter.find("=");
            if (splitIndex == std::string::npos) {
                continue;
            }
            
            auto parameterName = parameter.substr(0, splitIndex);
            auto parameterValue = parameter.substr(splitIndex + 1);
            
            decodeVisual6502UrlParameter(parameterName, parameterValue, command);
        }
        
        return command;
    }
    
    Visual6502Attributes decodeVisual6502Attributes(std::string attributesString) {
        Visual6502Attributes attributes;
        
        // Add a tab to attributesString to save also last attribute
        attributesString += '\t';
        
        std::string attribute;
        for (auto index = 0; index < attributesString.size(); ++index) {
            // Save attribute and skip tabs and spaces
            if ((attributesString[index] == '\t') || (attributesString[index] == ' ')) {
                if (attribute.empty() == false) {
                    attributes.attributes.push_back(getAttributeFromName(attribute));
                    attribute = "";
                }
                
                continue;
            }
            
            // Save char
            attribute += attributesString[index];
        }
        
        return attributes;
    }
    
    std::vector<std::string> decodeVisual6502Results(std::string resultsString) {
        std::vector<std::string> results;
        
        // Add a tab to resultsString to save also last result
        resultsString += '\t';
        
        std::string result;
        for (auto index = 0; index < resultsString.size(); ++index) {
            // Save result and skip tabs (but not spaces)
            //if ((resultsString[index] == '\t') || (resultsString[index] == ' ')) {
            if (resultsString[index] == '\t') {
                if (result.empty() == false) {
                    results.push_back(result);
                    result = "";
                }
                
                continue;
            }
            
            // Save char
            result += resultsString[index];
        }
        
        return results;
    }
    
    
    Bus bus;
    Cpu6502::Chip<Bus, Cpu6502InternalViewer> cpu6502(bus);
    Cpu6502InternalViewer cpu6502InternalViewer(cpu6502);
    
}

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
    
    // Initialize with NOP
    bus.memory.fill(0xEA);
    
    // Power up cpu
    cpu6502.powerUp();
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testFile:(NSString *)filename {
    // By default filename must be in /private/tmp path, so a BUILD_DIR Other C flag is added to get the build dir
    
    // Set NMI vector
    bus.write(0xFFFA, 0x00);
    bus.write(0xFFFB, 0x00);
    
    // Set IRQ vector
    bus.write(0xFFFE, 0x00);
    bus.write(0xFFFF, 0x00);
    
    // Set reset vector
    bus.write(0xFFFC, 0x00);
    bus.write(0xFFFD, 0x00);
    
    // Open Visual6502 Log
    std::ifstream ifsLog(std::string(BUILD_DIR) + "/UnitTestFiles/" + [filename UTF8String]);
    
    // Check that file exists
    XCTAssertTrue(ifsLog.good());
    
    std::string line;
    
    // Read an decode url
    std::getline(ifsLog, line);
    auto commands = decodeVisual6502Url(line);
    
    // Read and decode attributes
    std::getline(ifsLog, line);
    auto attributes = decodeVisual6502Attributes(line);
    
    // Set data in memory
    for (auto const &data : commands.data) {
        for (int i = 0; i < data.second.size(); ++i) {
            bus.memory[data.first + i] = data.second[i];
        }
    }
    
    // Release reset to start cpu
    cpu6502.reset(true);
    
    // We need to sync with Visual6502 by adding nine first clocks to exit the reset state
    for (int i = 0; i < 9; ++i) {
        cpu6502.clock();
    }
    
    // Results check loop
    int cycle = 0;
    for (std::string line; std::getline(ifsLog, line);) {
        std::cout << cycle << ") " << std::hex << cpu6502.getAddressBus() << "\n";
        
        // Decode current result in log
        auto result = decodeVisual6502Results(line);
        
        // Check commands
        for (auto const &nmiLine : commands.nmiLine) {
            if (nmiLine.first == cycle) {
                cpu6502.nmi(false);
            }
            
            if (nmiLine.second == cycle) {
                cpu6502.nmi(true);
            }
        }
        
        for (auto const &irqLine : commands.irqLine) {
            if (irqLine.first == cycle) {
                cpu6502.irq(false);
            }
            
            if (irqLine.second == cycle) {
                cpu6502.irq(true);
            }
        }
        
        for (auto const &soLine : commands.soLine) {
            if (soLine.first == cycle) {
                cpu6502.setOverflow(false);
            }
            
            if (soLine.second == cycle) {
                cpu6502.setOverflow(true);
            }
        }
        
        // Cpu6502 does not perform half-cycle, so we check only phi2 cycle
        ++cycle;
        
        if ((cycle & 0x1) == 0x1) {
            continue;
        }
        
        // Check result
        for (int i = 0; i < attributes.attributes.size(); ++i) {
            auto &attribute = *attributes.attributes[i];
            
            attribute.setValue(result[i]);
            XCTAssertTrue(attribute.compareValue(cpu6502InternalViewer));
        }
        
        // Check steps
        if (cycle >= commands.numCyclesToExecute) {
            break;
        }
        
        // Execute CPU
        cpu6502.clock();
    }
    
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
    //[self testFile:@"SetOverflow1.txt"];    // TODO: c'est normal que ca ne marche pas car le signal est setté a phi1 mais le Cpu6502 fonctionne par cycle et non pas half-cycle, donc il est reset juste avant que le cycle s'execute et donc le flag n'est jamais setté
}

- (void)testSetOverflow2 {
    //[self testFile:@"SetOverflow2.txt"];    // TODO: c'est normal que ca ne marche pas car le signal est setté a phi2 mais le Cpu6502 fonctionne par cycle et non pas half-cycle, donc il est setté alors qu'il ne le devrait pas car le reset au phi1 n'est pas pris en compte
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

@end
