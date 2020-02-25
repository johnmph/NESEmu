//
//  Common.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Common_hpp
#define NESEmu_Cartridge_Common_hpp

#include <cstdint>


namespace NESEmu { namespace Cartridge {
    
    // See https://wiki.nesdev.com/w/index.php/Mirroring
    enum class MirroringType {
        Horizontal,
        Vertical,
        SingleScreen,
        FourScreen
    };
    
    template <MirroringType EMirroring>
    uint16_t getMirroredAddress(uint16_t address);
    
} }

#endif /* NESEmu_Cartridge_Common_hpp */
