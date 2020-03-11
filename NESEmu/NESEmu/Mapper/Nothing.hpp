//
//  Nothing.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Nothing_hpp
#define NESEmu_Mapper_Nothing_hpp


namespace NESEmu { namespace Mapper {
    
    struct NoMapper {
        
        template <class TConnectedBus, class TInterruptHardware>
        void clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware);
        
        // Cpu memory bus
        template <class TConnectedBus>
        void cpuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void cpuWritePerformed(TConnectedBus &connectedBus);
        
        // Ppu memory bus
        template <class TConnectedBus>
        void ppuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void ppuWritePerformed(TConnectedBus &connectedBus);
    };
    
    #include "Nothing_s.hpp"
    
} }

#endif /* NESEmu_Mapper_Nothing_hpp */
