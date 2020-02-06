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
#include <unordered_map>
#include <map>
#include <vector>
#include <utility>
#include <memory>
#include "Cpu6502/Chip.hpp"
#include "Cpu6502InternalViewer.hpp"


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
    
    struct Visual6502UrlCommand {
        Visual6502UrlCommand() : numCyclesToExecute(0) {
        }
        
        std::unordered_map<std::string, std::vector<std::pair<int, int>>> checkAttribute;
        std::map<int, std::vector<uint8_t>> data;
        int numCyclesToExecute;
        std::vector<std::pair<int, int>> resetLine;
        std::vector<std::pair<int, int>> nmiLine;
        std::vector<std::pair<int, int>> irqLine;
        std::vector<std::pair<int, int>> soLine;
    };
    
    struct Visual6502Attribute {
        Visual6502Attribute(std::string const &name) : _name(name), _enableCheck(true) {
        }
        
        virtual ~Visual6502Attribute() {
        }
        
        std::string const &getName() {
            return _name;
        }
        
        void setEnableCheck(bool enableCheck) {
            _enableCheck = enableCheck;
        }
        
        bool compareValue(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) {
            if (_enableCheck == true) {
                return check(cpu6502InternalViewer);
            }
            
            return true;
        }
        
        virtual void setValue(std::string const &value) = 0;
        
    protected:
        virtual bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) = 0;
        
    private:
        std::string _name;
        bool _enableCheck;
    };
    
    struct Visual6502AttributeDontCheck : Visual6502Attribute {
        Visual6502AttributeDontCheck(std::string const &name) : Visual6502Attribute(name) {
        }
        
        void setValue(std::string const &value) override {
        }
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
            return true;
        }
    };
    
    struct Visual6502AttributeAb : Visual6502Attribute {
        Visual6502AttributeAb(std::string const &name) : Visual6502Attribute(name) {
        }
        
        void setValue(std::string const &value) override {
            _ab = std::stoi(value, 0, 16);
        }
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getCpu6502().getAddressBus() == _ab;
        }
        
    private:
        uint16_t _ab;
    };
    
    struct Visual6502AttributeDb : Visual6502Attribute {
        Visual6502AttributeDb(std::string const &name) : Visual6502Attribute(name) {
        }
        
        void setValue(std::string const &value) override {
            _db = std::stoi(value, 0, 16);
        }
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getCpu6502().getDataBus() == _db;
        }
        
    private:
        uint8_t _db;
    };
    
    struct Visual6502AttributeRw : Visual6502Attribute {
        Visual6502AttributeRw(std::string const &name) : Visual6502Attribute(name) {
        }
        
        void setValue(std::string const &value) override {
            _rw = std::stoi(value, 0, 10);
        }
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getCpu6502().getReadWriteSignal() == _rw;
        }
        
    private:
        bool _rw;
    };
    
    struct Visual6502AttributePc : Visual6502Attribute {
        Visual6502AttributePc(std::string const &name) : Visual6502Attribute(name) {
        }
        
        void setValue(std::string const &value) override {
            _pc = std::stoi(value, 0, 16);
        }
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getProgramCounter() == _pc;
        }
        
    private:
        uint16_t _pc;
    };
    
    struct Visual6502AttributePcl : Visual6502Attribute {
        Visual6502AttributePcl(std::string const &name) : Visual6502Attribute(name) {
        }
        
        void setValue(std::string const &value) override {
            _pcl = std::stoi(value, 0, 16);
        }
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
            return (cpu6502InternalViewer.getProgramCounter() & 0xFF) == _pcl;
        }
        
    private:
        uint8_t _pcl;
    };
    
    struct Visual6502AttributePch : Visual6502Attribute {
        Visual6502AttributePch(std::string const &name) : Visual6502Attribute(name) {
        }
        
        void setValue(std::string const &value) override {
            _pch = std::stoi(value, 0, 16);
        }
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
            return (cpu6502InternalViewer.getProgramCounter() >> 8) == _pch;
        }
        
    private:
        uint8_t _pch;
    };
    
    struct Visual6502AttributeA : Visual6502Attribute {
        Visual6502AttributeA(std::string const &name) : Visual6502Attribute(name) {
        }
        
        void setValue(std::string const &value) override {
            _a = std::stoi(value, 0, 16);
        }
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getAccumulator() == _a;
        }
        
    private:
        uint8_t _a;
    };
    
    struct Visual6502AttributeX : Visual6502Attribute {
        Visual6502AttributeX(std::string const &name) : Visual6502Attribute(name) {
        }
        
        void setValue(std::string const &value) override {
            _x = std::stoi(value, 0, 16);
        }
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getXIndex() == _x;
        }
        
    private:
        uint8_t _x;
    };
    
    struct Visual6502AttributeY : Visual6502Attribute {
        Visual6502AttributeY(std::string const &name) : Visual6502Attribute(name) {
        }
        
        void setValue(std::string const &value) override {
            _y = std::stoi(value, 0, 16);
        }
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getYIndex() == _y;
        }
        
    private:
        uint8_t _y;
    };
    
    struct Visual6502AttributeS : Visual6502Attribute {
        Visual6502AttributeS(std::string const &name) : Visual6502Attribute(name) {
        }
        
        void setValue(std::string const &value) override {
            _s = std::stoi(value, 0, 16);
        }
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
            return cpu6502InternalViewer.getStackPointer() == _s;
        }
        
    private:
        uint8_t _s;
    };
    
    struct Visual6502AttributeP : Visual6502Attribute {
        Visual6502AttributeP(std::string const &name) : Visual6502Attribute(name) {
        }
        
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
        
        bool check(Cpu6502InternalViewer<Bus> &cpu6502InternalViewer) override {
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
            return std::make_unique<Visual6502AttributeAb>(name);
        }
        
        if (name == "db") {
            return std::make_unique<Visual6502AttributeDb>(name);
        }
        
        if (name == "rw") {
            return std::make_unique<Visual6502AttributeRw>(name);
        }
        
        if (name == "pc") {
            return std::make_unique<Visual6502AttributePc>(name);
        }
        
        if (name == "pcl") {
            return std::make_unique<Visual6502AttributePcl>(name);
        }
        
        if (name == "pch") {
            return std::make_unique<Visual6502AttributePch>(name);
        }
        
        if (name == "a") {
            return std::make_unique<Visual6502AttributeA>(name);
        }
        
        if (name == "x") {
            return std::make_unique<Visual6502AttributeX>(name);
        }
        
        if (name == "y") {
            return std::make_unique<Visual6502AttributeY>(name);
        }
        
        if (name == "s") {
            return std::make_unique<Visual6502AttributeS>(name);
        }
        
        if (name == "p") {
            return std::make_unique<Visual6502AttributeP>(name);
        }
        
        return std::make_unique<Visual6502AttributeDontCheck>(name);
    }
    
    void decodeVisual6502UrlParameter(std::string const &name, std::string const &value, Visual6502UrlCommand &command) {
        // Added parameter -> check0xx with xx as name of attribute = disable check for this attribute at the value cycle
        if (name.find("check0") == 0) {
            auto attributeName = name.substr(6);
            
            command.checkAttribute[attributeName].push_back({ std::stoi(value, 0, 10), 0 });
            return;
        }
        
        // Added parameter -> check1xx with xx as name of attribute = reenable check for this attribute at the value cycle
        if (name.find("check1") == 0) {
            auto attributeName = name.substr(6);
            
            if (command.checkAttribute[attributeName].rbegin() != command.checkAttribute[attributeName].rend()) {
                command.checkAttribute[attributeName].rbegin()->second = std::stoi(value, 0, 10);
            }
            
            return;
        }
        
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
        
        // Reset low
        if (name == "reset0") {
            command.resetLine.push_back({ std::stoi(value, 0, 10), 0 });
            return;
        }
        
        // Reset high
        if (name == "reset1") {
            if (command.resetLine.rbegin() != command.resetLine.rend()) {
                command.resetLine.rbegin()->second = std::stoi(value, 0, 10);
            }
            
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
    Cpu6502InternalViewer<Bus>::Chip cpu6502(bus);
    Cpu6502InternalViewer<Bus> cpu6502InternalViewer(cpu6502);
    
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
            bus.write(data.first + i, data.second[i]);
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
        // Decode current result in log
        auto result = decodeVisual6502Results(line);
        
        // Check commands
        for (auto const &resetLine : commands.resetLine) {
            if (resetLine.first == cycle) {
                cpu6502.reset(false);
            }
            
            if (resetLine.second == cycle) {
                cpu6502.reset(true);
            }
        }
        
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
        
        // Execute CPU
        if ((cycle & 0x1) == 0x0) {
            cpu6502.clockPhi1();
        } else {
            cpu6502.clockPhi2();
        }
        
        //std::cout << cycle << ") " << std::hex << cpu6502.getAddressBus() << ", " << static_cast<int>(cpu6502.getDataBus()) << "\n";
        
        // Check result
        for (int i = 0; i < attributes.attributes.size(); ++i) {
            auto &attribute = *attributes.attributes[i];
            
            for (auto const &checkAttribute : commands.checkAttribute) {
                if (checkAttribute.first != attribute.getName()) {
                    continue;
                }
                
                for (auto const &checkCycles : checkAttribute.second) {
                    if (checkCycles.first == cycle) {
                        attribute.setEnableCheck(false);
                    }
                    
                    if (checkCycles.second == cycle) {
                        attribute.setEnableCheck(true);
                    }
                }
            }
            
            attribute.setValue(result[i]);
            XCTAssertTrue(attribute.compareValue(cpu6502InternalViewer));
        }
        
        // Increment cycle count
        ++cycle;
        
        // Check steps
        if (cycle >= commands.numCyclesToExecute) {
            break;
        }
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

@end
