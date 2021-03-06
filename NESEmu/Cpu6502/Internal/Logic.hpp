//
//  Logic.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Logic_hpp
#define Cpu6502_Internal_Logic_hpp


// AND, ASL, BIT, EOR, LSR, ORA, ROL, ROR

void and0();

void andImm0();

void andZp0();
void andZp1();

void andZpX0();
void andZpX1();
void andZpX2();

void andAbs0();
void andAbs1();
void andAbs2();

void andAbsX0();
void andAbsX1();
void andAbsX2();
void andAbsX3();

void andAbsY0();
void andAbsY1();
void andAbsY2();
void andAbsY3();

void andIndX0();
void andIndX1();
void andIndX2();
void andIndX3();
void andIndX4();

void andIndY0();
void andIndY1();
void andIndY2();
void andIndY3();
void andIndY4();

void ora0();

void oraImm0();

void oraZp0();
void oraZp1();

void oraZpX0();
void oraZpX1();
void oraZpX2();

void oraAbs0();
void oraAbs1();
void oraAbs2();

void oraAbsX0();
void oraAbsX1();
void oraAbsX2();
void oraAbsX3();

void oraAbsY0();
void oraAbsY1();
void oraAbsY2();
void oraAbsY3();

void oraIndX0();
void oraIndX1();
void oraIndX2();
void oraIndX3();
void oraIndX4();

void oraIndY0();
void oraIndY1();
void oraIndY2();
void oraIndY3();
void oraIndY4();

void eor0();

void eorImm0();

void eorZp0();
void eorZp1();

void eorZpX0();
void eorZpX1();
void eorZpX2();

void eorAbs0();
void eorAbs1();
void eorAbs2();

void eorAbsX0();
void eorAbsX1();
void eorAbsX2();
void eorAbsX3();

void eorAbsY0();
void eorAbsY1();
void eorAbsY2();
void eorAbsY3();

void eorIndX0();
void eorIndX1();
void eorIndX2();
void eorIndX3();
void eorIndX4();

void eorIndY0();
void eorIndY1();
void eorIndY2();
void eorIndY3();
void eorIndY4();

void bit0();
void bit1();

void bitZp0();
void bitZp1();

void bitAbs0();
void bitAbs1();
void bitAbs2();

void asl(uint8_t data);
void aslMemory0();

void aslImm0();
void aslImm1();

void aslZp0();
void aslZp1();

void aslZpX0();
void aslZpX1();
void aslZpX2();

void aslAbs0();
void aslAbs1();
void aslAbs2();

void aslAbsX0();
void aslAbsX1();
void aslAbsX2();
void aslAbsX3();

void lsr(uint8_t data);
void lsrMemory0();

void lsrImm0();
void lsrImm1();

void lsrZp0();
void lsrZp1();

void lsrZpX0();
void lsrZpX1();
void lsrZpX2();

void lsrAbs0();
void lsrAbs1();
void lsrAbs2();

void lsrAbsX0();
void lsrAbsX1();
void lsrAbsX2();
void lsrAbsX3();

void rol(uint8_t data);
void rolMemory0();

void rolImm0();
void rolImm1();

void rolZp0();
void rolZp1();

void rolZpX0();
void rolZpX1();
void rolZpX2();

void rolAbs0();
void rolAbs1();
void rolAbs2();

void rolAbsX0();
void rolAbsX1();
void rolAbsX2();
void rolAbsX3();

void ror(uint8_t data);
void rorMemory0();

void rorImm0();
void rorImm1();

void rorZp0();
void rorZp1();

void rorZpX0();
void rorZpX1();
void rorZpX2();

void rorAbs0();
void rorAbs1();
void rorAbs2();

void rorAbsX0();
void rorAbsX1();
void rorAbsX2();
void rorAbsX3();

#endif /* Cpu6502_Internal_Logic_hpp */
