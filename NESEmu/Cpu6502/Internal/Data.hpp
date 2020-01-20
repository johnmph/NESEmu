//
//  Data.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 6/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_Internal_Data_hpp
#define Cpu6502_Internal_Data_hpp


/*
 
 MODE           SYNTAX
 ---------------------
 Implied        Instr
 Immediate      Instr #$da
 Relative       Instr $of
 Zero Page      Instr $az
 Zero Page, X   Instr $az, X
 Absolute       Instr $ahal
 Absolute, X    Instr $ahal, X
 Absolute, Y    Instr $ahal, Y
 Indirect, X    Instr ($az, X)
 Indirect, Y    Instr ($az), Y
 
 da = data
 of = offset
 az = address on page zero
 ah = address high
 al = address low
 *  = Unofficial instruction
 
 */

template <class TBus, bool DecimalSupported>
const typename Chip<TBus, DecimalSupported>::OpcodeInstruction Chip<TBus, DecimalSupported>::_opcodeInstructionFuncs[256] = {
    // $00 -> Brk
    &Chip::brk0,
    // $01 -> ORA ($az, X)
    &Chip::oraIndX0,
    // $02 -> KIL
    &Chip::kil,
    // $03 -> ASO* ($az, X)
    &Chip::asoIndX0,
    // $04 -> NOP* $az
    &Chip::nopZp0,
    // $05 -> ORA $az
    &Chip::oraZp0,
    // $06 -> ASL $az
    &Chip::aslZp0,
    // $07 -> ASO* $az
    &Chip::asoZp0,
    // $08 -> PHP
    &Chip::php0,
    // $09 -> ORA #$da
    &Chip::oraImm0,
    // $0A -> ASL (accumulator)
    &Chip::aslImm0,
    // $0B -> ?
    &Chip::unofficial,
    // $0C -> NOP* $ahal
    &Chip::nopAbs0,
    // $0D -> ORA $ahal
    &Chip::oraAbs0,
    // $0E -> ASL $ahal
    &Chip::aslAbs0,
    // $0F -> ASO* $ahal
    &Chip::asoAbs0,
    // $10 -> BPL $of
    &Chip::bpl0,
    // $11 -> ORA ($az), Y
    &Chip::oraIndY0,
    // $12 -> KIL
    &Chip::kil,
    // $13 -> ASO* ($az), Y
    &Chip::asoIndY0,
    // $14 -> NOP* $az, X
    &Chip::nopZpX0,
    // $15 -> ORA $az, X
    &Chip::oraZpX0,
    // $16 -> ASL $az, X
    &Chip::aslZpX0,
    // $17 -> ASO* $az, X
    &Chip::asoZpX0,
    // $18 -> CLC
    &Chip::clc0,
    // $19 -> ORA $ahal, Y
    &Chip::oraAbsY0,
    // $1A -> NOP*
    &Chip::nop0,
    // $1B -> ASO* $ahal, Y
    &Chip::asoAbsY0,
    // $1C -> NOP* $ahal, X
    &Chip::nopAbsX0,
    // $1D -> ORA $ahal, X
    &Chip::oraAbsX0,
    // $1E -> ASL $ahal, X
    &Chip::aslAbsX0,
    // $1F -> ASO* $ahal, X
    &Chip::asoAbsX0,
    // $20 -> JSR
    &Chip::jsr0,
    // $21 -> AND ($az, X)
    &Chip::andIndX0,
    // $22 -> KIL
    &Chip::kil,
    // $23 -> RLA* ($az, X)
    &Chip::rlaIndX0,
    // $24 -> BIT $az
    &Chip::bitZp0,
    // $25 -> AND $az
    &Chip::andZp0,
    // $26 -> ROL $az
    &Chip::rolZp0,
    // $27 -> RLA* $az
    &Chip::rlaZp0,
    // $28 -> PLP
    &Chip::plp0,
    // $29 -> AND #$da
    &Chip::andImm0,
    // $2A -> ROL (Accumulator)
    &Chip::rolImm0,
    // $2B -> ?
    &Chip::unofficial,
    // $2C -> BIT $ahal
    &Chip::bitAbs0,
    // $2D -> AND $ahal
    &Chip::andAbs0,
    // $2E -> ROL $ahal
    &Chip::rolAbs0,
    // $2F -> RLA* $ahal
    &Chip::rlaAbs0,
    // $30 -> BMI $of
    &Chip::bmi0,
    // $31 -> AND ($az), Y
    &Chip::andIndY0,
    // $32 -> KIL
    &Chip::kil,
    // $33 -> RLA* ($az), Y
    &Chip::rlaIndY0,
    // $34 -> NOP* $az, X
    &Chip::nopZpX0,
    // $35 -> AND $az, X
    &Chip::andZpX0,
    // $36 -> ROL $az, X
    &Chip::rolZpX0,
    // $37 -> RLA* $az, X
    &Chip::rlaZpX0,
    // $38 -> SEC
    &Chip::sec0,
    // $39 -> AND $ahal, Y
    &Chip::andAbsY0,
    // $3A -> NOP*
    &Chip::nop0,
    // $3B -> RLA* $ahal, Y
    &Chip::rlaAbsY0,
    // $3C -> NOP* $ahal, X
    &Chip::nopAbsX0,
    // $3D -> AND $ahal, X
    &Chip::andAbsX0,
    // $3E -> ROL $ahal, X
    &Chip::rolAbsX0,
    // $3F -> RLA* $ahal, X
    &Chip::rlaAbsX0,
    // $40 -> RTI
    &Chip::rti0,
    // $41 -> EOR ($az, X)
    &Chip::eorIndX0,
    // $42 -> KIL
    &Chip::kil,
    // $43 -> LSE* ($az, X)
    &Chip::lseIndX0,
    // $44 -> NOP* $az
    &Chip::nopZp0,
    // $45 -> EOR $az
    &Chip::eorZp0,
    // $46 -> LSR $az
    &Chip::lsrZp0,
    // $47 -> LSE* $az
    &Chip::lseZp0,
    // $48 -> PHA
    &Chip::pha0,
    // $49 -> EOR #$da
    &Chip::eorImm0,
    // $4A -> LSR (Accumulator)
    &Chip::lsrImm0,
    // $4B -> ALR* #$da
    &Chip::alrImm0,
    // $4C -> JMP $ahal
    &Chip::jmpAbs0,
    // $4D -> EOR $ahal
    &Chip::eorAbs0,
    // $4E -> LSR $ahal
    &Chip::lsrAbs0,
    // $4F -> LSE* $ahal
    &Chip::lseAbs0,
    // $50 -> BVC $of
    &Chip::bvc0,
    // $51 -> EOR ($az), Y
    &Chip::eorIndY0,
    // $52 -> KIL
    &Chip::kil,
    // $53 -> LSE* ($az), Y
    &Chip::lseIndY0,
    // $54 -> NOP* $az, X
    &Chip::nopZpX0,
    // $55 -> EOR $az, X
    &Chip::eorZpX0,
    // $56 -> LSR $az, X
    &Chip::lsrZpX0,
    // $57 -> LSE* $az, X
    &Chip::lseZpX0,
    // $58 -> CLI
    &Chip::cli0,
    // $59 -> EOR $ahal, Y
    &Chip::eorAbsY0,
    // $5A -> NOP*
    &Chip::nop0,
    // $5B -> LSE* $ahal, Y
    &Chip::lseAbsY0,
    // $5C -> NOP* $ahal, X
    &Chip::nopAbsX0,
    // $5D -> EOR $ahal, X
    &Chip::eorAbsX0,
    // $5E -> LSR $ahal, X
    &Chip::lsrAbsX0,
    // $5F -> LSE* $ahal, X
    &Chip::lseAbsX0,
    // $60 -> RTS
    &Chip::rts0,
    // $61 -> ADC ($az, X)
    &Chip::adcIndX0,
    // $62 -> KIL
    &Chip::kil,
    // $63 -> RRA* ($az, X)
    &Chip::rraIndX0,
    // $64 -> NOP* $az
    &Chip::nopZp0,
    // $65 -> ADC $az
    &Chip::adcZp0,
    // $66 -> ROR $az
    &Chip::rorZp0,
    // $67 -> RRA* $az
    &Chip::rraZp0,
    // $68 -> PLA
    &Chip::pla0,
    // $69 -> ADC #$da
    &Chip::adcImm0,
    // $6A -> ROR (Accumulator)
    &Chip::rorImm0,
    // $6B -> ?
    &Chip::unofficial,
    // $6C -> JMP ($ahal)
    &Chip::jmpInd0,
    // $6D -> ADC $ahal
    &Chip::adcAbs0,
    // $6E -> ROR $ahal
    &Chip::rorAbs0,
    // $6F -> RRA* $ahal
    &Chip::rraAbs0,
    // $70 -> BVS $of
    &Chip::bvs0,
    // $71 -> ADC ($az), Y
    &Chip::adcIndY0,
    // $72 -> KIL
    &Chip::kil,
    // $73 -> RRA* ($az), Y
    &Chip::rraIndY0,
    // $74 -> NOP* $az, X
    &Chip::nopZpX0,
    // $75 -> ADC $az, X
    &Chip::adcZpX0,
    // $76 -> ROR $az, X
    &Chip::rorZpX0,
    // $77 -> RRA* $az, X
    &Chip::rraZpX0,
    // $78 -> SEI
    &Chip::sei0,
    // $79 -> ADC $ahal, Y
    &Chip::adcAbsY0,
    // $7A -> NOP*
    &Chip::nop0,
    // $7B -> RRA* $ahal, Y
    &Chip::rraAbsY0,
    // $7C -> NOP* $ahal, X
    &Chip::nopAbsX0,
    // $7D -> ADC $ahal, X
    &Chip::adcAbsX0,
    // $7E -> ROR $ahal, X
    &Chip::rorAbsX0,
    // $7F -> RRA* $ahal, X
    &Chip::rraAbsX0,
    // $80 -> NOP* #$da
    &Chip::nopImm0,
    // $81 -> STA ($az, X)
    &Chip::staIndX0,
    // $82 -> NOP*
    &Chip::nop0,
    // $83 -> AXS* ($az, X)
    &Chip::axsIndX0,
    // $84 -> STY $az
    &Chip::styZp0,
    // $85 -> STA $az
    &Chip::staZp0,
    // $86 -> STX $az
    &Chip::stxZp0,
    // $87 -> AXS* $az
    &Chip::axsZp0,
    // $88 -> DEY
    &Chip::dey0,
    // $89 -> NOP*
    &Chip::nop0,
    // $8A -> TXA
    &Chip::txa0,
    // $8B -> ?
    &Chip::unofficial,
    // $8C -> STY $ahal
    &Chip::styAbs0,
    // $8D -> STA $ahal
    &Chip::staAbs0,
    // $8E -> STX $ahal
    &Chip::stxAbs0,
    // $8F -> AXS* $ahal
    &Chip::axsAbs0,
    // $90 -> BCC $of
    &Chip::bcc0,
    // $91 -> STA ($az), Y
    &Chip::staIndY0,
    // $92 -> KIL
    &Chip::kil,
    // $93 -> ?
    &Chip::unofficial,
    // $94 -> STY $az, X
    &Chip::styZpX0,
    // $95 -> STA $az, X
    &Chip::staZpX0,
    // $96 -> STX $az, Y
    &Chip::stxZpY0,
    // $97 -> AXS* $az, Y
    &Chip::axsZpY0,
    // $98 -> TYA
    &Chip::tya0,
    // $99 -> STA $ahal, Y
    &Chip::staAbsY0,
    // $9A -> TXS
    &Chip::txs0,
    // $9B -> ?
    &Chip::unofficial,
    // $9C -> ?
    &Chip::unofficial,
    // $9D -> STA $ahal, X
    &Chip::staAbsX0,
    // $9E -> ?
    &Chip::unofficial,
    // $9F -> ?
    &Chip::unofficial,
    // $A0 -> LDY #$da
    &Chip::ldyImm0,
    // $A1 -> LDA ($az, X)
    &Chip::ldaIndX0,
    // $A2 -> LDX #$da
    &Chip::ldxImm0,
    // $A3 -> LAX* ($az, X)
    &Chip::laxIndX0,
    // $A4 -> LDY $az
    &Chip::ldyZp0,
    // $A5 -> LDA $az
    &Chip::ldaZp0,
    // $A6 -> LDX $az
    &Chip::ldxZp0,
    // $A7 -> LAX* $az
    &Chip::laxZp0,
    // $A8 -> TAY
    &Chip::tay0,
    // $A9 -> LDA #$da
    &Chip::ldaImm0,
    // $AA -> TAX
    &Chip::tax0,
    // $AB -> ?
    &Chip::unofficial,
    // $AC -> LDY $ahal
    &Chip::ldyAbs0,
    // $AD -> LDA $ahal
    &Chip::ldaAbs0,
    // $AE -> LDX $ahal
    &Chip::ldxAbs0,
    // $AF -> LAX* $ahal
    &Chip::laxAbs0,
    // $B0 -> BCS $of
    &Chip::bcs0,
    // $B1 -> LDA ($az), Y
    &Chip::ldaIndY0,
    // $B2 -> KIL
    &Chip::kil,
    // $B3 -> LAX ($zp), Y
    &Chip::laxIndY0,
    // $B4 -> LDY $az, X
    &Chip::ldyZpX0,
    // $B5 -> LDA $az, X
    &Chip::ldaZpX0,
    // $B6 -> LDX $az, Y
    &Chip::ldxZpY0,
    // $B7 -> LAX* $az, Y
    &Chip::laxZpY0,
    // $B8 -> CLV
    &Chip::clv0,
    // $B9 -> LDA $ahal, Y
    &Chip::ldaAbsY0,
    // $BA -> TSX
    &Chip::tsx0,
    // $BB -> ?
    &Chip::unofficial,
    // $BC -> LDY $ahal, X
    &Chip::ldyAbsX0,
    // $BD -> LDA $ahal, X
    &Chip::ldaAbsX0,
    // $BE -> LDX $ahal, Y
    &Chip::ldxAbsY0,
    // $BF -> LAX* $ahal, Y
    &Chip::laxAbsY0,
    // $C0 -> CPY #$da
    &Chip::cpyImm0,
    // $C1 -> CMP ($az, X)
    &Chip::cmpIndX0,
    // $C2 -> NOP*
    &Chip::nop0,
    // $C3 -> DCM* ($az, X)
    &Chip::dcmIndX0,
    // $C4 -> CPY $az
    &Chip::cpyZp0,
    // $C5 -> CMP $az
    &Chip::cmpZp0,
    // $C6 -> DEC $az
    &Chip::decZp0,
    // $C7 -> DCM* $az
    &Chip::dcmZp0,
    // $C8 -> INY
    &Chip::iny0,
    // $C9 -> CMP #$da
    &Chip::cmpImm0,
    // $CA -> DEX
    &Chip::dex0,
    // $CB -> ?
    &Chip::unofficial,
    // $CC -> CPY $ahal
    &Chip::cpyAbs0,
    // $CD -> CMP $ahal
    &Chip::cmpAbs0,
    // $CE -> DEC $ahal
    &Chip::decAbs0,
    // $CF -> DCM* $ahal
    &Chip::dcmAbs0,
    // $D0 -> BNE $of
    &Chip::bne0,
    // $D1 -> CMP ($az), Y
    &Chip::cmpIndY0,
    // $D2 -> KIL
    &Chip::kil,
    // $D3 -> DCM* ($az), Y
    &Chip::dcmIndY0,
    // $D4 -> NOP* $az, X
    &Chip::nopZpX0,
    // $D5 -> CMP $az, X
    &Chip::cmpZpX0,
    // $D6 -> DEC $az, X
    &Chip::decZpX0,
    // $D7 -> DCM* $az, X
    &Chip::dcmZpX0,
    // $D8 -> CLD
    &Chip::cld0,
    // $D9 -> CMP $ahal, Y
    &Chip::cmpAbsY0,
    // $DA -> NOP*
    &Chip::nop0,
    // $DB -> DCM* $ahal, Y
    &Chip::dcmAbsY0,
    // $DC -> NOP* $ahal, X
    &Chip::nopAbsX0,
    // $DD -> CMP $ahal, X
    &Chip::cmpAbsX0,
    // $DE -> DEC $ahal, X
    &Chip::decAbsX0,
    // $DF -> DCM* $ahal, X
    &Chip::dcmAbsX0,
    // $E0 -> CPX #$da
    &Chip::cpxImm0,
    // $E1 -> SBC ($az, X)
    &Chip::sbcIndX0,
    // $E2 -> NOP*
    &Chip::nop0,
    // $E3 -> INS* ($az, X)
    &Chip::insIndX0,
    // $E4 -> CPX $az
    &Chip::cpxZp0,
    // $E5 -> SBC $az
    &Chip::sbcZp0,
    // $E6 -> INC $az
    &Chip::incZp0,
    // $E7 -> INS* $az
    &Chip::insZp0,
    // $E8 -> INX
    &Chip::inx0,
    // $E9 -> SBC #$da
    &Chip::sbcImm0,
    // $EA -> NOP
    &Chip::nop0,
    // $EB -> SBC* #$da
    &Chip::sbcImm0,
    // $EC -> CPX $ahal
    &Chip::cpxAbs0,
    // $ED -> SBC $ahal
    &Chip::sbcAbs0,
    // $EE -> INC $ahal
    &Chip::incAbs0,
    // $EF -> INS* $ahal
    &Chip::insAbs0,
    // $F0 -> BEQ $of
    &Chip::beq0,
    // $F1 -> SBC ($az), Y
    &Chip::sbcIndY0,
    // $F2 -> KIL
    &Chip::kil,
    // $F3 -> INS* ($az), Y
    &Chip::insIndY0,
    // $F4 -> NOP* $az, X
    &Chip::nopZpX0,
    // $F5 -> SBC $az, X
    &Chip::sbcZpX0,
    // $F6 -> INC $az, X
    &Chip::incZpX0,
    // $F7 -> INS* $az, X
    &Chip::insZpX0,
    // $F8 -> SED
    &Chip::sed0,
    // $F9 -> SBC $ahal, Y
    &Chip::sbcAbsY0,
    // $FA -> NOP*
    &Chip::nop0,
    // $FB -> INS* $ahal, Y
    &Chip::insAbsY0,
    // $FC -> NOP* $ahal, X
    &Chip::nopAbsX0,
    // $FD -> SBC $ahal, X
    &Chip::sbcAbsX0,
    // $FE -> INC $ahal, X
    &Chip::incAbsX0,
    // $FF -> INS* $ahal, X
    &Chip::insAbsX0
};

template <class TBus, bool DecimalSupported>
const uint8_t Chip<TBus, DecimalSupported>::_interruptVectors[3][2] = {
    0xFA, 0xFF,     // NMI (Prior over IRQ/BRK but not Reset)
    0xFC, 0xFF,     // Reset (Higher priority)
    0xFE, 0xFF,     // IRQ, BRK (Lower priority)
};

template <class TBus, bool DecimalSupported>
const uint8_t Chip<TBus, DecimalSupported>::_stackPageNumber = 0x1;

#endif /* Cpu6502_Internal_Data_hpp */
