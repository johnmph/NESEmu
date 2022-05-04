//
//  INes.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Loader_INes_hpp
#define NESEmu_Cartridge_Loader_INes_hpp

// See https://wiki.nesdev.com/w/index.php/INES

#include "Interface.hpp"


namespace NESEmu { namespace Cartridge { namespace Loader {
    
    struct INes : Interface {

        static Model mapperModels[];
        
        bool isCartridgeSupported(std::istream &istream) override;
        Data getCartridgeDataFromStream(std::istream &istream) override;
        
    };
    
} } }

#endif /* NESEmu_Cartridge_Loader_INes_hpp */
