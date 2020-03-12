//
//  Factory_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Factory_s_hpp
#define NESEmu_Cartridge_Factory_s_hpp


template <Model EModel, class TGraphicHardware, class TLoopManager>
std::unique_ptr<Interface<EModel, TGraphicHardware, TLoopManager>> Factory::createCartridgeFromStream(std::istream &istream) {
    // TODO: browse chaque loader jusqu'a ce qu'un loader prenne en charge cette rom puis decoder et recuperer les infos via ce loader et ensuite créer la cartridge avec ces informations
    
    // Browse loaders
    for (auto &loader : _loaders) {
        // If loader doesn't support cartridge, go to next
        istream.clear();
        istream.seekg(0);
        
        if (!(loader->isCartridgeSupported(istream))) {
            continue;
        }
        
        // Load cartridge data
        istream.clear();
        istream.seekg(0);
        
        auto data = loader->getCartridgeDataFromStream(istream);
        
        // Convert data to cartridge
        return convertDataToCartridge<EModel, TGraphicHardware, TLoopManager>(std::move(data));
    }
    
    // Rom unsupported by all registered loaders
    return nullptr;
}

template <Model EModel, class TGraphicHardware, class TLoopManager>
std::unique_ptr<Interface<EModel, TGraphicHardware, TLoopManager>> Factory::convertDataToCartridge(Loader::Data data) {
    std::unique_ptr<Interface<EModel, TGraphicHardware, TLoopManager>> cartridge;
    
    switch (data.mapperModel) {
        case Mapper::Model::NROM : {
            if (data.mirroringType == Mapper::MirroringType::Horizontal) {
                Mapper::Mapper0::Chip<Mapper::MirroringType::Horizontal> mapper(std::move(data.prgRom), std::move(data.chrRom));
                cartridge = std::make_unique<Cartridge<EModel, TGraphicHardware, TLoopManager, decltype(mapper)>>(std::move(mapper));
            }
            else if (data.mirroringType == Mapper::MirroringType::Vertical) {
                Mapper::Mapper0::Chip<Mapper::MirroringType::Vertical> mapper(std::move(data.prgRom), std::move(data.chrRom));
                cartridge = std::make_unique<Cartridge<EModel, TGraphicHardware, TLoopManager, decltype(mapper)>>(std::move(mapper));
            }
        }
        break;
        
        case Mapper::Model::MMC1 : {
            Mapper::Mapper1::Chip<Mapper::Mapper1::Model::SNROM> mapper(std::move(data.prgRom), std::vector<uint8_t>());
            cartridge = std::make_unique<Cartridge<EModel, TGraphicHardware, TLoopManager, decltype(mapper)>>(std::move(mapper));
        }
        break;
        
        default: {
            // Mapper not supported
            cartridge = std::make_unique<Nothing<EModel, TGraphicHardware, TLoopManager>>();
        }
        break;
    }
    
    return cartridge;
}

#endif /* NESEmu_Cartridge_Factory_s_hpp */
