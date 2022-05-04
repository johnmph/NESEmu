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
}

template <class TControllerManager>
void Zapper<TControllerManager>::clock(uint8_t &data) {
    // D1-D2 and D5-D7 lines are unused here TODO: voir si open bus ou si pull-up ou pull-down resistor !!! ici open bus pour l'instant
    data &= 0xE6;
    
    // No serial data
    data |= 0x1;
    
    // Set light sense data (inverted)
    data |= _controllerManager.getLightSense() << 3;
    
    // Set trigger data
    data |= (!_controllerManager.getTrigger()) << 4;
}

template <class TControllerManager>
void Zapper<TControllerManager>::out(bool high) {
}

#endif /* NESEmu_Controller_Zapper_s_hpp */
