//
//  Arithmetic.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Arithmetic_hpp
#define Cpu6502_Internal_Arithmetic_hpp


// ADC, SBC, CMP, CPX, CPY, DEC, INC, DEX, DEY, INX, INY

void arithmetic1();

void adc0();

void adcImm0();

void adcZp0();
void adcZp1();
void adcZp2();

void adcZpX0();
void adcZpX1();
void adcZpX2();
void adcZpX3();

void adcAbs0();
void adcAbs1();
void adcAbs2();
void adcAbs3();

void adcAbsX0();
void adcAbsX1();
void adcAbsX2();
void adcAbsX3();
void adcAbsX4();

void adcAbsY0();
void adcAbsY1();
void adcAbsY2();
void adcAbsY3();
void adcAbsY4();

void adcIndX0();
void adcIndX1();
void adcIndX2();
void adcIndX3();
void adcIndX4();
void adcIndX5();

void adcIndY0();
void adcIndY1();
void adcIndY2();
void adcIndY3();
void adcIndY4();
void adcIndY5();

void sbc0();

void sbcImm0();

void sbcZp0();
void sbcZp1();
void sbcZp2();

void sbcZpX0();
void sbcZpX1();
void sbcZpX2();
void sbcZpX3();

void sbcAbs0();
void sbcAbs1();
void sbcAbs2();
void sbcAbs3();

void sbcAbsX0();
void sbcAbsX1();
void sbcAbsX2();
void sbcAbsX3();
void sbcAbsX4();

void sbcAbsY0();
void sbcAbsY1();
void sbcAbsY2();
void sbcAbsY3();
void sbcAbsY4();

void sbcIndX0();
void sbcIndX1();
void sbcIndX2();
void sbcIndX3();
void sbcIndX4();
void sbcIndX5();

void sbcIndY0();
void sbcIndY1();
void sbcIndY2();
void sbcIndY3();
void sbcIndY4();
void sbcIndY5();

void cp0(uint8_t data);
void cp1();

void cmp0();

void cmpImm0();

void cmpZp0();
void cmpZp1();
void cmpZp2();

void cmpZpX0();
void cmpZpX1();
void cmpZpX2();
void cmpZpX3();

void cmpAbs0();
void cmpAbs1();
void cmpAbs2();
void cmpAbs3();

void cmpAbsX0();
void cmpAbsX1();
void cmpAbsX2();
void cmpAbsX3();
void cmpAbsX4();

void cmpAbsY0();
void cmpAbsY1();
void cmpAbsY2();
void cmpAbsY3();
void cmpAbsY4();

void cmpIndX0();
void cmpIndX1();
void cmpIndX2();
void cmpIndX3();
void cmpIndX4();
void cmpIndX5();

void cmpIndY0();
void cmpIndY1();
void cmpIndY2();
void cmpIndY3();
void cmpIndY4();
void cmpIndY5();

void cpx0();

void cpxImm0();

void cpxZp0();
void cpxZp1();
void cpxZp2();

void cpxAbs0();
void cpxAbs1();
void cpxAbs2();
void cpxAbs3();

void cpy0();

void cpyImm0();

void cpyZp0();
void cpyZp1();
void cpyZp2();

void cpyAbs0();
void cpyAbs1();
void cpyAbs2();
void cpyAbs3();

void dec(uint8_t data);
void inc(uint8_t data);

void incdecMemory1();

void decMemory0();

void decZp0();
void decZp1();

void decZpX0();
void decZpX1();
void decZpX2();

void decAbs0();
void decAbs1();
void decAbs2();

void decAbsX0();
void decAbsX1();
void decAbsX2();
void decAbsX3();

void incMemory0();

void incZp0();
void incZp1();

void incZpX0();
void incZpX1();
void incZpX2();

void incAbs0();
void incAbs1();
void incAbs2();

void incAbsX0();
void incAbsX1();
void incAbsX2();
void incAbsX3();

void incdecRegister2(uint8_t &data);

void decRegister1(OpcodeInstruction nextInstruction, uint8_t data);

void dex0();
void dex1();
void dex2();

void dey0();
void dey1();
void dey2();

void incRegister1(OpcodeInstruction nextInstruction, uint8_t data);

void inx0();
void inx1();
void inx2();

void iny0();
void iny1();
void iny2();

#endif /* Cpu6502_Internal_Arithmetic_hpp */
