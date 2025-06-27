//
//  Model.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Model_hpp
#define NESEmu_Cartridge_Model_hpp


namespace NESEmu { namespace Cartridge {
    
    enum class Model {
        NROM,
        MMC1,
        UxROM,
        CNROM,
        MMC3,
        MMC5,
        FFE,
        AxROM,
        M008,   // TODO: voir pour le nom
        MMC2,
        MMC4,
        M072 = 72,
        M092 = 92,
        //TODO: a continuer
    };
    
} }

#endif /* NESEmu_Cartridge_Model_hpp */
