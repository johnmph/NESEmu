//
//  Factory.cpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#include "Factory.hpp"


namespace NESEmu { namespace Cartridge {
    
    void Factory::registerLoader(std::shared_ptr<Loader::Interface> loader, int priority) {
        _loaders.insert(_loaders.begin() + priority, loader);
    }

} }
