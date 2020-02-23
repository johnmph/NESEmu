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
uint8_t Standard<TControllerHardware>::read() {
    // Get bit out
    bool bitOut = _buttonsShiftRegister & 0x1;      // TODO: si plus de bits, ca doit retourner 1 car ca retourne 0 mais c'est inversé, mais c'est inversé ou ??? dans le controller ou apres le port au niveau du cpu ??? car dans ce cas les bits des boutons aussi seront inversés !!! : normalement c'est au niveau de la nes pres du CPU que c'est inversé donc retourner ici les valeurs non inversées
    
    // Shift register
    _buttonsShiftRegister >>= 1;
    
    // Return current button status
    return bitOut;
}

template <class TControllerHardware>
void Standard<TControllerHardware>::write(bool data) {
    // When true, it continuously update the shift register
    _needToUpdate = data;
}

template <class TControllerHardware>
void Standard<TControllerHardware>::update() {  // TODO: soit on l'appelle a chaque clock de la nes, ou du cpu, ou bien pour optimiser (si c'est ok avec tous les controllers) on l'appelle dans read et write (au debut de read et a la fin de write)
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
