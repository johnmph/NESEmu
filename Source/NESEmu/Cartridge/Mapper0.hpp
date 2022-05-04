//
//  Mapper0.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cartridge_Mapper0_hpp
#define NESEmu_Cartridge_Mapper0_hpp

#include <vector>
#include "Interface.hpp"
#include "Common.hpp"

// See https://wiki.nesdev.com/w/index.php/NROM
// TODO: par apres, factoriser le code commun des mappers
namespace NESEmu { namespace Cartridge { namespace Mapper0 {
    
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
    
    template <class TCpuHardwareInterface, class TPpuHardwareInterface>
    struct Chip : Interface<TCpuHardwareInterface, TPpuHardwareInterface> {
        
        Chip(std::vector<uint8_t> prgRom, std::size_t prgRamSize, std::vector<uint8_t> chrRom, std::size_t chrRamSize, MirroringType mirroringType);
        
        // Cpu memory bus
        void cpuReadPerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        void cpuWritePerformed(TCpuHardwareInterface &cpuHardwareInterface) override;
        
        // Ppu memory bus
        void ppuReadPerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        void ppuWritePerformed(TPpuHardwareInterface &ppuHardwareInterface) override;
        
    private:
        uint16_t getVramAddress(uint16_t address);
        
        MirroringType const _mirroringType;
    };
    
    #include "Mapper0_s.hpp"
    
} } }

#endif /* NESEmu_Cartridge_Mapper0_hpp */
