//
//  Nothing.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 10/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Nothing_hpp
#define NESEmu_Cartridge_Nothing_hpp

#include "Interface.hpp"
#include "../Mapper/Nothing.hpp"


namespace NESEmu { namespace Cartridge {
    
    template <Model EModel, class TGraphicHardware, class TLoopManager>
    struct Nothing : Interface<EModel, TGraphicHardware, TLoopManager> {
        
        std::unique_ptr<Implementation::Interface> createNesImplementation(TGraphicHardware &graphicHardware, TLoopManager &loopManager) override;
        
    };
    
    #include "Nothing_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Nothing_hpp */
