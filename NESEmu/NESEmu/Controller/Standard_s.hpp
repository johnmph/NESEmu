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
    //update();//TODO: normalement pas besoin car ca se fait dans out mais voir si ok et si moyen d'optimiser, il faut des roms tests pour les controls
    
    // Get shifted bit out, it's for the D0 line, D1-D4 lines are unused here, they are driven high by pull-up resistor
    data = (data & ~0x1) | 0x1E | (_buttonsShiftRegister & 0x1);
    
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
void Standard<TControllerHardware>::update() {
    // If no need to update, exit
    if (!_needToUpdate) {
        return;
    }
    
    // Update shift register with controller status
    _controllerHardware.update();
    
    _buttonsShiftRegister = _controllerHardware.getButtonA();
    _buttonsShiftRegister |= (_controllerHardware.getButtonB()) << 1;
    _buttonsShiftRegister |= (_controllerHardware.getButtonSelect()) << 2;
    _buttonsShiftRegister |= (_controllerHardware.getButtonStart()) << 3;
    _buttonsShiftRegister |= (_controllerHardware.getDirectionalUp()) << 4;
    _buttonsShiftRegister |= (_controllerHardware.getDirectionalDown()) << 5;
    _buttonsShiftRegister |= (_controllerHardware.getDirectionalLeft()) << 6;
    _buttonsShiftRegister |= (_controllerHardware.getDirectionalRight()) << 7;
    
    // Buttons from controller hardware returns true if pressed but in NES controller a pressed button is connected to ground so it returns false
    // We simplify code by inverting all buttons here
    _buttonsShiftRegister = ~_buttonsShiftRegister;
}

#endif /* NESEmu_Controller_Standard_s_hpp */
