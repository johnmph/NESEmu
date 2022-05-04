//
//  Register.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Register_hpp
#define Cpu6502_Internal_Register_hpp


// TAX, TXA, TAY, TYA, TSX, TXS

void transfert0(uint8_t source, uint8_t &destination);

void tax0();
void tax1();

void txa0();
void txa1();

void tay0();
void tay1();

void tya0();
void tya1();

void tsx0();
void tsx1();

void txs0();
void txs1();

#endif /* Cpu6502_Internal_Register_hpp */
