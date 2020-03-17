//
//  Interface.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Interface_hpp
#define NESEmu_Cartridge_Interface_hpp

#include <cstdint>


namespace NESEmu { namespace Cartridge {
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Interface {
        
        virtual ~Interface() = 0;
        
        virtual void clock(TCpuHardwareInterface &cpuHardwareInterface, TPpuHardwareInterface &ppuHardwareInterface);   // TODO: voir si moyen de desactiver l'appel au compile time (cad que sur les mappers ou on a pas besoin de clock il n'y aurait pas d'appels de clock), seulement sur les mappers qui en ont besoin, ainsi on evite des pertes de performances (ou si l'optimisation suffit pour ne pas appeler la methode si elle est vide) !
        
        // Cpu memory bus
        virtual void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) = 0;
        virtual void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) = 0;
        
        // Ppu memory bus
        virtual void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) = 0;
        virtual void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) = 0;
        
    };
    
    #include "Interface_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Interface_hpp */
