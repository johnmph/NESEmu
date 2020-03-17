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
#include "Nothing.hpp"
#include "Mapper0.hpp"
#include "Mapper1.hpp"
#include "Mapper2.hpp"
#include "Mapper3.hpp"
#include "Mapper4.hpp"
#include "Mapper7.hpp"
// TODO: les mettre tous


namespace NESEmu { namespace Cartridge {
    
    struct Factory {
        
        void registerLoader(std::shared_ptr<Loader::Interface> loader, int priority = 0);   // TODO: créer une autre methode sans le parametre loader mais avec un parametre template pour le créer ici (mais il faut un enable_if pour checker que le type derive bien de Loader::Interface) : a voir car par exemple le loader avec database et checksum devra avoir un constructor parametrisé avec la base de données et donc on ne peut pas le créer ici, il devra etre créé a l'exterieur et passé ici comme on le fait maintenant
        
        template <class TCpuHardwareInterface, class TPpuHardwareInterface>
        std::unique_ptr<Interface<TCpuHardwareInterface, TPpuHardwareInterface>> createCartridgeFromStream(std::istream &istream);
        
    private:
        
        template <class TCpuHardwareInterface, class TPpuHardwareInterface>
        std::unique_ptr<Interface<TCpuHardwareInterface, TPpuHardwareInterface>> convertDataToCartridge(Loader::Data data);
        
        std::vector<std::shared_ptr<Loader::Interface>> _loaders;
    };
    
    #include "Factory_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Factory_hpp */
