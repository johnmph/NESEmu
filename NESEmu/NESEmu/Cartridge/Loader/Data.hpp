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
#include "../Model.hpp"
#include "../Common.hpp"


namespace NESEmu { namespace Cartridge { namespace Loader {
    
    struct Data {
        
        Model mapperModel;
        
        std::vector<uint8_t> prgRom;
        std::size_t prgRamSize;
        std::vector<uint8_t> chrRom;
        std::size_t chrRamSize;
        
        MirroringType mirroringType;
        
        bool hasPersistentMemory;
    };
    
} } }

#endif /* NESEmu_Cartridge_Loader_Data_hpp */
