//
//  Board.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Board_hpp
#define NESEmu_Mapper_Board_hpp

#include <tuple>
#include "Common.hpp"


namespace NESEmu { namespace Mapper {
    
    /*
     
     IPrgRomSizeInKb (ou en bloc de 16kb ??, a voir)
     IPrgRamSizeInKb (est ce que la SRAM a toujours la meme taille ? si oui juste un bool a la place d'un unsigned int pour dire si present ou pas : oui il peut avoir plusieurs tailles (de plus petite a egal a plus grande que l'address range), voir quand taille petite et adresse depasse, est ce que mirror address ou open bus ?)
     IChrRomSizeInKb (ou en bloc de 8kb ? a voir)
     Chr Ram est ce qu'il peut y avoir de la Chr-Ram en plus de la chr rom ? si oui avoir les 2 parametres, sinon avoir juste un parametre IChrMemorySizeInKb et avoir un bool pour savoir si ram ou non (rom)
     MirroringType
     IRQ : voir si tous la gestion des IRQ de tous les mappers est la meme, si oui avoir un bool pour savoir si gere le IRQ ou non sinon ne pas avoir de parametre template et gerer ca dans chaque mapper qui l'utilise : non les mappers implemente l'irq differement, le MMC5 par exemple attends 3 read a la meme address dans un nametable alors que le MMC3 se base sur les edges de PPU A12
     
     TODO: surement couper en differentes parties (policy) qui emule les chips ROM, RAM, ... ainsi on peut composer un mapper avec ces chips (comme Zelda 1 qui est un MMC1 avec 8ko de chr-ram et Zelda 2 qui est aussi un MMC1 mais avec 128ko de chr-rom)
     
     TODO: d'apres les tests, les profiling et les comparaisons d'asm, ce code avec les policy, tuple, ... revient bien au meme qu'en direct, donc utiliser ce systeme car plus de flexibilité sans perte de performances
     
     Audio : a voir
     Bus Conflit : a voir
     
     */
    
    // Traits to know if a class has a cpuReadPerformed method
    template <class TConnectedBus, typename T, bool = true>
    struct HasCpuReadPerformed : std::false_type {};
    
    template <class TConnectedBus, typename T>
    struct HasCpuReadPerformed<TConnectedBus, T, (sizeof(decltype(&T::template cpuReadPerformed<TConnectedBus>)) >= 0)> : std::true_type {};
    
    // Traits to know if a class has a cpuWritePerformed method
    template <class TConnectedBus, typename T, bool = true>
    struct HasCpuWritePerformed : std::false_type {};
    
    template <class TConnectedBus, typename T>
    struct HasCpuWritePerformed<TConnectedBus, T, (sizeof(decltype(&T::template cpuWritePerformed<TConnectedBus>)) >= 0)> : std::true_type {};
    
    // Traits to know if a class has a ppuReadPerformed method
    template <class TConnectedBus, typename T, bool = true>
    struct HasPpuReadPerformed : std::false_type {};
    
    template <class TConnectedBus, typename T>
    struct HasPpuReadPerformed<TConnectedBus, T, (sizeof(decltype(&T::template ppuReadPerformed<TConnectedBus>)) >= 0)> : std::true_type {};
    
    // Traits to know if a class has a ppuWritePerformed method
    template <class TConnectedBus, typename T, bool = true>
    struct HasPpuWritePerformed : std::false_type {};
    
    template <class TConnectedBus, typename T>
    struct HasPpuWritePerformed<TConnectedBus, T, (sizeof(decltype(&T::template ppuWritePerformed<TConnectedBus>)) >= 0)> : std::true_type {};
    
    
    template <class TMMC, template <class> class ...TChips>
    struct Board {
        
        Board(std::tuple<TMMC, TChips<TMMC>...> chips);
        
        template <class TConnectedBus, class TInterruptHardware>
        void clock(TConnectedBus &connectedBus, TInterruptHardware &interruptHardware);   // TODO: voir si moyen de desactiver l'appel au compile time (cad que sur les mappers ou on a pas besoin de clock il n'y aurait pas d'appels de clock), seulement sur les mappers qui en ont besoin, ainsi on evite des pertes de performances (ou si l'optimisation suffit pour ne pas appeler la methode si elle est vide) !
        
        // Cpu memory bus
        template <class TConnectedBus>
        void cpuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void cpuWritePerformed(TConnectedBus &connectedBus);
        
        // Ppu memory bus
        template <class TConnectedBus>
        void ppuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void ppuWritePerformed(TConnectedBus &connectedBus);
        
    private:
        
        template <class TConnectedBus>
        void chipCpuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<HasCpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type = 0>
        void chipCpuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<!HasCpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type = 0>
        void chipCpuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void chipCpuWritePerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<HasCpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type = 0>
        void chipCpuWritePerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<!HasCpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type = 0>
        void chipCpuWritePerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void chipPpuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<HasPpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type = 0>
        void chipPpuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<!HasPpuReadPerformed<TConnectedBus, TFirstChip>::value, int>::type = 0>
        void chipPpuReadPerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus>
        void chipPpuWritePerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<HasPpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type = 0>
        void chipPpuWritePerformed(TConnectedBus &connectedBus);
        
        template <class TConnectedBus, class TFirstChip, class ...TOtherChips, typename std::enable_if<!HasPpuWritePerformed<TConnectedBus, TFirstChip>::value, int>::type = 0>
        void chipPpuWritePerformed(TConnectedBus &connectedBus);
        
        
        std::tuple<TMMC, TChips<TMMC>...> _chips;
    };
    
    #include "Board_s.hpp"
    
} }

#endif /* NESEmu_Mapper_Board_hpp */
