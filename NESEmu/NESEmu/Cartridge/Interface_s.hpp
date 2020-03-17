//
//  Interface_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Interface_s_hpp
#define NESEmu_Cartridge_Interface_s_hpp


template <class TCpuHardwareInterface, class TPpuHardwareInterface>
Interface<TCpuHardwareInterface, TPpuHardwareInterface>::~Interface() {
}

template <class TCpuHardwareInterface, class TPpuHardwareInterface>
void Interface<TCpuHardwareInterface, TPpuHardwareInterface>::clock(TCpuHardwareInterface &cpuHardwareInterface, TPpuHardwareInterface &ppuHardwareInterface) {
}

#endif /* NESEmu_Cartridge_Interface_s_hpp */
