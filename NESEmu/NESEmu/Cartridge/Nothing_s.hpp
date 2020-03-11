//
//  Nothing_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 10/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Nothing_s_hpp
#define NESEmu_Cartridge_Nothing_s_hpp


template <Model EModel, class TGraphicHardware, class TLoopManager>
std::unique_ptr<Implementation::Interface> Nothing<EModel, TGraphicHardware, TLoopManager>::createNesImplementation(TGraphicHardware &graphicHardware, TLoopManager &loopManager) {
    Mapper::NoMapper noMapper;
    
    return std::make_unique<Implementation::Nes<EModel, Mapper::NoMapper, TGraphicHardware, TLoopManager>>(noMapper, graphicHardware, loopManager);
}

#endif /* NESEmu_Cartridge_Nothing_s_hpp */
