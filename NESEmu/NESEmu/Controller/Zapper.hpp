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


namespace NESEmu { namespace Controller {//TODO: a faire, voir https://wiki.nesdev.com/w/index.php/Input_devices pour l'interface, il faudra peut etre la modifier (pour standard aussi)
    
    template <class TControllerHardware>
    struct Zapper : Interface {
        
        Zapper(TControllerHardware &controllerHardware);
        
        void clock(uint8_t &data) override;
        void out(bool high) override;
        
        void update();
        
    private:
        TControllerHardware &_controllerHardware;
    };
    
    #include "Zapper_s.hpp"
    
} }

#endif /* NESEmu_Controller_Zapper_hpp */
