//
//  Mapper0_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 24/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper0_s_hpp
#define NESEmu_Cartridge_Mapper0_s_hpp


template <int IPrgRomSizeInKb, int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper0<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::cpuReadPerformed(TConnectedBus &bus) {
}

template <int IPrgRomSizeInKb, int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper0<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::cpuWritePerformed(TConnectedBus &bus) {
    
}

template <int IPrgRomSizeInKb, int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper0<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::ppuReadPerformed(TConnectedBus &bus) {
    
}

template <int IPrgRomSizeInKb, int IPrgRamSizeInKb, MirroringType EMirroring>
template <class TConnectedBus>
void Mapper0<IPrgRomSizeInKb, IPrgRamSizeInKb, EMirroring>::ppuWritePerformed(TConnectedBus &bus) {
    
}

#endif /* NESEmu_Cartridge_Mapper0_s_hpp */
