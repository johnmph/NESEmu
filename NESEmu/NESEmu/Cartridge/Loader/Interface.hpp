//
//  Interface.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Loader_Interface_hpp
#define NESEmu_Cartridge_Loader_Interface_hpp

#include <istream>
#include "Data.hpp"


namespace NESEmu { namespace Cartridge { namespace Loader {
    
    struct Interface {
        
        virtual ~Interface() = default;
        
        virtual bool isCartridgeSupported(std::istream &istream) = 0;
        virtual Data getCartridgeDataFromStream(std::istream &istream) = 0;
        
    };
    
} } }

#endif /* NESEmu_Cartridge_Loader_Interface_hpp */
