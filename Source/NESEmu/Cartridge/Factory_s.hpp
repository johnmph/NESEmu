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
            cartridge = std::make_unique<Mapper4::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), data.prgRamSize, std::move(data.chrRom), data.chrRamSize, data.mirroringType);
        }
        break;
        
        case Model::MMC5 : {
            cartridge = std::make_unique<Mapper5::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), /*data.prgRamSize*/64*1024, 1, std::move(data.chrRom), data.chrRamSize);//TODO: a changer mais comment savoir la taille dans ines (mais ce n'est pas a gerer ici (il suffit de mettre un member prgRamChipCount en + dans data et de setter ces informations dans INes))
        }
        break;
        /*
        case Model::FFE : {
            
        }
        break;*/
        
        case Model::AxROM : {
            cartridge = std::make_unique<Mapper7::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), data.chrRamSize);
        }
        break;
        
        case Model::MMC2 : {
            cartridge = std::make_unique<Mapper9::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), data.prgRamSize, std::move(data.chrRom));
        }
        break;
        
        case Model::MMC4 : {
            cartridge = std::make_unique<Mapper10::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), std::move(data.chrRom));
        }
        break;

        case Model::M072 : {
            cartridge = std::make_unique<Mapper72::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), std::move(data.chrRom), data.mirroringType);
        }
        break;
        
        case Model::M092 : {
            cartridge = std::make_unique<Mapper92::Chip<TCpuHardwareInterface, TPpuHardwareInterface>>(std::move(data.prgRom), std::move(data.chrRom), data.mirroringType);
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
