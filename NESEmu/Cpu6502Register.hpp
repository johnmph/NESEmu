//
//  Cpu6502Register.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Register_hpp
#define Cpu6502Register_hpp


// TAX, TXA, TAY, TYA, TSX, TXS

void transfert0(uint8_t source, uint8_t &destination);

void tax0();

void txa0();

void tay0();

void tya0();

void tsx0();

void txs0();

#endif /* Cpu6502Register_hpp */
