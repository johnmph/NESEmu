//
//  main.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "Nes.hpp"
#include "NESEmu/Cartridge/Mapper0.hpp"


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
    GraphicHardware graphicHardware;
    
    // Open ROM
    std::ifstream ifs("../UnitTestFiles/SMB.nes", std::ios::binary);
    
    // Mapper for SMB
    NESEmu::Cartridge::Mapper0<32, 0, NESEmu::Cartridge::MirroringType::Horizontal> mapper0(ifs);
    
    // Create NES with Mapper
    NESEmu::Nes<NESEmu::Model::Ntsc, NESEmu::Cartridge::Mapper0<32, 0, NESEmu::Cartridge::MirroringType::Horizontal>, GraphicHardware> nes(mapper0, graphicHardware);
    
    // Power up NES
    nes.powerUp();
    
    // Release reset to start NES
    nes.reset(true);
    
    // Clock loop
    for (;;) {  // TODO: rajouter une condition pour quitter
        nes.clock();
    }
    
    return 0;
}

