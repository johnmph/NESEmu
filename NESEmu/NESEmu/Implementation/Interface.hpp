//
//  Interface.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Implementation_Interface_hpp
#define NESEmu_Implementation_Interface_hpp

#include <memory>
#include "../Controller/Interface.hpp"


namespace NESEmu { namespace Implementation {
    
    struct Interface {
        
        virtual ~Interface() = 0;
        
        virtual void powerUp() = 0;
        virtual void reset(bool high) = 0;
        
        virtual void connectController(unsigned int portNumber, std::unique_ptr<Controller::Interface> controller) = 0;
        virtual std::unique_ptr<Controller::Interface> disconnectController(unsigned int portNumber) = 0;
        
        virtual void run() = 0; // TODO: il faudra rajouter un template parameter pour gerer la boucle
    };
    
} }

#endif /* NESEmu_Implementation_Interface_hpp */
