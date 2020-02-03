//
//  AddressingMode.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_AddressingMode_hpp
#define Cpu6502_Internal_AddressingMode_hpp


void implied();
void immediate();
void absolute0();
void absolute1();
void absoluteRead();
void absoluteWrite(uint8_t data);
void absoluteRMWRead();
void zeroPage();
void zeroPageRead();
void zeroPageWrite(uint8_t data);
void zeroPageRMWRead();
void relative0();
void relative1(bool condition);
void relativeBranch0();
void relativeBranch1();
void absoluteIndexed0();
void absoluteIndexed1(uint8_t index);
void absoluteIndexedX1();
void absoluteIndexedY1();
void absoluteIndexedRead0(OpcodeInstruction nextInstruction);
void absoluteIndexedRead1();
void absoluteIndexedRMWRead0();
void absoluteIndexedRMWRead1();
void absoluteIndexedWrite0();
void absoluteIndexedWrite1(uint8_t data);
void zeroPageIndexed0();
void zeroPageIndexed1(uint8_t index);
void zeroPageIndexedX1();
void zeroPageIndexedY1();
void zeroPageIndexedRead();
void zeroPageIndexedWrite(uint8_t data);
void zeroPageIndexedRMWRead();
void zeroPagePreIndexedIndirect0();
void zeroPagePreIndexedIndirect1();
void zeroPagePreIndexedIndirect2();
void zeroPagePreIndexedIndirect3();
void zeroPagePreIndexedIndirectRead();
void zeroPagePreIndexedIndirectWrite(uint8_t data);
void zeroPagePreIndexedIndirectRMWRead();
void zeroPageIndirectPostIndexed0();
void zeroPageIndirectPostIndexed1();
void zeroPageIndirectPostIndexed2();
void zeroPageIndirectPostIndexedRead0(OpcodeInstruction nextInstruction);
void zeroPageIndirectPostIndexedRead1();
void zeroPageIndirectPostIndexedWrite0();
void zeroPageIndirectPostIndexedRMWRead0();
void zeroPageIndirectPostIndexedRMWRead1();
void zeroPageIndirectPostIndexedWrite1(uint8_t data);
void anyRMWModify(OpcodeInstruction nextInstruction, uint8_t data);
void anyRMWWrite(uint8_t data);

#endif /* Cpu6502_Internal_AddressingMode_hpp */
