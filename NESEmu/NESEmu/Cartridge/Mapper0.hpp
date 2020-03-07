//
//  Mapper0.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper0_hpp
#define NESEmu_Cartridge_Mapper0_hpp

#include <istream>
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/NROM
// TODO: par apres, factoriser le code commun des mappers
namespace NESEmu { namespace Cartridge {
    
    /*
     
     IPrgRomSizeInKb (ou en bloc de 16kb ??, a voir)
     IPrgRamSizeInKb (est ce que la SRAM a toujours la meme taille ? si oui juste un bool a la place d'un unsigned int pour dire si present ou pas : oui il peut avoir plusieurs tailles (de plus petite a egal a plus grande que l'address range), voir quand taille petite et adresse depasse, est ce que mirror address ou open bus ?)
     IChrRomSizeInKb (ou en bloc de 8kb ? a voir)
     Chr Ram est ce qu'il peut y avoir de la Chr-Ram en plus de la chr rom ? si oui avoir les 2 parametres, sinon avoir juste un parametre IChrMemorySizeInKb et avoir un bool pour savoir si ram ou non (rom)
     MirroringType
     IRQ : voir si tous la gestion des IRQ de tous les mappers est la meme, si oui avoir un bool pour savoir si gere le IRQ ou non sinon ne pas avoir de parametre template et gerer ca dans chaque mapper qui l'utilise : non les mappers implemente l'irq differement, le MMC5 par exemple attends 3 read a la meme address dans un nametable alors que le MMC3 se base sur les edges de PPU A12
     
     Audio : a voir
     Bus Conflit : a voir
     
     */
    
    template <unsigned int IPrgRomSizeInKb, unsigned int IPrgRamSizeInKb, MirroringType EMirroring>//TODO: comment limiter le mirroring a H et V seulement ?
    struct Mapper0 {
        
        Mapper0(std::istream &istream);
        
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
        std::vector<uint8_t> _prgRom;
        std::vector<uint8_t> _prgRam;
        std::vector<uint8_t> _chrRom;
    };
    
    #include "Mapper0_s.hpp"
    
} }

#endif /* NESEmu_Cartridge_Mapper0_hpp */
