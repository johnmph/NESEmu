//
//  DmcChannel.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 22/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Apu_Internal_Dmc_hpp
#define NESEmu_Apu_Internal_Dmc_hpp

// See https://wiki.nesdev.com/w/index.php/APU_DMC

#include <cstdint>


namespace NESEmu { namespace Apu {
    
    struct DmcChannel {
        
        void powerUp();
        void reset();
        
        void clock();
        
        // Output
        uint8_t getOutput() const;
        
    };
    
} }

#endif /* NESEmu_Apu_Internal_Dmc_hpp */
