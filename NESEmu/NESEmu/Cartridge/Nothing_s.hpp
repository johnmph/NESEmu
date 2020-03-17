//
//  Nothing_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Nothing_s_hpp
#define NESEmu_Cartridge_Nothing_s_hpp


template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Nothing<TCpuHardwareInterface, TPpuHardwareInterface>::cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) {
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Nothing<TCpuHardwareInterface, TPpuHardwareInterface>::cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) {
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Nothing<TCpuHardwareInterface, TPpuHardwareInterface>::ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) {
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Nothing<TCpuHardwareInterface, TPpuHardwareInterface>::ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) {
}

#endif /* NESEmu_Cartridge_Nothing_s_hpp */
