//
//  Interface.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Interface_hpp
#define NESEmu_Cartridge_Interface_hpp

#include <memory>
#include "../Implementation/Nes.hpp"


namespace NESEmu { namespace Cartridge {
    
    template <Model EModel, class TGraphicHardware, class TLoopManager>
    struct Interface {
        
        virtual ~Interface() = 0;
        
        virtual std::unique_ptr<Implementation::Interface> createNesImplementation(TGraphicHardware &graphicHardware, TLoopManager &loopManager) = 0;
        
    };
    
    #include "Interface_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Interface_hpp */
