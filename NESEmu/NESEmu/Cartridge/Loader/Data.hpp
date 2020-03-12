//
//  Data.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Loader_Data_hpp
#define NESEmu_Cartridge_Loader_Data_hpp

#include <vector>
#include "../../Mapper/Model.hpp"
#include "../../Mapper/Common.hpp"


namespace NESEmu { namespace Cartridge { namespace Loader {
    
    struct Data {
        
        Mapper::Model mapperModel;
        std::vector<uint8_t> prgRom;
        std::vector<uint8_t> prgRam;
        std::vector<uint8_t> chrRom;
        unsigned int chrRamSizeInKb;
        /*unsigned int prgRomSizeInKb;
        unsigned int prgRamSizeInKb;
        unsigned int chrRomSizeInKb;
        unsigned int chrRamSizeInKb;*/
        Mapper::MirroringType mirroringType;
    };
    
} } }

#endif /* NESEmu_Cartridge_Loader_Data_hpp */
