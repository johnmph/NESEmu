//
//  Attributes_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef VisualUnitTest_Cpu2A03_Attributes_s_hpp
#define VisualUnitTest_Cpu2A03_Attributes_s_hpp


template <class TCpu2A03>
AttributeCAb<TCpu2A03>::AttributeCAb(std::string const &name) : Attribute<TCpu2A03>(name) {
}

template <class TCpu2A03>
void AttributeCAb<TCpu2A03>::setValue(std::string const &value) {
    _cAb = std::stoi(value, 0, 16);
}

template <class TCpu2A03>
bool AttributeCAb<TCpu2A03>::check(TCpu2A03 &cpu2A03) {
    return cpu2A03.getInternalAddressBus() == _cAb;
}

template <class TCpu2A03>
AttributeCDb<TCpu2A03>::AttributeCDb(std::string const &name) : Attribute<TCpu2A03>(name) {
}

template <class TCpu2A03>
void AttributeCDb<TCpu2A03>::setValue(std::string const &value) {
    _cDb = std::stoi(value, 0, 16);
}

template <class TCpu2A03>
bool AttributeCDb<TCpu2A03>::check(TCpu2A03 &cpu2A03) {
    return cpu2A03.getInternalDataBus() == _cDb;
}

template <class TCpu2A03>
AttributeCRw<TCpu2A03>::AttributeCRw(std::string const &name) : Attribute<TCpu2A03>(name) {
}

template <class TCpu2A03>
void AttributeCRw<TCpu2A03>::setValue(std::string const &value) {
    _cRw = std::stoi(value, 0, 10);
}

template <class TCpu2A03>
bool AttributeCRw<TCpu2A03>::check(TCpu2A03 &cpu2A03) {
    return static_cast<bool>(cpu2A03.getInternalReadWriteSignal()) == _cRw;
}

template <class TCpu2A03>
std::unique_ptr<Attribute<TCpu2A03>> attributeFactory(std::string const &name) {
    if (name == "c_ab") {
        return std::make_unique<AttributeCAb<TCpu2A03>>(name);
    }
    
    if (name == "c_db") {
        return std::make_unique<AttributeCDb<TCpu2A03>>(name);
    }
    
    if (name == "c_rw") {
        return std::make_unique<AttributeCRw<TCpu2A03>>(name);
    }
    
    // Check 6502 attributes
    return Cpu6502::attributeFactory<TCpu2A03>(name);
}

#endif /* VisualUnitTest_Cpu2A03_Attributes_s_hpp */
