//
//  Mapper0n.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/03/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Mapper_Mapper0n_hpp
#define NESEmu_Mapper_Mapper0n_hpp

#include <vector>
#include <tuple>
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/NROM
// TODO: par apres, factoriser le code commun des mappers
namespace NESEmu { namespace Mapper { namespace Mapper0n {
    
    /*
     
     IPrgRomSizeInKb (ou en bloc de 16kb ??, a voir)
     IPrgRamSizeInKb (est ce que la SRAM a toujours la meme taille ? si oui juste un bool a la place d'un unsigned int pour dire si present ou pas : oui il peut avoir plusieurs tailles (de plus petite a egal a plus grande que l'address range), voir quand taille petite et adresse depasse, est ce que mirror address ou open bus ?)
     IChrRomSizeInKb (ou en bloc de 8kb ? a voir)
     Chr Ram est ce qu'il peut y avoir de la Chr-Ram en plus de la chr rom ? si oui avoir les 2 parametres, sinon avoir juste un parametre IChrMemorySizeInKb et avoir un bool pour savoir si ram ou non (rom)
     MirroringType
     IRQ : voir si tous la gestion des IRQ de tous les mappers est la meme, si oui avoir un bool pour savoir si gere le IRQ ou non sinon ne pas avoir de parametre template et gerer ca dans chaque mapper qui l'utilise : non les mappers implemente l'irq differement, le MMC5 par exemple attends 3 read a la meme address dans un nametable alors que le MMC3 se base sur les edges de PPU A12
     
     TODO: surement couper en differentes parties (policy) qui emule les chips ROM, RAM, ... ainsi on peut composer un mapper avec ces chips (comme Zelda 1 qui est un MMC1 avec 8ko de chr-ram et Zelda 2 qui est aussi un MMC1 mais avec 128ko de chr-rom)
     
     Audio : a voir
     Bus Conflit : a voir
     
     */
    
    struct PrgRom {
        
        PrgRom(std::vector<uint8_t> data) : _data(std::move(data)), _addressMask(_data.size() - 1) {//TODO: dans un cpp
        }
        
        // Cpu memory bus
        template <class TConnectedBus>
        void cpuReadPerformed(TConnectedBus &connectedBus, uint16_t address) const;
        /*
        template <class TConnectedBus>
        void cpuWritePerformed(TConnectedBus &connectedBus, uint16_t address);*/
        
    private:
        std::vector<uint8_t> const _data;
        uint16_t const _addressMask;
    };
    
    struct PrgRam {
        
        PrgRam(std::vector<uint8_t> data) : _data(std::move(data)), _addressMask(_data.size() - 1) {//TODO: dans un cpp
        }
        
        // Cpu memory bus
        template <class TConnectedBus>
        void cpuReadPerformed(TConnectedBus &connectedBus, uint16_t address) const;
        
        template <class TConnectedBus>
        void cpuWritePerformed(TConnectedBus &connectedBus, uint16_t address);
        
    private:
        std::vector<uint8_t> _data;
        uint16_t const _addressMask;
    };
    
    struct ChrRom {
        
        ChrRom(std::vector<uint8_t> data) : _data(std::move(data)), _addressMask(_data.size() - 1) {//TODO: dans un cpp
        }
        
        // Ppu memory bus
        template <class TConnectedBus>
        void ppuReadPerformed(TConnectedBus &connectedBus, uint16_t address) const;
        
        template <class TConnectedBus>
        void ppuWritePerformed(TConnectedBus &connectedBus, uint16_t address);
        
    private:
        std::vector<uint8_t> const _data;
        uint16_t const _addressMask;
    };
    
    struct ChrRam {
        
        ChrRam(std::vector<uint8_t> data) : _data(std::move(data)), _addressMask(_data.size() - 1) {//TODO: dans un cpp
        }
        
        // Ppu memory bus
        template <class TConnectedBus>
        void ppuReadPerformed(TConnectedBus &connectedBus, uint16_t address) const;
        
        template <class TConnectedBus>
        void ppuWritePerformed(TConnectedBus &connectedBus, uint16_t address);
        
    private:
        std::vector<uint8_t> _data;
        uint16_t const _addressMask;
    };
    

    template <MirroringType EMirroring, class TCpuChips, class TPpuChips>
    struct Chip {
        
        Chip(TCpuChips cpuChips, TPpuChips ppuChips);
        
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
        template <class TConnectedBus, class ...TChips>
        void cpuReadPerformed(std::tuple<TChips...> &chipsList, TConnectedBus &connectedBus, uint16_t address);
        
        template <class TConnectedBus, class ...TChips>
        void cpuWritePerformed(std::tuple<TChips...> &chipsList, TConnectedBus &connectedBus, uint16_t address);
        
        template <class TConnectedBus, class ...TChips>
        void ppuReadPerformed(std::tuple<TChips...> &chipsList, TConnectedBus &connectedBus, uint16_t address);
        
        template <class TConnectedBus, class ...TChips>
        void ppuWritePerformed(std::tuple<TChips...> &chipsList, TConnectedBus &connectedBus, uint16_t address);
        
        //std::tuple<TChips...> _chips;
        TCpuChips _cpuChips;
        TPpuChips _ppuChips;
    };
    
    // Mapper 0 has only Horizontal or Vertical mirroring
    /*template<>
    struct Chip<MirroringType::SingleScreen>;
    
    template<>
    struct Chip<MirroringType::FourScreen>;*/
    
    #include "Mapper0n_s.hpp"
    
} } }

#endif /* NESEmu_Mapper_Mapper0n_hpp */
