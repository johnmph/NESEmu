//
//  Controller.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Controller_Interface_hpp
#define NESEmu_Controller_Interface_hpp

// TODO : https://wiki.nesdev.com/w/index.php/Standard_controller
// TODO : https://wiki.nesdev.com/w/index.php/Controller_detection

#include <cstdint>


namespace NESEmu { namespace Controller {
    
    struct Interface {
        
        virtual uint8_t read() = 0;
        virtual void write(bool data) = 0;
        
    };
    
} }

#endif /* NESEmu_Controller_Interface_hpp */
