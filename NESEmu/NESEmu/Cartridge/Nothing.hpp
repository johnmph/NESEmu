//
//  Nothing.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 10/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Nothing_hpp
#define NESEmu_Cartridge_Nothing_hpp

#include "Cartridge.hpp"
#include "../Mapper/Nothing.hpp"


namespace NESEmu { namespace Cartridge {
    
    template <Model EModel, class TGraphicHardware, class TLoopManager>
    struct Nothing : Cartridge<EModel, TGraphicHardware, TLoopManager, Mapper::NoMapper> {
        
        Nothing();
        
    };
    
    #include "Nothing_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Nothing_hpp */
