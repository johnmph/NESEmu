//
//  INes2.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Loader_INes2_hpp
#define NESEmu_Cartridge_Loader_INes2_hpp

// See https://www.nesdev.org/wiki/NES_2.0

#include "Interface.hpp"


namespace NESEmu { namespace Cartridge { namespace Loader {
    
    struct INes2 : Interface {

        static Model mapperModels[];
        
        bool isCartridgeSupported(std::istream &istream) override;
        Data getCartridgeDataFromStream(std::istream &istream) override;
        
    private:
        std::size_t getPrgRomSize(uint8_t lsb, uint8_t msb);
        std::size_t getChrRomSize(uint8_t lsb, uint8_t msb);
        std::size_t getRomSize(uint8_t lsb, uint8_t msb, unsigned int bankSize);
    };
    
} } }

#endif /* NESEmu_Cartridge_Loader_INes2_hpp */
