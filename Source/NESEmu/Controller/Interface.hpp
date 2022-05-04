//
//  Controller.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Controller_Interface_hpp
#define NESEmu_Controller_Interface_hpp

// See https://wiki.nesdev.com/w/index.php/Controller_port_pinout
// See https://wiki.nesdev.com/w/index.php/Input_devices
// See https://wiki.nesdev.com/w/index.php/Controller_detection

#include <cstdint>


namespace NESEmu { namespace Controller {
    
    struct Interface {
        
        virtual ~Interface() = default;
        
        virtual void clock(uint8_t &data) = 0;
        virtual void out(bool high) = 0;
        
    };
    
} }

#endif /* NESEmu_Controller_Interface_hpp */
