//
//  Analyzer_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 6/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef VisualUnitTest_Analyzer_s_hpp
#define VisualUnitTest_Analyzer_s_hpp


template <class TCpu>
template <class TAttributeFactoryFunction, class TWriteFunction>
Analyzer<TCpu>::Analyzer(std::istream &istream, TAttributeFactoryFunction &&attributeFactoryFunction, TWriteFunction &&writeFunction) : _istream(istream) {
    std::string line;
    
    // Read and decode url
    std::getline(_istream, line);
    decodeUrl(line);
    
    // Read and decode attributes
    std::getline(_istream, line);
    decodeAttributes(attributeFactoryFunction, line);
    
    // Set data in memory
    for (auto const &data : _command.data) {
        for (int i = 0; i < data.second.size(); ++i) {
            writeFunction(data.first + i, data.second[i]);
        }
    }
}

template <class TCpu>
template <class TFunction>
void Analyzer<TCpu>::analyze(TCpu &cpu6502, TFunction &&checkResult, int startCycle) {
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
                
                cpu6502.correctDataBusForVisual6502();
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

template <class TCpu>
void Analyzer<TCpu>::decodeUrlParameter(std::string const &name, std::string const &value, UrlCommand &command) {
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
        command.data.push_back({ std::stoi(value, 0, 16), std::vector<uint8_t>() });
        return;
    }
    
    // Data
    if (name == "d") {
        // Check to ensure that we have first set an address in url
        if (command.data.size() > 0) {
            auto &commandDataBytes = command.data.back().second;
            
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

template <class TCpu>
void Analyzer<TCpu>::decodeUrl(std::string const &url) {
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

template <class TCpu>
template <class TAttributeFactoryFunction>
void Analyzer<TCpu>::decodeAttributes(TAttributeFactoryFunction &&attributeFactoryFunction, std::string attributesString) {
    // Add a tab to attributesString to save also last attribute
    attributesString += '\t';
    
    std::string attribute;
    for (auto index = 0; index < attributesString.size(); ++index) {
        // Save attribute and skip tabs and spaces
        if ((attributesString[index] == '\t') || (attributesString[index] == ' ')) {
            if (!attribute.empty()) {
                _attributes.attributes.push_back(attributeFactoryFunction(attribute));
                attribute = "";
            }
            
            continue;
        }
        
        // Save char
        attribute += attributesString[index];
    }
}

template <class TCpu>
std::vector<std::string> Analyzer<TCpu>::decodeResults(std::string resultsString) {
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

#endif /* VisualUnitTest_Cpu6502_Analyzer_s_hpp */
