//
//  Interface.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Loader_Interface_hpp
#define NESEmu_Cartridge_Loader_Interface_hpp

#include <memory>
#include <istream>
#include "../Cartridge.hpp"
#include "../../Mapper/Mapper0.hpp"//TODO: inclure les autres


namespace NESEmu { namespace Cartridge { namespace Loader {
    
    template <Model EModel, class TGraphicHardware, class TLoopManager>
    struct Interface {
        
        virtual ~Interface() = 0;
        
        virtual std::unique_ptr<::NESEmu::Cartridge::Interface<EModel, TGraphicHardware, TLoopManager>> createCartridgeFromStream(std::istream &istream) = 0;
        
    };
    
    #include "Interface_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Loader_Interface_hpp */
