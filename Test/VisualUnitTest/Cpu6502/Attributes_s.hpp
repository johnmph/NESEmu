//
//  Attributes_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 6/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef VisualUnitTest_Cpu6502_Attributes_s_hpp
#define VisualUnitTest_Cpu6502_Attributes_s_hpp


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
    return static_cast<bool>(cpu6502.getReadWriteSignal()) == _rw;
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
std::unique_ptr<Attribute<TCpu6502>> attributeFactory(std::string const &name) {
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

#endif /* VisualUnitTest_Cpu6502_Attributes_s_hpp */
