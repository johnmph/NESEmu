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
    // Start / Reset (No opcode, electronic line), this instruction continues with brk
    &Cpu6502::startLow, &Cpu6502::startHigh,
    // $00 -> Brk
    &Cpu6502::brk0, &Cpu6502::brk1, &Cpu6502::brk2, &Cpu6502::brk3, &Cpu6502::brk4, &Cpu6502::brk5, &Cpu6502::brk6,
    // $01 -> ORA ($az, X)
    &Cpu6502::zeroPagePreIndexedIndirect0, &Cpu6502::zeroPagePreIndexedIndirect1, &Cpu6502::zeroPagePreIndexedIndirect2, &Cpu6502::zeroPagePreIndexedIndirect3, &Cpu6502::zeroPagePreIndexedIndirectLoad, &Cpu6502::ora0, &Cpu6502::ora1,
    // $02 -> ?
    // $03 -> ?
    // $04 -> ?
    // $05 -> ORA $az
    // $06 -> ASL $az
    // $07 -> ?
    // $08 -> PHP
    // $09 -> ORA #$da
    // $0A -> ASL (accumulator)
    // $0B -> ?
    // $0C -> ?
    // $0D -> ORA $ahal
    // $0E -> ASL $ahal
    // $0F -> ?
    // $10 -> BPL $of
    // $11 -> ORA ($az), Y
    // $12 -> ?
    // $13 -> ?
    // $14 -> ?
    // $15 -> ORA $az, X
    // $16 -> ASL $az, X
    // $17 -> ?
    // $18 -> CLC
    // $19 -> ORA $ahal, Y
    // $1A -> ?
    // $1B -> ?
    // $1C -> ?
    // $1D -> ORA $ahal, X
    // $1E -> ASL $ahal, X
    // $1F -> ?
    // $20 -> JSR
    // $21 -> AND ($az, X)
    // $22 -> ?
    // $23 -> ?
    // $24 -> BIT $az
    // $25 -> AND $az
    // $26 -> ROL $az
    // $27 -> ?
    // $28 -> PLP
    // $29 -> AND #$da
    // $2A -> ROL (Accumulator)
    // $2B -> ?
    // $2C -> BIT $ahal
    // $2D -> AND $ahal
    // $2E -> ROL $ahal
    // $2F -> ?
    // $30 -> BMI $of
    // $31 -> AND ($az), Y
    // $32 -> ?
    // $33 -> ?
    // $34 -> ?
    // $35 -> AND $az, X
    // $36 -> ROL $az, X
    // $37 -> ?
    // $38 -> SEC
    // $39 -> AND $ahal, Y
    // $3A -> ?
    // $3B -> ?
    // $3C -> ?
    // $3D -> AND $ahal, X
    // $3E -> ROL $ahal, X
    // $3F -> ?
    // $40 -> RTI
    // $41 -> EOR ($az, X)
    // $42 -> ?
    // $43 -> ?
    // $44 -> ?
    // $45 -> EOR $az
    // $46 -> LSR $az
    // $47 -> ?
    // $48 -> PHA
    // $49 -> EOR #$da
    // $4A -> LSR (Accumulator)
    // $4B -> ?
    // $4C -> JMP $ahal
    // $4D -> EOR $ahal
    // $4E -> LSR $ahal
    // $4F -> ?
    // $50 -> BVC $of
    // $51 -> EOR ($az), Y
    // $52 -> ?
    // $53 -> ?
    // $54 -> ?
    // $55 -> EOR $az, X
    // $56 -> LSR $az, X
    // $57 -> ?
    // $58 -> CLI
    // $59 -> EOR $ahal, Y
    // $5A -> ?
    // $5B -> ?
    // $5C -> ?
    // $5D -> EOR $ahal, X
    // $5E -> LSR $ahal, X
    // $5F -> ?
    // $60 -> RTS
    // $61 -> ADC ($az, X)
    // $62 -> ?
    // $63 -> ?
    // $64 -> ?
    // $65 -> ADC $az
    // $66 -> ROR $az
    // $67 -> ?
    // $68 -> PLA
    // $69 -> ADC #$da
    // $6A -> ROR (Accumulator)
    // $6B -> ?
    // $6C -> JMP ($ahal)
    // $6D -> ADC $ahal
    // $6E -> ROR $ahal
    // $6F -> ?
    // $70 -> BVS $of
    // $71 -> ADC ($az), Y
    // $72 -> ?
    // $73 -> ?
    // $74 -> ?
    // $75 -> ADC $az, X
    // $76 -> ROR $az, X
    // $77 -> ?
    // $78 -> SEI
    // $79 -> ADC $ahal, Y
    // $7A -> ?
    // $7B -> ?
    // $7C -> ?
    // $7D -> ADC $ahal, X
    // $7E -> ROR $ahal, X
    // $7F -> ?
    // $80 -> ?
    // $81 -> STA ($az, X)
    // $82 -> ?
    // $83 -> ?
    // $84 -> STY $az
    // $85 -> STA $az
    // $86 -> STX $az
    // $87 -> ?
    // $88 -> DEY
    // $89 -> ?
    // $8A -> TXA
    // $8B -> ?
    // $8C -> STY $ahal
    // $8D -> STA $ahal
    // $8E -> STX $ahal
    // $8F -> ?
    // $90 -> BCC $of
    // $91 -> STA ($az), Y
    // $92 -> ?
    // $93 -> ?
    // $94 -> STY $az, X
    // $95 -> STA $az, X
    // $96 -> STX $az, Y
    // $97 -> ?
    // $98 -> TYA
    // $99 -> STA $ahal, Y
    // $9A -> TXS
    // $9B -> ?
    // $9C -> ?
    // $9D -> STA $ahal, X
    // $9E -> ?
    // $9F -> ?
    // $A0 -> LDY #$da
    // $A1 -> LDA ($az, X)
    // $A2 -> LDX #$da
    // $A3 -> ?
    // $A4 -> LDY $az
    // $A5 -> LDA $az
    // $A6 -> LDX $az
    // $A7 -> ?
    // $A8 -> TAY
    // $A9 -> LDA #$da
    // $AA -> TAX
    // $AB -> ?
    // $AC -> LDY $ahal
    // $AD -> LDA $ahal
    // $AE -> LDX $ahal
    // $AF -> ?
    // $B0 -> BCS $of
    // $B1 -> LDA ($az), Y
    // $B2 -> ?
    // $B3 -> ?
    // $B4 -> LDY $az, X
    // $B5 -> LDA $az, X
    // $B6 -> LDX $az, X
    // $B7 -> ?
    // $B8 -> CLV
    // $B9 -> LDA $ahal, Y
    // $BA -> TSX
    // $BB -> ?
    // $BC -> LDY $ahal, X
    // $BD -> LDA $ahal, X
    // $BE -> LDX $ahal, X
    // $BF -> ?
    // $C0 -> CPY #$da
    // $C1 -> CMP ($az, X)
    // $C2 -> ?
    // $C3 -> ?
    // $C4 -> CPY $az
    // $C5 -> CMP $az
    // $C6 -> DEC $az
    // $C7 -> ?
    // $C8 -> INY
    // $C9 -> CMP #$da
    // $CA -> DEX
    // $CB -> ?
    // $CC -> CPY $ahal
    // $CD -> CMP $ahal
    // $CE -> DEC $ahal
    // $CF -> ?
    // $D0 -> BNE $of
    // $D1 -> CMP ($az), Y
    // $D2 -> ?
    // $D3 -> ?
    // $D4 -> ?
    // $D5 -> CMP $az, X
    // $D6 -> DEC $az, X
    // $D7 -> ?
    // $D8 -> CLD
    // $D9 -> CMP $ahal, Y
    // $DA -> ?
    // $DB -> ?
    // $DC -> ?
    // $DD -> CMP $ahal, X
    // $DE -> DEC $ahal, X
    // $DF -> ?
    // $E0 -> CPX #$da
    // $E1 -> SBC ($az, X)
    // $E2 -> ?
    // $E3 -> ?
    // $E4 -> CPX $az
    // $E5 -> SBC $az
    // $E6 -> INC $az
    // $E7 -> ?
    // $E8 -> INX
    // $E9 -> SBC #$da
    // $EA -> NOP
    // $EB -> ?
    // $EC -> CPX $ahal
    // $ED -> SBC $ahal
    // $EE -> INC $ahal
    // $EF -> ?
    // $F0 -> BEQ $of
    // $F1 -> SBC ($az), Y
    // $F2 -> ?
    // $F3 -> ?
    // $F4 -> ?
    // $F5 -> SBC $az, X
    // $F6 -> INC $az, X
    // $F7 -> ?
    // $F8 -> SED
    // $F9 -> SBC $ahal, Y
    // $FA -> ?
    // $FB -> ?
    // $FC -> ?
    // $FD -> SBC $ahal, X
    // $FE -> INC $ahal, X
    // $FF -> ?
    
    
    
    &Cpu6502::implied, &Cpu6502::clv,                                                                // Test implied
    &Cpu6502::immediate, &Cpu6502::lda,                                                              // Test immediate read
    &Cpu6502::absolute0, &Cpu6502::absolute1, &Cpu6502::absoluteLoad, &Cpu6502::lda,                 // Test absolute read
    &Cpu6502::absolute0, &Cpu6502::absolute1, &Cpu6502::staAbsolute, &Cpu6502::finishInstruction,    // Test absolute write
    &Cpu6502::zeroPage0, &Cpu6502::zeroPageLoad, &Cpu6502::lda,                                      // Test zero page read
    &Cpu6502::zeroPage0, &Cpu6502::staZeroPage, &Cpu6502::finishInstruction,                         // Test zero page write
    &Cpu6502::relative, &Cpu6502::bcs, &Cpu6502::relativeBranch0, &Cpu6502::relativeBranch1,         // Test relative branch    // TODO: a tester apres quand fini alu
    &Cpu6502::absoluteIndexed0, &Cpu6502::absoluteIndexedX1, &Cpu6502::absoluteIndexedLoad0, &Cpu6502::absoluteIndexedLoad1, &Cpu6502::lda,                 // Test absolute indexed X read
    &Cpu6502::absoluteIndexed0, &Cpu6502::absoluteIndexedY1, &Cpu6502::staAbsoluteIndexed, &Cpu6502::absoluteIndexedStore1, &Cpu6502::finishInstruction,  // Test absolute indexed Y write
    &Cpu6502::zeroPageIndexed0, &Cpu6502::zeroPageIndexedX1, &Cpu6502::zeroPageIndexedLoad, &Cpu6502::lda, // Test zero page indexed X read
    &Cpu6502::zeroPageIndexed0, &Cpu6502::zeroPageIndexedY1, &Cpu6502::staZeroPageIndexed, &Cpu6502::finishInstruction, // Test zero page indexed Y write
};

// 256 opcodes
template <class TBus>
const int Cpu6502<TBus>::_instrPipelineStartIndexes[256] = {
    2, 9, 11, 13, 17, 20, 23, 27, 32, 37, 41, 46, 53, 62, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
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
