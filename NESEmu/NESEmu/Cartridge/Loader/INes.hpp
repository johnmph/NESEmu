//
//  INes.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Loader_INes_hpp
#define NESEmu_Cartridge_Loader_INes_hpp

#include "Interface.hpp"


namespace NESEmu { namespace Cartridge { namespace Loader {
    
    template <Model EModel, class TGraphicHardware, class TLoopManager>
    struct INes : Interface<EModel, TGraphicHardware, TLoopManager> {
        
        std::unique_ptr<::NESEmu::Cartridge::Interface<EModel, TGraphicHardware, TLoopManager>> createCartridgeFromStream(std::istream &istream) override;
        
    };
    
    #include "INes_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Loader_INes_hpp */
