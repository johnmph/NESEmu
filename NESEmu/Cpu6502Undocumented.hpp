//
//  Cpu6502Undocumented.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 12/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502Undocumented_hpp
#define Cpu6502Undocumented_hpp
// TODO: finir les opcodes non officiels : https://www.pagetable.com/?p=39

// STP, SLO, RLA, SRE, SRE, RRA, SAX (AXS), XAA, AHX, TAS, SHY, SHX, LAS, DCP, ISC, SBC($EB), ALR, ANC, ARR, LAX, NOP (other addressing modes), KIL (Kill instruction, opcodes which jams the machine)

// ASO (SLO) = ASL + ORA

void aso0();
void aso1();
void aso2();

void asoZp0();
void asoZp1();

void asoZpX0();
void asoZpX1();
void asoZpX2();

void asoAbs0();
void asoAbs1();
void asoAbs2();

void asoAbsX0();
void asoAbsX1();
void asoAbsX2();
void asoAbsX3();

void asoAbsY0();
void asoAbsY1();
void asoAbsY2();
void asoAbsY3();

void asoIndX0();
void asoIndX1();
void asoIndX2();
void asoIndX3();
void asoIndX4();

void asoIndY0();
void asoIndY1();
void asoIndY2();
void asoIndY3();
void asoIndY4();

// RLA = ROL + AND

void rla0();
void rla1();
void rla2();

void rlaZp0();
void rlaZp1();

void rlaZpX0();
void rlaZpX1();
void rlaZpX2();

void rlaAbs0();
void rlaAbs1();
void rlaAbs2();

void rlaAbsX0();
void rlaAbsX1();
void rlaAbsX2();
void rlaAbsX3();

void rlaAbsY0();
void rlaAbsY1();
void rlaAbsY2();
void rlaAbsY3();

void rlaIndX0();
void rlaIndX1();
void rlaIndX2();
void rlaIndX3();
void rlaIndX4();

void rlaIndY0();
void rlaIndY1();
void rlaIndY2();
void rlaIndY3();
void rlaIndY4();

// LSE (SRE) = LSR + EOR

void lse0();
void lse1();
void lse2();

void lseZp0();
void lseZp1();

void lseZpX0();
void lseZpX1();
void lseZpX2();

void lseAbs0();
void lseAbs1();
void lseAbs2();

void lseAbsX0();
void lseAbsX1();
void lseAbsX2();
void lseAbsX3();

void lseAbsY0();
void lseAbsY1();
void lseAbsY2();
void lseAbsY3();

void lseIndX0();
void lseIndX1();
void lseIndX2();
void lseIndX3();
void lseIndX4();

void lseIndY0();
void lseIndY1();
void lseIndY2();
void lseIndY3();
void lseIndY4();

// RRA = ROR + ADC

void rra0();
void rra1();
void rra2();

void rraZp0();
void rraZp1();

void rraZpX0();
void rraZpX1();
void rraZpX2();

void rraAbs0();
void rraAbs1();
void rraAbs2();

void rraAbsX0();
void rraAbsX1();
void rraAbsX2();
void rraAbsX3();

void rraAbsY0();
void rraAbsY1();
void rraAbsY2();
void rraAbsY3();

void rraIndX0();
void rraIndX1();
void rraIndX2();
void rraIndX3();
void rraIndX4();

void rraIndY0();
void rraIndY1();
void rraIndY2();
void rraIndY3();
void rraIndY4();

// AXS (SAX) = STA + STX

void axsZp0();
void axsZp1();

void axsZpY0();
void axsZpY1();
void axsZpY2();

void axsAbs0();
void axsAbs1();
void axsAbs2();

void axsIndX0();
void axsIndX1();
void axsIndX2();
void axsIndX3();
void axsIndX4();

// LAX = LDA + LDX

void lax0();

void laxZp0();
void laxZp1();

void laxZpY0();
void laxZpY1();
void laxZpY2();

void laxAbs0();
void laxAbs1();
void laxAbs2();

void laxAbsY0();
void laxAbsY1();
void laxAbsY2();
void laxAbsY3();

void laxIndX0();
void laxIndX1();
void laxIndX2();
void laxIndX3();
void laxIndX4();

void laxIndY0();
void laxIndY1();
void laxIndY2();
void laxIndY3();
void laxIndY4();

// DCP (DCM) = DEC + CMP

void dcp0();
void dcp1();
void dcp2();
void dcp3();

void dcpZp0();
void dcpZp1();

void dcpZpX0();
void dcpZpX1();
void dcpZpX2();

void dcpAbs0();
void dcpAbs1();
void dcpAbs2();

void dcpAbsX0();
void dcpAbsX1();
void dcpAbsX2();
void dcpAbsX3();

void dcpAbsY0();
void dcpAbsY1();
void dcpAbsY2();
void dcpAbsY3();

void dcpIndX0();
void dcpIndX1();
void dcpIndX2();
void dcpIndX3();
void dcpIndX4();

void dcpIndY0();
void dcpIndY1();
void dcpIndY2();
void dcpIndY3();
void dcpIndY4();

// INS (ISC) = INC + SBC

void ins0();
void ins1();
void ins2();
void ins3();

void insZp0();
void insZp1();

void insZpX0();
void insZpX1();
void insZpX2();

void insAbs0();
void insAbs1();
void insAbs2();

void insAbsX0();
void insAbsX1();
void insAbsX2();
void insAbsX3();

void insAbsY0();
void insAbsY1();
void insAbsY2();
void insAbsY3();

void insIndX0();
void insIndX1();
void insIndX2();
void insIndX3();
void insIndX4();

void insIndY0();
void insIndY1();
void insIndY2();
void insIndY3();
void insIndY4();

// ALR = AND + LSR

// ARR = AND + ROR

// XAA = TXA + AND

// OAL = ORA #$EE + AND + TAX

// SAX = AND + SBC (on X register)

// NOP

void nopImm0();

void nopZp0();
void nopZp1();

void nopZpX0();
void nopZpX1();
void nopZpX2();

void nopAbs0();
void nopAbs1();
void nopAbs2();

void nopAbsX0();
void nopAbsX1();
void nopAbsX2();
void nopAbsX3();

// KIL

void kil();

// TAS = AND + TXS + STA (On X register)

// SAY = AND + STA (On Y register)

// XAS = AND + STA (On X register)

// AXA =

// ANC = AND + ROL

// LAS = AND + TAX + TXS


#endif /* Cpu6502Undocumented_hpp */
