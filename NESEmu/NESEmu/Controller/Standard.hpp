//
//  Standard.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 23/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Controller_Standard_hpp
#define NESEmu_Controller_Standard_hpp

// See https://wiki.nesdev.com/w/index.php/Standard_controller

#include "Interface.hpp"


namespace NESEmu { namespace Controller {
    
    template <class TControllerHardware>
    struct Standard : Interface {
        
        Standard(TControllerHardware &controllerHardware);
        
        void clock(uint8_t &data) override;
        void out(bool high) override;
        
        void update();
        
    private:
        TControllerHardware &_controllerHardware;
        uint8_t _buttonsShiftRegister;
        bool _needToUpdate;
    };
    
    #include "Standard_s.hpp"
    
} }

#endif /* NESEmu_Controller_Standard_hpp */
