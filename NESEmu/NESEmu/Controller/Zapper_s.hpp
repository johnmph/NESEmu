//
//  Zapper_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Controller_Zapper_s_hpp
#define NESEmu_Controller_Zapper_s_hpp


template <class TControllerHardware>
Zapper<TControllerHardware>::Zapper(TControllerHardware &controllerHardware) : _controllerHardware(controllerHardware) {
    // TODO: avoir les methodes getTrigger() et getPhotoDiode()
}

template <class TControllerHardware>
void Zapper<TControllerHardware>::clock(uint8_t &data) {
}

template <class TControllerHardware>
void Zapper<TControllerHardware>::out(bool high) {
}

template <class TControllerHardware>
void Zapper<TControllerHardware>::update() {
}

#endif /* NESEmu_Controller_Zapper_s_hpp */
