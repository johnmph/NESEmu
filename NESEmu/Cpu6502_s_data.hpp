//
//  Cpu6502_s_data.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 6/01/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Cpu6502_s_data_hpp
#define Cpu6502_s_data_hpp


/*
 
 MODE           SYNTAX
 ---------------------
                Implied
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
 
 */

template <class TBus>
const typename Cpu6502<TBus>::InstructionPipeline Cpu6502<TBus>::_instrPipelineFuncs[256] = {
    // $00 -> Brk
    &Cpu6502::brk0,
    // $01 -> ORA ($az, X)
    &Cpu6502::oraIndX0,
    // $02 -> KIL
    &Cpu6502::kil,
    // $03 -> ASO* ($az, X)
    &Cpu6502::asoIndX0,
    // $04 -> NOP* $az
    &Cpu6502::nopZp0,
    // $05 -> ORA $az
    &Cpu6502::oraZp0,
    // $06 -> ASL $az
    &Cpu6502::aslZp0,
    // $07 -> ASO* $az
    &Cpu6502::asoZp0,
    // $08 -> PHP
    &Cpu6502::php0,
    // $09 -> ORA #$da
    &Cpu6502::oraImm0,
    // $0A -> ASL (accumulator)
    &Cpu6502::aslImm0,
    // $0B -> ?
    &Cpu6502::unofficial,
    // $0C -> NOP* $ahal
    &Cpu6502::nopAbs0,
    // $0D -> ORA $ahal
    &Cpu6502::oraAbs0,
    // $0E -> ASL $ahal
    &Cpu6502::aslAbs0,
    // $0F -> ASO* $ahal
    &Cpu6502::asoAbs0,
    // $10 -> BPL $of
    &Cpu6502::bpl0,
    // $11 -> ORA ($az), Y
    &Cpu6502::oraIndY0,
    // $12 -> KIL
    &Cpu6502::kil,
    // $13 -> ASO* ($az), Y
    &Cpu6502::asoIndY0,
    // $14 -> NOP* $az, X
    &Cpu6502::nopZpX0,
    // $15 -> ORA $az, X
    &Cpu6502::oraZpX0,
    // $16 -> ASL $az, X
    &Cpu6502::aslZpX0,
    // $17 -> ASO* $az, X
    &Cpu6502::asoZpX0,
    // $18 -> CLC
    &Cpu6502::clc0,
    // $19 -> ORA $ahal, Y
    &Cpu6502::oraAbsY0,
    // $1A -> NOP*
    &Cpu6502::nop0,
    // $1B -> ASO* $ahal, Y
    &Cpu6502::asoAbsY0,
    // $1C -> NOP* $ahal, X
    &Cpu6502::nopAbsX0,
    // $1D -> ORA $ahal, X
    &Cpu6502::oraAbsX0,
    // $1E -> ASL $ahal, X
    &Cpu6502::aslAbsX0,
    // $1F -> ASO* $ahal, X
    &Cpu6502::asoAbsX0,
    // $20 -> JSR
    &Cpu6502::jsr0,
    // $21 -> AND ($az, X)
    &Cpu6502::andIndX0,
    // $22 -> KIL
    &Cpu6502::kil,
    // $23 -> RLA* ($az, X)
    &Cpu6502::rlaIndX0,
    // $24 -> BIT $az
    &Cpu6502::bitZp0,
    // $25 -> AND $az
    &Cpu6502::andZp0,
    // $26 -> ROL $az
    &Cpu6502::rolZp0,
    // $27 -> RLA* $az
    &Cpu6502::rlaZp0,
    // $28 -> PLP
    &Cpu6502::plp0,
    // $29 -> AND #$da
    &Cpu6502::andImm0,
    // $2A -> ROL (Accumulator)
    &Cpu6502::rolImm0,
    // $2B -> ?
    &Cpu6502::unofficial,
    // $2C -> BIT $ahal
    &Cpu6502::bitAbs0,
    // $2D -> AND $ahal
    &Cpu6502::andAbs0,
    // $2E -> ROL $ahal
    &Cpu6502::rolAbs0,
    // $2F -> RLA* $ahal
    &Cpu6502::rlaAbs0,
    // $30 -> BMI $of
    &Cpu6502::bmi0,
    // $31 -> AND ($az), Y
    &Cpu6502::andIndY0,
    // $32 -> KIL
    &Cpu6502::kil,
    // $33 -> RLA* ($az), Y
    &Cpu6502::rlaIndY0,
    // $34 -> NOP* $az, X
    &Cpu6502::nopZpX0,
    // $35 -> AND $az, X
    &Cpu6502::andZpX0,
    // $36 -> ROL $az, X
    &Cpu6502::rolZpX0,
    // $37 -> RLA* $az, X
    &Cpu6502::rlaZpX0,
    // $38 -> SEC
    &Cpu6502::sec0,
    // $39 -> AND $ahal, Y
    &Cpu6502::andAbsY0,
    // $3A -> NOP*
    &Cpu6502::nop0,
    // $3B -> RLA* $ahal, Y
    &Cpu6502::rlaAbsY0,
    // $3C -> NOP* $ahal, X
    &Cpu6502::nopAbsX0,
    // $3D -> AND $ahal, X
    &Cpu6502::andAbsX0,
    // $3E -> ROL $ahal, X
    &Cpu6502::rolAbsX0,
    // $3F -> RLA* $ahal, X
    &Cpu6502::rlaAbsX0,
    // $40 -> RTI
    &Cpu6502::rti0,
    // $41 -> EOR ($az, X)
    &Cpu6502::eorIndX0,
    // $42 -> KIL
    &Cpu6502::kil,
    // $43 -> LSE* ($az, X)
    &Cpu6502::lseIndX0,
    // $44 -> NOP* $az
    &Cpu6502::nopZp0,
    // $45 -> EOR $az
    &Cpu6502::eorZp0,
    // $46 -> LSR $az
    &Cpu6502::lsrZp0,
    // $47 -> LSE* $az
    &Cpu6502::lseZp0,
    // $48 -> PHA
    &Cpu6502::pha0,
    // $49 -> EOR #$da
    &Cpu6502::eorImm0,
    // $4A -> LSR (Accumulator)
    &Cpu6502::lsrImm0,
    // $4B -> ?
    &Cpu6502::unofficial,
    // $4C -> JMP $ahal
    &Cpu6502::jmpAbs0,
    // $4D -> EOR $ahal
    &Cpu6502::eorAbs0,
    // $4E -> LSR $ahal
    &Cpu6502::lsrAbs0,
    // $4F -> LSE* $ahal
    &Cpu6502::lseAbs0,
    // $50 -> BVC $of
    &Cpu6502::bvc0,
    // $51 -> EOR ($az), Y
    &Cpu6502::eorIndY0,
    // $52 -> KIL
    &Cpu6502::kil,
    // $53 -> LSE* ($az), Y
    &Cpu6502::lseIndY0,
    // $54 -> NOP* $az, X
    &Cpu6502::nopZpX0,
    // $55 -> EOR $az, X
    &Cpu6502::eorZpX0,
    // $56 -> LSR $az, X
    &Cpu6502::lsrZpX0,
    // $57 -> LSE* $az, X
    &Cpu6502::lseZpX0,
    // $58 -> CLI
    &Cpu6502::cli0,
    // $59 -> EOR $ahal, Y
    &Cpu6502::eorAbsY0,
    // $5A -> NOP*
    &Cpu6502::nop0,
    // $5B -> LSE* $ahal, Y
    &Cpu6502::lseAbsY0,
    // $5C -> NOP* $ahal, X
    &Cpu6502::nopAbsX0,
    // $5D -> EOR $ahal, X
    &Cpu6502::eorAbsX0,
    // $5E -> LSR $ahal, X
    &Cpu6502::lsrAbsX0,
    // $5F -> LSE* $ahal, X
    &Cpu6502::lseAbsX0,
    // $60 -> RTS
    &Cpu6502::rts0,
    // $61 -> ADC ($az, X)
    &Cpu6502::adcIndX0,
    // $62 -> KIL
    &Cpu6502::kil,
    // $63 -> RRA* ($az, X)
    &Cpu6502::rraIndX0,
    // $64 -> NOP* $az
    &Cpu6502::nopZp0,
    // $65 -> ADC $az
    &Cpu6502::adcZp0,
    // $66 -> ROR $az
    &Cpu6502::rorZp0,
    // $67 -> RRA* $az
    &Cpu6502::rraZp0,
    // $68 -> PLA
    &Cpu6502::pla0,
    // $69 -> ADC #$da
    &Cpu6502::adcImm0,
    // $6A -> ROR (Accumulator)
    &Cpu6502::rorImm0,
    // $6B -> ?
    &Cpu6502::unofficial,
    // $6C -> JMP ($ahal)
    &Cpu6502::jmpInd0,
    // $6D -> ADC $ahal
    &Cpu6502::adcAbs0,
    // $6E -> ROR $ahal
    &Cpu6502::rorAbs0,
    // $6F -> RRA* $ahal
    &Cpu6502::rraAbs0,
    // $70 -> BVS $of
    &Cpu6502::bvs0,
    // $71 -> ADC ($az), Y
    &Cpu6502::adcIndY0,
    // $72 -> KIL
    &Cpu6502::kil,
    // $73 -> RRA* ($az), Y
    &Cpu6502::rraIndY0,
    // $74 -> NOP* $az, X
    &Cpu6502::nopZpX0,
    // $75 -> ADC $az, X
    &Cpu6502::adcZpX0,
    // $76 -> ROR $az, X
    &Cpu6502::rorZpX0,
    // $77 -> RRA* $az, X
    &Cpu6502::rraZpX0,
    // $78 -> SEI
    &Cpu6502::sei0,
    // $79 -> ADC $ahal, Y
    &Cpu6502::adcAbsY0,
    // $7A -> NOP*
    &Cpu6502::nop0,
    // $7B -> RRA* $ahal, Y
    &Cpu6502::rraAbsY0,
    // $7C -> NOP* $ahal, X
    &Cpu6502::nopAbsX0,
    // $7D -> ADC $ahal, X
    &Cpu6502::adcAbsX0,
    // $7E -> ROR $ahal, X
    &Cpu6502::rorAbsX0,
    // $7F -> RRA* $ahal, X
    &Cpu6502::rraAbsX0,
    // $80 -> NOP* #$da
    &Cpu6502::nopImm0,
    // $81 -> STA ($az, X)
    &Cpu6502::staIndX0,
    // $82 -> NOP*
    &Cpu6502::nop0,
    // $83 -> AXS* ($az, X)
    &Cpu6502::axsIndX0,
    // $84 -> STY $az
    &Cpu6502::styZp0,
    // $85 -> STA $az
    &Cpu6502::staZp0,
    // $86 -> STX $az
    &Cpu6502::stxZp0,
    // $87 -> AXS* $az
    &Cpu6502::axsZp0,
    // $88 -> DEY
    &Cpu6502::dey0,
    // $89 -> NOP*
    &Cpu6502::nop0,
    // $8A -> TXA
    &Cpu6502::txa0,
    // $8B -> ?
    &Cpu6502::unofficial,
    // $8C -> STY $ahal
    &Cpu6502::styAbs0,
    // $8D -> STA $ahal
    &Cpu6502::staAbs0,
    // $8E -> STX $ahal
    &Cpu6502::stxAbs0,
    // $8F -> AXS* $ahal
    &Cpu6502::axsAbs0,
    // $90 -> BCC $of
    &Cpu6502::bcc0,
    // $91 -> STA ($az), Y
    &Cpu6502::staIndY0,
    // $92 -> KIL
    &Cpu6502::kil,
    // $93 -> ?
    &Cpu6502::unofficial,
    // $94 -> STY $az, X
    &Cpu6502::styZpX0,
    // $95 -> STA $az, X
    &Cpu6502::staZpX0,
    // $96 -> STX $az, Y
    &Cpu6502::stxZpY0,
    // $97 -> AXS* $az, Y
    &Cpu6502::axsZpY0,
    // $98 -> TYA
    &Cpu6502::tya0,
    // $99 -> STA $ahal, Y
    &Cpu6502::staAbsY0,
    // $9A -> TXS
    &Cpu6502::txs0,
    // $9B -> ?
    &Cpu6502::unofficial,
    // $9C -> ?
    &Cpu6502::unofficial,
    // $9D -> STA $ahal, X
    &Cpu6502::staAbsX0,
    // $9E -> ?
    &Cpu6502::unofficial,
    // $9F -> ?
    &Cpu6502::unofficial,
    // $A0 -> LDY #$da
    &Cpu6502::ldyImm0,
    // $A1 -> LDA ($az, X)
    &Cpu6502::ldaIndX0,
    // $A2 -> LDX #$da
    &Cpu6502::ldxImm0,
    // $A3 -> LAX* ($az, X)
    &Cpu6502::laxIndX0,
    // $A4 -> LDY $az
    &Cpu6502::ldyZp0,
    // $A5 -> LDA $az
    &Cpu6502::ldaZp0,
    // $A6 -> LDX $az
    &Cpu6502::ldxZp0,
    // $A7 -> LAX* $az
    &Cpu6502::laxZp0,
    // $A8 -> TAY
    &Cpu6502::tay0,
    // $A9 -> LDA #$da
    &Cpu6502::ldaImm0,
    // $AA -> TAX
    &Cpu6502::tax0,
    // $AB -> ?
    &Cpu6502::unofficial,
    // $AC -> LDY $ahal
    &Cpu6502::ldyAbs0,
    // $AD -> LDA $ahal
    &Cpu6502::ldaAbs0,
    // $AE -> LDX $ahal
    &Cpu6502::ldxAbs0,
    // $AF -> LAX* $ahal
    &Cpu6502::laxAbs0,
    // $B0 -> BCS $of
    &Cpu6502::bcs0,
    // $B1 -> LDA ($az), Y
    &Cpu6502::ldaIndY0,
    // $B2 -> KIL
    &Cpu6502::kil,
    // $B3 -> LAX ($zp), Y
    &Cpu6502::laxIndY0,
    // $B4 -> LDY $az, X
    &Cpu6502::ldyZpX0,
    // $B5 -> LDA $az, X
    &Cpu6502::ldaZpX0,
    // $B6 -> LDX $az, Y
    &Cpu6502::ldxZpY0,
    // $B7 -> LAX* $az, Y
    &Cpu6502::laxZpY0,
    // $B8 -> CLV
    &Cpu6502::clv0,
    // $B9 -> LDA $ahal, Y
    &Cpu6502::ldaAbsY0,
    // $BA -> TSX
    &Cpu6502::tsx0,
    // $BB -> ?
    &Cpu6502::unofficial,
    // $BC -> LDY $ahal, X
    &Cpu6502::ldyAbsX0,
    // $BD -> LDA $ahal, X
    &Cpu6502::ldaAbsX0,
    // $BE -> LDX $ahal, Y
    &Cpu6502::ldxAbsY0,
    // $BF -> LAX* $ahal, Y
    &Cpu6502::laxAbsY0,
    // $C0 -> CPY #$da
    &Cpu6502::cpyImm0,
    // $C1 -> CMP ($az, X)
    &Cpu6502::cmpIndX0,
    // $C2 -> NOP*
    &Cpu6502::nop0,
    // $C3 -> DCP* ($az, X)
    &Cpu6502::dcpIndX0,
    // $C4 -> CPY $az
    &Cpu6502::cpyZp0,
    // $C5 -> CMP $az
    &Cpu6502::cmpZp0,
    // $C6 -> DEC $az
    &Cpu6502::decZp0,
    // $C7 -> DCP* $az
    &Cpu6502::dcpZp0,
    // $C8 -> INY
    &Cpu6502::iny0,
    // $C9 -> CMP #$da
    &Cpu6502::cmpImm0,
    // $CA -> DEX
    &Cpu6502::dex0,
    // $CB -> ?
    &Cpu6502::unofficial,
    // $CC -> CPY $ahal
    &Cpu6502::cpyAbs0,
    // $CD -> CMP $ahal
    &Cpu6502::cmpAbs0,
    // $CE -> DEC $ahal
    &Cpu6502::decAbs0,
    // $CF -> DCP* $ahal
    &Cpu6502::dcpAbs0,
    // $D0 -> BNE $of
    &Cpu6502::bne0,
    // $D1 -> CMP ($az), Y
    &Cpu6502::cmpIndY0,
    // $D2 -> KIL
    &Cpu6502::kil,
    // $D3 -> DCP* ($az), Y
    &Cpu6502::dcpIndY0,
    // $D4 -> NOP* $az, X
    &Cpu6502::nopZpX0,
    // $D5 -> CMP $az, X
    &Cpu6502::cmpZpX0,
    // $D6 -> DEC $az, X
    &Cpu6502::decZpX0,
    // $D7 -> DCP* $az, X
    &Cpu6502::dcpZpX0,
    // $D8 -> CLD
    &Cpu6502::cld0,
    // $D9 -> CMP $ahal, Y
    &Cpu6502::cmpAbsY0,
    // $DA -> NOP*
    &Cpu6502::nop0,
    // $DB -> DCP* $ahal, Y
    &Cpu6502::dcpAbsY0,
    // $DC -> NOP* $ahal, X
    &Cpu6502::nopAbsX0,
    // $DD -> CMP $ahal, X
    &Cpu6502::cmpAbsX0,
    // $DE -> DEC $ahal, X
    &Cpu6502::decAbsX0,
    // $DF -> DCP* $ahal, X
    &Cpu6502::dcpAbsX0,
    // $E0 -> CPX #$da
    &Cpu6502::cpxImm0,
    // $E1 -> SBC ($az, X)
    &Cpu6502::sbcIndX0,
    // $E2 -> NOP*
    &Cpu6502::nop0,
    // $E3 -> INS* ($az, X)
    &Cpu6502::insIndX0,
    // $E4 -> CPX $az
    &Cpu6502::cpxZp0,
    // $E5 -> SBC $az
    &Cpu6502::sbcZp0,
    // $E6 -> INC $az
    &Cpu6502::incZp0,
    // $E7 -> INS* $az
    &Cpu6502::insZp0,
    // $E8 -> INX
    &Cpu6502::inx0,
    // $E9 -> SBC #$da
    &Cpu6502::sbcImm0,
    // $EA -> NOP
    &Cpu6502::nop0,
    // $EB -> SBC* #$da
    &Cpu6502::sbcImm0,
    // $EC -> CPX $ahal
    &Cpu6502::cpxAbs0,
    // $ED -> SBC $ahal
    &Cpu6502::sbcAbs0,
    // $EE -> INC $ahal
    &Cpu6502::incAbs0,
    // $EF -> INS* $ahal
    &Cpu6502::insAbs0,
    // $F0 -> BEQ $of
    &Cpu6502::beq0,
    // $F1 -> SBC ($az), Y
    &Cpu6502::sbcIndY0,
    // $F2 -> KIL
    &Cpu6502::kil,
    // $F3 -> INS* ($az), Y
    &Cpu6502::insIndY0,
    // $F4 -> NOP* $az, X
    &Cpu6502::nopZpX0,
    // $F5 -> SBC $az, X
    &Cpu6502::sbcZpX0,
    // $F6 -> INC $az, X
    &Cpu6502::incZpX0,
    // $F7 -> INS* $az, X
    &Cpu6502::insZpX0,
    // $F8 -> SED
    &Cpu6502::sed0,
    // $F9 -> SBC $ahal, Y
    &Cpu6502::sbcAbsY0,
    // $FA -> NOP*
    &Cpu6502::nop0,
    // $FB -> INS* $ahal, Y
    &Cpu6502::insAbsY0,
    // $FC -> NOP* $ahal, X
    &Cpu6502::nopAbsX0,
    // $FD -> SBC $ahal, X
    &Cpu6502::sbcAbsX0,
    // $FE -> INC $ahal, X
    &Cpu6502::incAbsX0,
    // $FF -> INS* $ahal, X
    &Cpu6502::insAbsX0
};

template <class TBus>
const uint8_t Cpu6502<TBus>::_interruptVectors[3][2] = {
    0xFA, 0xFF,     // NMI (Prior over IRQ/BRK but not Reset)
    0xFC, 0xFF,     // Reset (Higher priority)
    0xFE, 0xFF,     // IRQ, BRK (Lower priority)
};

template <class TBus>
const uint8_t Cpu6502<TBus>::_stackPageNumber = 0x1;

#endif /* Cpu6502_s_data_hpp */
