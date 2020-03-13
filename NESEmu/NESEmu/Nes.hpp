//
//  Nes.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 10/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Nes_hpp
#define NESEmu_Nes_hpp

#include "Implementation/Nes.hpp"
#include "Cartridge/Nothing.hpp"

//TODO: voir pour noexcept sur les methodes
namespace NESEmu {
    
    template <Model EModel, class TGraphicHardware, class TLoopManager>//TODO: trouver un autre nom que TLoopManager
    struct Nes {
        
        Nes(TGraphicHardware &graphicHardware, TLoopManager &loopManager);
        
        //template <class TCartridge>
        //void insertCartridge(TCartridge &cartridge);
        void insertCartridge(std::unique_ptr<Cartridge::Interface<EModel, TGraphicHardware, TLoopManager>> cartridge);
        // TODO: un remove cartridge aussi ?
        
        void powerUp();
        
        void reset(bool high);  // TODO: a la place d'avoir ca, peut etre avoir un template parameter TResetHardware qui aura une methode qui retourne un bool pour le reset (pour mieux decoupler ?), a voir (ca simule le fait d'avoir le bouton reset sur la Nes), ou peut etre laisser ainsi
        
        void connectController(unsigned int portNumber, std::unique_ptr<Controller::Interface> controller);
        void disconnectController(unsigned int portNumber);
        
        void run();
        
        
    private:
        // Internals
        TGraphicHardware &_graphicHardware;
        TLoopManager &_loopManager;
        std::unique_ptr<Cartridge::Interface<EModel, TGraphicHardware, TLoopManager>> _cartridge;
        std::unique_ptr<Implementation::Interface> _nesImplementation;
    };
    
    #include "Nes_s.hpp"
    
}

#endif /* NESEmu_Nes_hpp */
