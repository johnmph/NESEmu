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
void Factory<TCpuHardwareInterface, TPpuHardwareInterface>::registerLoader(std::shared_ptr<Loader::Interface> loader, int priority) {
    _loaders.insert(_loaders.begin() + priority, loader);
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
std::unique_ptr<Interface<TCpuHardwareInterface, TPpuHardwareInterface>> Factory<TCpuHardwareInterface, TPpuHardwareInterface>::createCartridgeFromStream(std::istream &istream) {
    assert(istream.good());
    
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
        return convertDataToCartridge(std::move(data));
    }
    
    // Rom unsupported by all registered loaders
    return std::make_unique<Nothing<TCpuHardwareInterface, TPpuHardwareInterface>>();
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
std::unique_ptr<Interface<TCpuHardwareInterface, TPpuHardwareInterface>> Factory<TCpuHardwareInterface, TPpuHardwareInterface>::convertDataToCartridge(Loader::Data data) {
    std::unique_ptr<Interface<TCpuHardwareInterface, TPpuHardwareInterface>> cartridge;
    
    switch (data.mapperModel) {
        case Model::NROM : {
            cartridge = std::make_unique<Mapper0::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), data.prgRamSize, std::move(data.chrRom), data.chrRamSize, data.mirroringType);
        }
        break;
        
        case Model::MMC1 : {
            cartridge = std::make_unique<Mapper1::Chip<TCpuHardwareInterface, TPpuHardwareInterface, Mapper1::Model::SNROM>>(std::move(data.prgRom), data.prgRamSize, std::move(data.chrRom), data.chrRamSize);
        }
        break;
        
        case Model::UxROM : {
            cartridge = std::make_unique<Mapper2::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), data.prgRamSize, std::move(data.chrRom), data.chrRamSize, data.mirroringType);
        }
        break;
        
        case Model::CNROM : {
            cartridge = std::make_unique<Mapper3::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), data.prgRamSize, std::move(data.chrRom), data.mirroringType);
        }
        break;
        
        case Model::MMC3 : {
            std::size_t prgRamSize = (data.prgRamSize > 0) ? data.prgRamSize : (8 * 1024);//TODO: si je met pas 8192 mario 3 foire car il a de la prg ram alors que le header dit qu'il en a pas, a voir !!!
            
            cartridge = std::make_unique<Mapper4::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), prgRamSize, std::move(data.chrRom), data.mirroringType);
        }
        break;
        /*
        case Model::MMC5 : {
            
        }
        break;
        
        case Model::FFE : {
            
        }
        break;*/
        
        case Model::AxROM : {
            cartridge = std::make_unique<Mapper7::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), data.chrRamSize);
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
