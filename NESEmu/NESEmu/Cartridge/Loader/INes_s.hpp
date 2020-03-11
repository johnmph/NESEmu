//
//  INes_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Loader_INes_s_hpp
#define NESEmu_Cartridge_Loader_INes_s_hpp


template <Model EModel, class TGraphicHardware, class TLoopManager>
std::unique_ptr<::NESEmu::Cartridge::Interface<EModel, TGraphicHardware, TLoopManager>> INes<EModel, TGraphicHardware, TLoopManager>::createCartridgeFromStream(std::istream &istream) {
    
    Mapper::Mapper0<32, 8, Mapper::MirroringType::Vertical> mapper0(istream);//TODO: changer
    
    return std::make_unique<Cartridge<EModel, TGraphicHardware, TLoopManager, decltype(mapper0)>>(mapper0);
}

#endif /* NESEmu_Cartridge_Loader_INes_s_hpp */
