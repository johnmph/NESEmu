//
//  Nes_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 10/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Nes_s_hpp
#define NESEmu_Nes_s_hpp


template <Model EModel, class TGraphicHardware, class TLoopManager>
Nes<EModel, TGraphicHardware, TLoopManager>::Nes(TGraphicHardware &graphicHardware, TLoopManager &loopManager) : _graphicHardware(graphicHardware), _loopManager(loopManager) {
    // Start with No cartridge
    Cartridge::Nothing<EModel, TGraphicHardware, TLoopManager> cartridge;
    _nesImplementation = std::move(cartridge.createNesImplementation(_graphicHardware, _loopManager));
}

template <Model EModel, class TGraphicHardware, class TLoopManager>
template <class TCartridge>
void Nes<EModel, TGraphicHardware, TLoopManager>::insertCartridge(TCartridge &cartridge) {//TODO: const &cartridge ?
    // Get controllers from current nes implementation
    std::unique_ptr<Controller::Interface> controllers[2];
    
    controllers[0] = _nesImplementation->disconnectController(0);
    controllers[1] = _nesImplementation->disconnectController(1);
    
    // Get a nes implementation from cartridge and set nes implementation with
    _nesImplementation = std::move(cartridge.createNesImplementation(_graphicHardware, _loopManager));
    
    // Reconnect controllers to new implementation
    _nesImplementation->connectController(0, std::move(controllers[0]));
    _nesImplementation->connectController(1, std::move(controllers[1]));
}

template <Model EModel, class TGraphicHardware, class TLoopManager>
void Nes<EModel, TGraphicHardware, TLoopManager>::powerUp() {
    _nesImplementation->powerUp();
}

template <Model EModel, class TGraphicHardware, class TLoopManager>
void Nes<EModel, TGraphicHardware, TLoopManager>::reset(bool high) {
    _nesImplementation->reset(high);
}

template <Model EModel, class TGraphicHardware, class TLoopManager>
void Nes<EModel, TGraphicHardware, TLoopManager>::connectController(unsigned int portNumber, std::unique_ptr<Controller::Interface> controller) {
    _nesImplementation->connectController(portNumber, std::move(controller));
}

template <Model EModel, class TGraphicHardware, class TLoopManager>
void Nes<EModel, TGraphicHardware, TLoopManager>::disconnectController(unsigned int portNumber) {
    _nesImplementation->disconnectController(portNumber);
}

template <Model EModel, class TGraphicHardware, class TLoopManager>
void Nes<EModel, TGraphicHardware, TLoopManager>::run() {
    _nesImplementation->run();
}

#endif /* NESEmu_Nes_s_hpp */
