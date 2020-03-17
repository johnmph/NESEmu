//
//  Factory_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Factory_s_hpp
#define NESEmu_Cartridge_Factory_s_hpp


template <class TCpuHardwareInterface, class TPpuHardwareInterface>
std::unique_ptr<Interface<TCpuHardwareInterface, TPpuHardwareInterface>> Factory::createCartridgeFromStream(std::istream &istream) {
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
        return convertDataToCartridge<TCpuHardwareInterface, TPpuHardwareInterface>(std::move(data));
    }
    
    // Rom unsupported by all registered loaders
    return nullptr;
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
std::unique_ptr<Interface<TCpuHardwareInterface, TPpuHardwareInterface>> Factory::convertDataToCartridge(Loader::Data data) {
    std::unique_ptr<Interface<TCpuHardwareInterface, TPpuHardwareInterface>> cartridge;
    
    switch (data.mapperModel) {
        case Model::NROM : {
            cartridge = std::make_unique<Mapper0::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), std::vector<uint8_t>(8 * 1024), std::move(data.chrRom), data.chrRamSize, data.mirroringType);
        }
        break;
        
        case Model::MMC1 : {
            cartridge = std::make_unique<Mapper1::Chip<TCpuHardwareInterface, TPpuHardwareInterface, Mapper1::Model::SNROM>>(std::move(data.prgRom), std::vector<uint8_t>(8 * 1024), std::move(data.chrRom), data.chrRamSize);
        }
        break;
        
        default: {
            // Mapper not supported
            cartridge = std::make_unique<Nothing<TCpuHardwareInterface, TPpuHardwareInterface>>();
        }
        break;
    }
    
    return cartridge;
}

#endif /* NESEmu_Cartridge_Factory_s_hpp */
