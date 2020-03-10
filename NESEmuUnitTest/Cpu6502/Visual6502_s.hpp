//
//  Visual6502_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 6/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Visual6502_s_hpp
#define Visual6502_s_hpp


template <class TCpu6502>
Attribute<TCpu6502>::Attribute(std::string const &name) : _name(name), _enableCheck(true) {
}

template <class TCpu6502>
Attribute<TCpu6502>::~Attribute() {
}

template <class TCpu6502>
std::string const &Attribute<TCpu6502>::getName() {
    return _name;
}

template <class TCpu6502>
void Attribute<TCpu6502>::setEnableCheck(bool enableCheck) {
    _enableCheck = enableCheck;
}

template <class TCpu6502>
bool Attribute<TCpu6502>::compareValue(TCpu6502 &cpu6502) {
    if (_enableCheck) {
        return check(cpu6502);
    }
    
    return true;
}

template <class TCpu6502>
AttributeDontCheck<TCpu6502>::AttributeDontCheck(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributeDontCheck<TCpu6502>::setValue(std::string const &value) {
}

template <class TCpu6502>
bool AttributeDontCheck<TCpu6502>::check(TCpu6502 &cpu6502) {
    return true;
}

template <class TCpu6502>
AttributeAb<TCpu6502>::AttributeAb(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributeAb<TCpu6502>::setValue(std::string const &value) {
    _ab = std::stoi(value, 0, 16);
}

template <class TCpu6502>
bool AttributeAb<TCpu6502>::check(TCpu6502 &cpu6502) {
    return cpu6502.getAddressBus() == _ab;
}

template <class TCpu6502>
AttributeDb<TCpu6502>::AttributeDb(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributeDb<TCpu6502>::setValue(std::string const &value) {
    _db = std::stoi(value, 0, 16);
}

template <class TCpu6502>
bool AttributeDb<TCpu6502>::check(TCpu6502 &cpu6502) {
    return cpu6502.getDataBus() == _db;
}

template <class TCpu6502>
AttributeRw<TCpu6502>::AttributeRw(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributeRw<TCpu6502>::setValue(std::string const &value) {
    _rw = std::stoi(value, 0, 10);
}

template <class TCpu6502>
bool AttributeRw<TCpu6502>::check(TCpu6502 &cpu6502) {
    return cpu6502.getReadWriteSignal() == _rw;
}

template <class TCpu6502>
AttributePc<TCpu6502>::AttributePc(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributePc<TCpu6502>::setValue(std::string const &value) {
    _pc = std::stoi(value, 0, 16);
}

template <class TCpu6502>
bool AttributePc<TCpu6502>::check(TCpu6502 &cpu6502) {
    return cpu6502.getProgramCounter() == _pc;
}

template <class TCpu6502>
AttributePcl<TCpu6502>::AttributePcl(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributePcl<TCpu6502>::setValue(std::string const &value) {
    _pcl = std::stoi(value, 0, 16);
}

template <class TCpu6502>
bool AttributePcl<TCpu6502>::check(TCpu6502 &cpu6502) {
    return (cpu6502.getProgramCounter() & 0xFF) == _pcl;
}

template <class TCpu6502>
AttributePch<TCpu6502>::AttributePch(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributePch<TCpu6502>::setValue(std::string const &value) {
    _pch = std::stoi(value, 0, 16);
}

template <class TCpu6502>
bool AttributePch<TCpu6502>::check(TCpu6502 &cpu6502) {
    return (cpu6502.getProgramCounter() >> 8) == _pch;
}

template <class TCpu6502>
AttributeA<TCpu6502>::AttributeA(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributeA<TCpu6502>::setValue(std::string const &value) {
    _a = std::stoi(value, 0, 16);
}

template <class TCpu6502>
bool AttributeA<TCpu6502>::check(TCpu6502 &cpu6502) {
    return cpu6502.getAccumulator() == _a;
}

template <class TCpu6502>
AttributeX<TCpu6502>::AttributeX(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributeX<TCpu6502>::setValue(std::string const &value) {
    _x = std::stoi(value, 0, 16);
}

template <class TCpu6502>
bool AttributeX<TCpu6502>::check(TCpu6502 &cpu6502) {
    return cpu6502.getXIndex() == _x;
}

template <class TCpu6502>
AttributeY<TCpu6502>::AttributeY(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributeY<TCpu6502>::setValue(std::string const &value) {
    _y = std::stoi(value, 0, 16);
}

template <class TCpu6502>
bool AttributeY<TCpu6502>::check(TCpu6502 &cpu6502) {
    return cpu6502.getYIndex() == _y;
}

template <class TCpu6502>
AttributeS<TCpu6502>::AttributeS(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributeS<TCpu6502>::setValue(std::string const &value) {
    _s = std::stoi(value, 0, 16);
}

template <class TCpu6502>
bool AttributeS<TCpu6502>::check(TCpu6502 &cpu6502) {
    return cpu6502.getStackPointer() == _s;
}

template <class TCpu6502>
AttributeP<TCpu6502>::AttributeP(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributeP<TCpu6502>::setValue(std::string const &value) {
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

template <class TCpu6502>
bool AttributeP<TCpu6502>::check(TCpu6502 &cpu6502) {
    // B is not a flag, it doesn't really exists, it's just the value pushed on the stack which differency BRK instruction from interrupts
    // In Visual6502, B is representing the node which conditionally drives the data bus during a push of P
    return (cpu6502.getStatusFlags() & 0xEF) == (_p & 0xEF);
}

template <class TCpu6502>
AttributeSync<TCpu6502>::AttributeSync(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributeSync<TCpu6502>::setValue(std::string const &value) {
    _sync = std::stoi(value, 0, 10);
}

template <class TCpu6502>
bool AttributeSync<TCpu6502>::check(TCpu6502 &cpu6502) {
    return cpu6502.getSyncSignal() == _sync;
}

template <class TCpu6502>
AttributeReadyLine<TCpu6502>::AttributeReadyLine(std::string const &name) : Attribute<TCpu6502>(name) {
}

template <class TCpu6502>
void AttributeReadyLine<TCpu6502>::setValue(std::string const &value) {
    _readyLine = std::stoi(value, 0, 10);
}

template <class TCpu6502>
bool AttributeReadyLine<TCpu6502>::check(TCpu6502 &cpu6502) {
    return cpu6502.getReadyLine() == _readyLine;
}

template <class TCpu6502>
template <class TFunction>
Analyzer<TCpu6502>::Analyzer(std::istream &istream, TFunction &&writeFunction) : _istream(istream) {
    std::string line;
    
    // Read and decode url
    std::getline(_istream, line);
    decodeUrl(line);
    
    // Read and decode attributes
    std::getline(_istream, line);
    decodeAttributes(line);
    
    // Set data in memory
    for (auto const &data : _command.data) {
        for (int i = 0; i < data.second.size(); ++i) {
            writeFunction(data.first + i, data.second[i]);
        }
    }
}

template <class TCpu6502>
template <class TFunction>
void Analyzer<TCpu6502>::analyze(TCpu6502 &cpu6502, TFunction &&checkResult, int startCycle) {
    int cycle = startCycle;
    int lastResultFullCycle = -1;
    
    for (std::string line; std::getline(_istream, line);) {
        // Decode current result
        auto result = decodeResults(line);
        
        int resultFullCycle = std::stoi(result[0], 0, 10);
        
        while ((cycle <= (resultFullCycle * 2)) || (lastResultFullCycle == resultFullCycle)) {
            // Execute CPU
            if ((cycle & 0x1) == 0x0) {
                cpu6502.startPhi1();
            } else {
                cpu6502.startPhi2();
            }
            
            // Check commands (not on cycle 0 like Visual6502)
            if (cycle > 0) {
                for (auto const &resetLine : _command.resetLine) {
                    if (resetLine.first == cycle) {
                        cpu6502.reset(false);
                    }
                    
                    if (resetLine.second == cycle) {
                        cpu6502.reset(true);
                    }
                }
                
                for (auto const &nmiLine : _command.nmiLine) {
                    if (nmiLine.first == cycle) {
                        cpu6502.nmi(false);
                    }
                    
                    if (nmiLine.second == cycle) {
                        cpu6502.nmi(true);
                    }
                }
                
                for (auto const &irqLine : _command.irqLine) {
                    if (irqLine.first == cycle) {
                        cpu6502.irq(false);
                    }
                    
                    if (irqLine.second == cycle) {
                        cpu6502.irq(true);
                    }
                }
                
                for (auto const &soLine : _command.soLine) {
                    if (soLine.first == cycle) {
                        cpu6502.setOverflow(false);
                    }
                    
                    if (soLine.second == cycle) {
                        cpu6502.setOverflow(true);
                    }
                }
                
                for (auto const &rdyLine : _command.rdyLine) {
                    if (rdyLine.first == cycle) {
                        cpu6502.ready(false);
                    }
                    
                    if (rdyLine.second == cycle) {
                        cpu6502.ready(true);
                    }
                }
            }
            
            // Execute CPU
            if ((cycle & 0x1) == 0x0) {
                cpu6502.endPhi1();
            } else {
                cpu6502.endPhi2();
            }
            
            //std::cout << cycle << ") " << std::hex << cpu6502.getAddressBus() << ", " << static_cast<int>(cpu6502.getDataBus()) << "\n";
            
            // Check attribute enable
            for (int i = 0; i < _attributes.attributes.size(); ++i) {
                auto &attribute = *_attributes.attributes[i];
                
                for (auto const &checkAttribute : _command.checkAttribute) {
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
            }
            
            // Increment cycle count
            ++cycle;
            
            // Check steps
            if (cycle >= _command.numCyclesToExecute) {
                return;
            }
            
            // If we are in phi2, exit this loop
            if (lastResultFullCycle == resultFullCycle) {
                break;
            }
        }
        
        // Save resultFullCycle
        lastResultFullCycle = resultFullCycle;
        
        // Check result
        for (int i = 0; i < _attributes.attributes.size(); ++i) {
            auto &attribute = *_attributes.attributes[i];
            
            attribute.setValue(result[i]);
            checkResult(attribute.compareValue(cpu6502), attribute.getName());
        }
    }
}

template <class TCpu6502>
std::unique_ptr<Attribute<TCpu6502>> Analyzer<TCpu6502>::makeAttributeFromName(std::string const &name) {
    if (name == "ab") {
        return std::make_unique<AttributeAb<TCpu6502>>(name);
    }
    
    if (name == "db") {
        return std::make_unique<AttributeDb<TCpu6502>>(name);
    }
    
    if (name == "rw") {
        return std::make_unique<AttributeRw<TCpu6502>>(name);
    }
    
    if (name == "pc") {
        return std::make_unique<AttributePc<TCpu6502>>(name);
    }
    
    if (name == "pcl") {
        return std::make_unique<AttributePcl<TCpu6502>>(name);
    }
    
    if (name == "pch") {
        return std::make_unique<AttributePch<TCpu6502>>(name);
    }
    
    if (name == "a") {
        return std::make_unique<AttributeA<TCpu6502>>(name);
    }
    
    if (name == "x") {
        return std::make_unique<AttributeX<TCpu6502>>(name);
    }
    
    if (name == "y") {
        return std::make_unique<AttributeY<TCpu6502>>(name);
    }
    
    if (name == "s") {
        return std::make_unique<AttributeS<TCpu6502>>(name);
    }
    
    if (name == "p") {
        return std::make_unique<AttributeP<TCpu6502>>(name);
    }
    
    if (name == "sync") {
        return std::make_unique<AttributeSync<TCpu6502>>(name);
    }
    
    if (name == "rdy") {
        return std::make_unique<AttributeReadyLine<TCpu6502>>(name);
    }
    
    return std::make_unique<AttributeDontCheck<TCpu6502>>(name);
}

template <class TCpu6502>
void Analyzer<TCpu6502>::decodeUrlParameter(std::string const &name, std::string const &value, UrlCommand &command) {
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
    
    // Rdy low
    if (name == "rdy0") {
        command.rdyLine.push_back({ std::stoi(value, 0, 10), 0 });
        return;
    }
    
    // So high
    if (name == "rdy1") {
        if (command.rdyLine.rbegin() != command.rdyLine.rend()) {
            command.rdyLine.rbegin()->second = std::stoi(value, 0, 10);
        }
        
        return;
    }
}

template <class TCpu6502>
void Analyzer<TCpu6502>::decodeUrl(std::string const &url) {
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
        
        decodeUrlParameter(parameterName, parameterValue, _command);
    }
}

template <class TCpu6502>
void Analyzer<TCpu6502>::decodeAttributes(std::string attributesString) {
    // Add a tab to attributesString to save also last attribute
    attributesString += '\t';
    
    std::string attribute;
    for (auto index = 0; index < attributesString.size(); ++index) {
        // Save attribute and skip tabs and spaces
        if ((attributesString[index] == '\t') || (attributesString[index] == ' ')) {
            if (!attribute.empty()) {
                _attributes.attributes.push_back(makeAttributeFromName(attribute));
                attribute = "";
            }
            
            continue;
        }
        
        // Save char
        attribute += attributesString[index];
    }
}

template <class TCpu6502>
std::vector<std::string> Analyzer<TCpu6502>::decodeResults(std::string resultsString) {
    std::vector<std::string> results;
    
    // Add a tab to resultsString to save also last result
    resultsString += '\t';
    
    std::string result;
    for (auto index = 0; index < resultsString.size(); ++index) {
        // Save result and skip tabs (but not spaces)
        //if ((resultsString[index] == '\t') || (resultsString[index] == ' ')) {
        if (resultsString[index] == '\t') {
            if (!result.empty()) {
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

#endif /* Visual6502_s_hpp */
