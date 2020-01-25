//
//  Cpu_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 23/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef NESEmu_Cpu_s_hpp
#define NESEmu_Cpu_s_hpp


template <>
struct Constants<Model::Ricoh2A03> {
    // APU
    static constexpr int apuFrameCounterRateInHz = 60;
};

template <>
struct Constants<Model::Ricoh2A07> {
    // APU
    static constexpr int apuFrameCounterRateInHz = 50;
};


template <Model EModel, class TBus>
Chip<EModel, TBus>::Chip(TBus &bus) : _bus(bus), _cpu(*this) {
}

// TODO: ne pas oublier les 1ers dummy cycle (voir : ) : Il faut en fait terminer l'instruction courante qui a fait le dma et commencer le DMA : on doit laisser decodeOpcodeAndExecuteInstruction decoder l'instruction (setter _currentInstruction) mais pas l'executer et executer le DMA a la place, puis une fois le DMA fini, executer _currentInstruction (simplement appeler clock du CPU)

// TODO: voir si le DMA correct ainsi, si le DMA peut etre interrompu par une interrupt ce n'est pas bon ainsi, si le DMA n'affecte pas l'addressBusLow et addressBusHigh, _inputDataLatch, _dataOutput et _readWrite, passer direct par read et write d'ici plutot que par celui du cpu : le DMA affecte addressBusLow/High, _inputDataLatch, _dataOutput et _readWrite et normalement le DMA n'est pas interrompu par un interrupt mais l'interrupt commence directement apres le DMA
// TODO: si DMA started mais sync == false, on execute un clock qui n'execute rien si memory read car ready = false, donc voir si possible de ne jamais avoir sync et donc d'etre bloqué ici !!! : si comme ca dans ce cas, ne mettre ready = false qu'une fois dans le if du dma ici et pas dans le write du $4014

template <Model EModel, class TBus>
void Chip<EModel, TBus>::clock() {
    // Execute DMA if asked DMA and if CPU has terminated current instruction
    if ((_dmaCount > 0) && ((_dmaCount != 256) || (_cpu.getSyncSignal() == true))) {    // TODO: normalement le _dmaCount != 256 n'est pas nécessaire
        if (_dmaToggle == false) {
            // Read from dmaAddress
            _cpu.readDataBus(256 - _dmaCount, _dmaAddress);
            //_dmaData = read((_dmaAddress << 8) | (256 - _dmaCount));
        } else {
            // Write to PPU
            _cpu.writeDataBus(0x04, 0x20, _cpu.getDataBus());
            //write((_dmaAddress << 8) | (256 - _dmaCount), _dmaData);
            
            // One byte copied
            --_dmaCount;
            
            // Reenable CPU if DMA is completed
            if (_dmaCount == 0) {
                // TODO: normalement ici le PC ne sera pas bon (incrementé) car on a fetché avant le DMA et le DMA a modifié le predecode register !!!
                // TODO: on dirait que le fetchOpcode est un fetch special qui ne check pas les interruptions et qui fetche a partir d'une adresse sauvée (car comme marqué juste au dessus le PC est deja incrementé)
                _cpu.ready(true);
            }
        }
        
        _dmaToggle = !_dmaToggle;
    }
    
    // Cpu clock
    _cpu.clock();
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::reset(bool high) {
    _cpu.reset(high);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::nmi(bool high) {
    _cpu.nmi(high);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::irq(bool high) {
    _cpu.irq(high);
}

template <Model EModel, class TBus>
uint8_t Chip<EModel, TBus>::getDataBus() const {
    return _cpu.getDataBus();
}

template <Model EModel, class TBus>
uint8_t Chip<EModel, TBus>::read(uint16_t address) {
    // The CPU itself decodes reads of $4015 (APU status) and $4016-$4017 (controller read). But no circuit in the Control Deck decodes reads from $4000-$4014 or $4018-$7FFF
    // See https://wiki.nesdev.com/w/index.php/Open_bus_behavior
    
    // Sound
    if (address == 0x4015) {
        return 0x0; // TODO: changer
    }
    // Joystick 1
    else if (address == 0x4016) {
        return 0x0; // TODO: changer
    }
    // Joystick 2
    else if (address == 0x4017) {
        return 0x0; // TODO: changer
    }
    
    // External bus
    return _bus.read(address);
}

template <Model EModel, class TBus>
void Chip<EModel, TBus>::write(uint16_t address, uint8_t data) {
    // Sound
    if (((address >= 0x4000) && (address < 0x4014)) || (address == 0x4015)) {
        //TODO: ajouter
        return;
    }
    // DMA
    else if (address == 0x4014) {
        // Start a DMA transfert
        _dmaAddress = data;
        _dmaCount = 256;
        _cpu.ready(false);
        
        return;
    }
    // Joystick 1
    else if (address == 0x4016) {
        //TODO: ajouter
        return;
    }
    // Joystick 2
    else if (address == 0x4017) {
        //TODO: ajouter
        return;
    }
    // Test mode
    else if ((address >= 0x4018) && (address < 0x4020)) {
        //TODO: ajouter
        return;
    }
    
    // External bus
    _bus.write(address, data);
}

#endif /* NESEmu_Cpu_s_hpp */
