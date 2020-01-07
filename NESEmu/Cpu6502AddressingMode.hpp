//
//  Cpu6502AddressingMode.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502AddressingMode_hpp
#define Cpu6502AddressingMode_hpp


void implied();
void immediate();
void absolute0();
void absolute1();
void absoluteLoad();
void absoluteStore(uint8_t data);
void zeroPage();
void zeroPageLoad();
void zeroPageStore(uint8_t data);
void relative();
void relativeBranch0();
void relativeBranch1();
void absoluteIndexed0();
void absoluteIndexed1(uint8_t index);
void absoluteIndexedX1();
void absoluteIndexedY1();
void absoluteIndexedLoad0();
void absoluteIndexedLoad1();
void absoluteIndexedStore0();
void absoluteIndexedStore1(uint8_t data);
void zeroPageIndexed0();
void zeroPageIndexed1(uint8_t index);
void zeroPageIndexedX1();
void zeroPageIndexedY1();
void zeroPageIndexedLoad();
void zeroPageIndexedStore(uint8_t data);
void zeroPagePreIndexedIndirect0();
void zeroPagePreIndexedIndirect1();
void zeroPagePreIndexedIndirect2();
void zeroPagePreIndexedIndirect3();
void zeroPagePreIndexedIndirectLoad();
void zeroPagePreIndexedIndirectStore(uint8_t data);
void zeroPageIndirectPostIndexed0();
void zeroPageIndirectPostIndexed1();
void zeroPageIndirectPostIndexed2();
void zeroPageIndirectPostIndexedLoad0();
void zeroPageIndirectPostIndexedLoad1();
void zeroPageIndirectPostIndexedStore0();
void zeroPageIndirectPostIndexedStore1(uint8_t data);

#endif /* Cpu6502AddressingMode_hpp */
