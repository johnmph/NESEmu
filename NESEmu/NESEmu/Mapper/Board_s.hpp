//
//  Board_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Board_s_hpp
#define NESEmu_Mapper_Board_s_hpp


template <class TMMC, template <class> class ...TChips>
Board<TMMC, TChips...>::Board(std::tuple<TMMC, TChips<TMMC>...> chips) : _chips(std::move(chips)) {
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TInterruptHardware>
void Board<TMMC, TChips...>::clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware) {
    // Does nothing
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus>
void Board<TMMC, TChips...>::cpuReadPerformed(TConnectedBus &connectedBus) {
    // Browse all chips
    //chipCpuReadPerformed<TConnectedBus, TMMC>(connectedBus);
    chipCpuReadPerformed<TConnectedBus, TMMC, TChips<TMMC>...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus>
void Board<TMMC, TChips...>::cpuWritePerformed(TConnectedBus &connectedBus) {
    // Browse all chips
    //chipCpuWritePerformed<TConnectedBus, TMMC>(connectedBus);
    chipCpuWritePerformed<TConnectedBus, TMMC, TChips<TMMC>...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus>
void Board<TMMC, TChips...>::ppuReadPerformed(TConnectedBus &connectedBus) {
    // Browse all chips
    //chipPpuReadPerformed<TConnectedBus, TMMC>(connectedBus);
    chipPpuReadPerformed<TConnectedBus, TMMC, TChips<TMMC>...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus>
void Board<TMMC, TChips...>::ppuWritePerformed(TConnectedBus &connectedBus) {
    // Browse all chips
    //chipPpuWritePerformed<TConnectedBus, TMMC>(connectedBus);
    chipPpuWritePerformed<TConnectedBus, TMMC, TChips<TMMC>...>(connectedBus);
}


template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus>
void Board<TMMC, TChips...>::chipCpuReadPerformed(TConnectedBus &connectedBus) {
}
/*
template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<std::is_same<TFirstChip, TMMC>::value && HasCpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipCpuReadPerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).cpuReadPerformed(connectedBus);//TODO: attention, get avec le type est en C++14 seulement, en C++11 c'est avec un index
    chipCpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<std::is_same<TFirstChip, TMMC>::value && !HasCpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipCpuReadPerformed(TConnectedBus &connectedBus) {
    chipCpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}
*/
template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if</*!std::is_same<TFirstChip, TMMC>::value && */HasCpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipCpuReadPerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).cpuReadPerformed(/*_mmc*/std::get<0>(_chips), connectedBus);//TODO: attention, get avec le type est en C++14 seulement, en C++11 c'est avec un index
    chipCpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if</*!std::is_same<TFirstChip, TMMC>::value && */!HasCpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipCpuReadPerformed(TConnectedBus &connectedBus) {
    chipCpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus>
void Board<TMMC, TChips...>::chipCpuWritePerformed(TConnectedBus &connectedBus) {
}
/*
template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<std::is_same<TFirstChip, TMMC>::value && HasCpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipCpuWritePerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).cpuWritePerformed(connectedBus);
    chipCpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<std::is_same<TFirstChip, TMMC>::value && !HasCpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipCpuWritePerformed(TConnectedBus &connectedBus) {
    chipCpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}
*/
template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if</*!std::is_same<TFirstChip, TMMC>::value && */HasCpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipCpuWritePerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).cpuWritePerformed(/*_mmc*/std::get<0>(_chips), connectedBus);
    chipCpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if</*!std::is_same<TFirstChip, TMMC>::value && */!HasCpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipCpuWritePerformed(TConnectedBus &connectedBus) {
    chipCpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus>
void Board<TMMC, TChips...>::chipPpuReadPerformed(TConnectedBus &connectedBus) {
}
/*
template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<std::is_same<TFirstChip, TMMC>::value && HasPpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipPpuReadPerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).ppuReadPerformed(connectedBus);
    chipPpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<std::is_same<TFirstChip, TMMC>::value && !HasPpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipPpuReadPerformed(TConnectedBus &connectedBus) {
    chipPpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}
*/
template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if</*!std::is_same<TFirstChip, TMMC>::value && */HasPpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipPpuReadPerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).ppuReadPerformed(/*_mmc*/std::get<0>(_chips), connectedBus);
    chipPpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if</*!std::is_same<TFirstChip, TMMC>::value && */!HasPpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipPpuReadPerformed(TConnectedBus &connectedBus) {
    chipPpuReadPerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus>
void Board<TMMC, TChips...>::chipPpuWritePerformed(TConnectedBus &connectedBus) {
}
/*
template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<std::is_same<TFirstChip, TMMC>::value && HasPpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipPpuWritePerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).ppuWritePerformed(connectedBus);
    chipPpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<std::is_same<TFirstChip, TMMC>::value && !HasPpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipPpuWritePerformed(TConnectedBus &connectedBus) {
    chipPpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}
*/
template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if</*!std::is_same<TFirstChip, TMMC>::value && */HasPpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipPpuWritePerformed(TConnectedBus &connectedBus) {
    std::get<TFirstChip>(_chips).ppuWritePerformed(/*_mmc*/std::get<0>(_chips), connectedBus);
    chipPpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

template <class TMMC, template <class> class ...TChips>
template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if</*!std::is_same<TFirstChip, TMMC>::value && */!HasPpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type>
void Board<TMMC, TChips...>::chipPpuWritePerformed(TConnectedBus &connectedBus) {
    chipPpuWritePerformed<TConnectedBus, TOtherChips...>(connectedBus);
}

#endif /* NESEmu_Mapper_Board_s_hpp */
