//
//  Zapper_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Controller_Zapper_s_hpp
#define NESEmu_Controller_Zapper_s_hpp


template <class TControllerManager>
Zapper<TControllerManager>::Zapper(TControllerManager &controllerManager) : _controllerManager(controllerManager) {
    // TODO: avoir les methodes getTrigger() et getPhotoDiode()
}

template <class TControllerManager>
void Zapper<TControllerManager>::clock(uint8_t &data) {
}

template <class TControllerManager>
void Zapper<TControllerManager>::out(bool high) {
}

template <class TControllerManager>
void Zapper<TControllerManager>::update() {
}

#endif /* NESEmu_Controller_Zapper_s_hpp */
