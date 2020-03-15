//
//  Nothing_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Nothing_s_hpp
#define NESEmu_Cartridge_Nothing_s_hpp


template <Model EModel, class TGraphicHardware, class TLoopManager>
Nothing<EModel, TGraphicHardware, TLoopManager>::Nothing() : Cartridge<EModel, TGraphicHardware, TLoopManager, Mapper::Nothing::Chip>(Mapper::Nothing::Chip()) {
}

#endif /* NESEmu_Cartridge_Nothing_s_hpp */
