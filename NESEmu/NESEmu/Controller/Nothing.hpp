//
//  Nothing.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 23/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Controller_Nothing_hpp
#define NESEmu_Controller_Nothing_hpp

#include "Interface.hpp"


namespace NESEmu { namespace Controller {
    
    struct Nothing : Interface {
        
        uint8_t read() override;
        void write(bool data) override;
        
        void update();
    };
    
} }

#endif /* NESEmu_Controller_Nothing_hpp */
