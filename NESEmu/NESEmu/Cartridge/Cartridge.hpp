//
//  Cartridge.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Cartridge_hpp
#define NESEmu_Cartridge_Cartridge_hpp

#include "Interface.hpp"


namespace NESEmu { namespace Cartridge {
    
    template <Model EModel, class TGraphicHardware, class TLoopManager, class TMapper>
    struct Cartridge : Interface<EModel, TGraphicHardware, TLoopManager> {
        
        Cartridge(TMapper const &mapper);
        
        std::unique_ptr<Implementation::Interface> createNesImplementation(TGraphicHardware &graphicHardware, TLoopManager &loopManager) override;
        
    private:
        TMapper _mapper;
    };
    
    #include "Cartridge_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Cartridge_hpp */
