//
//  Zapper.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Controller_Zapper_hpp
#define NESEmu_Controller_Zapper_hpp

// See https://wiki.nesdev.com/w/index.php/Zapper

#include "Interface.hpp"


namespace NESEmu { namespace Controller {
    
    template <class TControllerManager>
    struct Zapper : Interface {
        
        explicit Zapper(TControllerManager &controllerManager);
        
        void clock(uint8_t &data) override;
        void out(bool high) override;
        
    private:
        TControllerManager &_controllerManager;
    };
    
    #include "Zapper_s.hpp"
    
} }

#endif /* NESEmu_Controller_Zapper_hpp */
