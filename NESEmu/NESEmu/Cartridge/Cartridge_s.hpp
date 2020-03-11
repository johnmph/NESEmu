//
//  Cartridge_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Cartridge_s_hpp
#define NESEmu_Cartridge_Cartridge_s_hpp


template <Model EModel, class TGraphicHardware, class TLoopManager, class TMapper>
Cartridge<EModel, TGraphicHardware, TLoopManager, TMapper>::Cartridge(TMapper const &mapper) : _mapper(mapper) {
}

template <Model EModel, class TGraphicHardware, class TLoopManager, class TMapper>
std::unique_ptr<Implementation::Interface> Cartridge<EModel, TGraphicHardware, TLoopManager, TMapper>::createNesImplementation(TGraphicHardware &graphicHardware, TLoopManager &loopManager) {
    return std::make_unique<Implementation::Nes<EModel, TMapper, TGraphicHardware, TLoopManager>>(_mapper, graphicHardware, loopManager);
}

#endif /* NESEmu_Cartridge_Cartridge_s_hpp */
