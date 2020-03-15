//
//  Factory.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Factory_hpp
#define NESEmu_Cartridge_Factory_hpp

#include <memory>
#include <vector>
#include "Loader/Interface.hpp"
#include "Cartridge.hpp"
#include "Nothing.hpp"
#include "../Mapper/Board.hpp"
#include "../Mapper/Mapper0.hpp"
#include "../Mapper/Mapper1.hpp"
#include "../Mapper/Mapper2.hpp"
// TODO: les mettre tous


namespace NESEmu { namespace Cartridge {
    
    struct Factory {
        
        void registerLoader(std::shared_ptr<Loader::Interface> loader, int priority = 0);   // TODO: créer une autre methode sans le parametre loader mais avec un parametre template pour le créer ici (mais il faut un enable_if pour checker que le type derive bien de Loader::Interface) : a voir car par exemple le loader avec database et checksum devra avoir un constructor parametrisé avec la base de données et donc on ne peut pas le créer ici, il devra etre créé a l'exterieur et passé ici comme on le fait maintenant
        
        template <Model EModel, class TGraphicHardware, class TLoopManager>
        std::unique_ptr<Interface<EModel, TGraphicHardware, TLoopManager>> createCartridgeFromStream(std::istream &istream);
        
    private:
        
        template <Model EModel, class TGraphicHardware, class TLoopManager>
        std::unique_ptr<Interface<EModel, TGraphicHardware, TLoopManager>> convertDataToCartridge(Loader::Data data);
        
        std::vector<std::shared_ptr<Loader::Interface>> _loaders;
    };
    
    #include "Factory_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Factory_hpp */
