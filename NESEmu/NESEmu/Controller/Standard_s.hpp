//
//  Standard_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 23/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Controller_Standard_s_hpp
#define NESEmu_Controller_Standard_s_hpp


template <class TControllerHardware>
Standard<TControllerHardware>::Standard(TControllerHardware &controllerHardware) : _controllerHardware(controllerHardware) {
}

template <class TControllerHardware>
void Standard<TControllerHardware>::clock(uint8_t &data) {
    // Update
    update();
    
    // Get shifted bit out, it's for the D0 line, D1-D4 lines are unused here, they are driven high by pull-up resistor
    data = (data & ~0x1) | 0x1F | (_buttonsShiftRegister & 0x1);
    
    // Shift register
    _buttonsShiftRegister >>= 1;
}

template <class TControllerHardware>
void Standard<TControllerHardware>::out(bool high) {
    // When true, it continuously update the shift register
    _needToUpdate = high;
    
    // Update
    update();
}

template <class TControllerHardware>
void Standard<TControllerHardware>::update() {  // TODO: soit on l'appelle a chaque clock du cpu, ou bien pour optimiser (si c'est ok avec tous les controllers) on l'appelle dans clock et out (au debut de clock et a la fin de out)
    // If no need to update, exit
    if (_needToUpdate == false) {
        return;
    }
    
    // Update shift register with controller status
    _buttonsShiftRegister = _controllerHardware.getButtonA();
    _buttonsShiftRegister |= (_controllerHardware.getButtonB()) << 1;
    _buttonsShiftRegister |= (_controllerHardware.getButtonSelect()) << 2;
    _buttonsShiftRegister |= (_controllerHardware.getButtonStart()) << 3;
    _buttonsShiftRegister |= (_controllerHardware.getDirectionalUp()) << 4;
    _buttonsShiftRegister |= (_controllerHardware.getDirectionalDown()) << 5;
    _buttonsShiftRegister |= (_controllerHardware.getDirectionalLeft()) << 6;
    _buttonsShiftRegister |= (_controllerHardware.getDirectionalRight()) << 7;
    
    // TODO: Inverted ?
    _buttonsShiftRegister = ~_buttonsShiftRegister;
}

#endif /* NESEmu_Controller_Standard_s_hpp */
