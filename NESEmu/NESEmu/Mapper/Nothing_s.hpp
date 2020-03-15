//
//  Nothing_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 11/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Nothing_s_hpp
#define NESEmu_Mapper_Nothing_s_hpp


template <class TConnectedBus, class TInterruptHardware>
void Chip::clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware) {
}

template <class TConnectedBus>
void Chip::cpuReadPerformed(TConnectedBus &connectedBus) {
}

template <class TConnectedBus>
void Chip::cpuWritePerformed(TConnectedBus &connectedBus) {
}

template <class TConnectedBus>
void Chip::ppuReadPerformed(TConnectedBus &connectedBus) {
}

template <class TConnectedBus>
void Chip::ppuWritePerformed(TConnectedBus &connectedBus) {
}

#endif /* NESEmu_Mapper_Nothing_s_hpp */
