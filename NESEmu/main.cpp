//
//  main.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 31/12/19.
//  Copyright © 2019 Jonathan Baliko. All rights reserved.
//

#include <iostream>
#include "Cpu6502.hpp"


enum class Flags {
    Carry,
    Zero,
    InterruptDisable,
    DecimalMode,
    Break,
    Unused,
    Overflow,
    Negative
};

constexpr uint8_t getEnableFlagsMask(std::initializer_list<Flags> const &flags) {
    uint8_t mask = 0x0;
    
    for (auto const &flag : flags) {
        mask |= 1 << static_cast<uint8_t>(flag);
    }
    
    return mask;
}

constexpr uint8_t getDisableFlagsMask(std::initializer_list<Flags> const &flags) {
    return ~getEnableFlagsMask(flags);
}

constexpr void disableFlags(uint8_t &status, std::initializer_list<Flags> const &flags) {
    status &= getDisableFlagsMask(flags);
}

void setFlag(uint8_t &status, Flags flag, bool value) {
    status |= value << static_cast<uint8_t>(flag);
}

int main(int argc, const char * argv[]) {
    // insert code here...
    
    
    uint8_t status = 10;
    disableFlags(status, { Flags::Carry, Flags::Overflow });
    setFlag(status, Flags::Carry, true);
    setFlag(status, Flags::Overflow, false);
    
    //uint8_t mask = getFlagsMask({ Flags::Carry, Flags::Overflow });
    
    //std::cout << c << "\n";
    
    return 0;
}
