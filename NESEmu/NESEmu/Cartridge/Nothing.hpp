//
//  Nothing.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 10/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Nothing_hpp
#define NESEmu_Cartridge_Nothing_hpp

#include "Interface.hpp"


namespace NESEmu { namespace Cartridge {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Nothing : Interface<TCpuHardwareInterface, TPpuHardwareInterface> {
        
        // Cpu memory bus
        void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        
        // Ppu memory bus
        void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        
    };
    
    #include "Nothing_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Nothing_hpp */
