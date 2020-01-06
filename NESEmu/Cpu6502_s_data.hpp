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
    &Cpu6502::zeroPagePreIndexedIndirect0, &Cpu6502::zeroPagePreIndexedIndirect1, &Cpu6502::zeroPagePreIndexedIndirect2, &Cpu6502::zeroPagePreIndexedIndirect3, &Cpu6502::zeroPagePreIndexedIndirectLoad, &Cpu6502::ora,
    // $02 -> ?
    &Cpu6502::finishInstruction,
    // $03 -> ?
    &Cpu6502::finishInstruction,
    // $04 -> ?
    &Cpu6502::finishInstruction,
    // $05 -> ORA $az
    &Cpu6502::zeroPage, &Cpu6502::zeroPageLoad, &Cpu6502::ora,
    // $06 -> ASL $az
    &Cpu6502::zeroPage, &Cpu6502::zeroPageLoad, &Cpu6502::asl0, &Cpu6502::asl1ZeroPage, &Cpu6502::finishInstruction,
    // $07 -> ?
    &Cpu6502::finishInstruction,
    // $08 -> PHP
    // $09 -> ORA #$da
    // $0A -> ASL (accumulator)
    &Cpu6502::aslAccumulator0, &Cpu6502::aslAccumulator1,
    // $0B -> ?
    &Cpu6502::finishInstruction,
    // $0C -> ?
    &Cpu6502::finishInstruction,
    // $0D -> ORA $ahal
    // $0E -> ASL $ahal
    &Cpu6502::absolute0, &Cpu6502::absolute1, &Cpu6502::absoluteLoad, &Cpu6502::asl0, &Cpu6502::asl1Absolute, &Cpu6502::finishInstruction,
    // $0F -> ?
    &Cpu6502::finishInstruction,
    // $10 -> BPL $of
    // $11 -> ORA ($az), Y
    // $12 -> ?
    &Cpu6502::finishInstruction,
    // $13 -> ?
    &Cpu6502::finishInstruction,
    // $14 -> ?
    &Cpu6502::finishInstruction,
    // $15 -> ORA $az, X
    // $16 -> ASL $az, X
    &Cpu6502::zeroPageIndexed0, &Cpu6502::zeroPageIndexedX1, &Cpu6502::zeroPageIndexedLoad, &Cpu6502::asl0, &Cpu6502::asl1ZeroPageIndexed, &Cpu6502::finishInstruction,
    // $17 -> ?
    &Cpu6502::finishInstruction,
    // $18 -> CLC
    // $19 -> ORA $ahal, Y
    // $1A -> ?
    &Cpu6502::finishInstruction,
    // $1B -> ?
    &Cpu6502::finishInstruction,
    // $1C -> ?
    &Cpu6502::finishInstruction,
    // $1D -> ORA $ahal, X
    // $1E -> ASL $ahal, X
    &Cpu6502::absoluteIndexed0, &Cpu6502::absoluteIndexedX1, &Cpu6502::absoluteIndexedLoad0, &Cpu6502::absoluteIndexedLoad1, &Cpu6502::asl0, &Cpu6502::asl1AbsoluteIndexed, &Cpu6502::absoluteIndexedStore1, &Cpu6502::finishInstruction,//TODO: probleme (1 cycle de trop)
    // $1F -> ?
    &Cpu6502::finishInstruction,
    // $20 -> JSR
    // $21 -> AND ($az, X)
    // $22 -> ?
    &Cpu6502::finishInstruction,
    // $23 -> ?
    &Cpu6502::finishInstruction,
    // $24 -> BIT $az
    // $25 -> AND $az
    // $26 -> ROL $az
    // $27 -> ?
    &Cpu6502::finishInstruction,
    // $28 -> PLP
    // $29 -> AND #$da
    // $2A -> ROL (Accumulator)
    // $2B -> ?
    &Cpu6502::finishInstruction,
    // $2C -> BIT $ahal
    // $2D -> AND $ahal
    // $2E -> ROL $ahal
    // $2F -> ?
    &Cpu6502::finishInstruction,
    // $30 -> BMI $of
    // $31 -> AND ($az), Y
    // $32 -> ?
    &Cpu6502::finishInstruction,
    // $33 -> ?
    &Cpu6502::finishInstruction,
    // $34 -> ?
    &Cpu6502::finishInstruction,
    // $35 -> AND $az, X
    // $36 -> ROL $az, X
    // $37 -> ?
    &Cpu6502::finishInstruction,
    // $38 -> SEC
    // $39 -> AND $ahal, Y
    // $3A -> ?
    &Cpu6502::finishInstruction,
    // $3B -> ?
    &Cpu6502::finishInstruction,
    // $3C -> ?
    &Cpu6502::finishInstruction,
    // $3D -> AND $ahal, X
    // $3E -> ROL $ahal, X
    // $3F -> ?
    &Cpu6502::finishInstruction,
    // $40 -> RTI
    // $41 -> EOR ($az, X)
    // $42 -> ?
    &Cpu6502::finishInstruction,
    // $43 -> ?
    &Cpu6502::finishInstruction,
    // $44 -> ?
    &Cpu6502::finishInstruction,
    // $45 -> EOR $az
    // $46 -> LSR $az
    // $47 -> ?
    &Cpu6502::finishInstruction,
    // $48 -> PHA
    // $49 -> EOR #$da
    // $4A -> LSR (Accumulator)
    // $4B -> ?
    &Cpu6502::finishInstruction,
    // $4C -> JMP $ahal
    // $4D -> EOR $ahal
    // $4E -> LSR $ahal
    // $4F -> ?
    &Cpu6502::finishInstruction,
    // $50 -> BVC $of
    // $51 -> EOR ($az), Y
    // $52 -> ?
    &Cpu6502::finishInstruction,
    // $53 -> ?
    &Cpu6502::finishInstruction,
    // $54 -> ?
    &Cpu6502::finishInstruction,
    // $55 -> EOR $az, X
    // $56 -> LSR $az, X
    // $57 -> ?
    &Cpu6502::finishInstruction,
    // $58 -> CLI
    // $59 -> EOR $ahal, Y
    // $5A -> ?
    &Cpu6502::finishInstruction,
    // $5B -> ?
    &Cpu6502::finishInstruction,
    // $5C -> ?
    &Cpu6502::finishInstruction,
    // $5D -> EOR $ahal, X
    // $5E -> LSR $ahal, X
    // $5F -> ?
    &Cpu6502::finishInstruction,
    // $60 -> RTS
    // $61 -> ADC ($az, X)
    // $62 -> ?
    &Cpu6502::finishInstruction,
    // $63 -> ?
    &Cpu6502::finishInstruction,
    // $64 -> ?
    &Cpu6502::finishInstruction,
    // $65 -> ADC $az
    // $66 -> ROR $az
    // $67 -> ?
    &Cpu6502::finishInstruction,
    // $68 -> PLA
    // $69 -> ADC #$da
    // $6A -> ROR (Accumulator)
    // $6B -> ?
    &Cpu6502::finishInstruction,
    // $6C -> JMP ($ahal)
    // $6D -> ADC $ahal
    // $6E -> ROR $ahal
    // $6F -> ?
    &Cpu6502::finishInstruction,
    // $70 -> BVS $of
    // $71 -> ADC ($az), Y
    // $72 -> ?
    &Cpu6502::finishInstruction,
    // $73 -> ?
    &Cpu6502::finishInstruction,
    // $74 -> ?
    &Cpu6502::finishInstruction,
    // $75 -> ADC $az, X
    // $76 -> ROR $az, X
    // $77 -> ?
    &Cpu6502::finishInstruction,
    // $78 -> SEI
    // $79 -> ADC $ahal, Y
    // $7A -> ?
    &Cpu6502::finishInstruction,
    // $7B -> ?
    &Cpu6502::finishInstruction,
    // $7C -> ?
    &Cpu6502::finishInstruction,
    // $7D -> ADC $ahal, X
    // $7E -> ROR $ahal, X
    // $7F -> ?
    &Cpu6502::finishInstruction,
    // $80 -> ?
    &Cpu6502::finishInstruction,
    // $81 -> STA ($az, X)
    // $82 -> ?
    &Cpu6502::finishInstruction,
    // $83 -> ?
    &Cpu6502::finishInstruction,
    // $84 -> STY $az
    // $85 -> STA $az
    &Cpu6502::zeroPage, &Cpu6502::staZeroPage, &Cpu6502::finishInstruction,
    // $86 -> STX $az
    // $87 -> ?
    &Cpu6502::finishInstruction,
    // $88 -> DEY
    // $89 -> ?
    &Cpu6502::finishInstruction,
    // $8A -> TXA
    // $8B -> ?
    &Cpu6502::finishInstruction,
    // $8C -> STY $ahal
    // $8D -> STA $ahal
    &Cpu6502::absolute0, &Cpu6502::absolute1, &Cpu6502::staAbsolute, &Cpu6502::finishInstruction,
    // $8E -> STX $ahal
    // $8F -> ?
    &Cpu6502::finishInstruction,
    // $90 -> BCC $of
    // $91 -> STA ($az), Y
    // $92 -> ?
    &Cpu6502::finishInstruction,
    // $93 -> ?
    &Cpu6502::finishInstruction,
    // $94 -> STY $az, X
    // $95 -> STA $az, X
    &Cpu6502::zeroPageIndexed0, &Cpu6502::zeroPageIndexedX1, &Cpu6502::staZeroPageIndexed, &Cpu6502::finishInstruction,
    // $96 -> STX $az, Y
    // $97 -> ?
    &Cpu6502::finishInstruction,
    // $98 -> TYA
    // $99 -> STA $ahal, Y
    &Cpu6502::absoluteIndexed0, &Cpu6502::absoluteIndexedY1, &Cpu6502::staAbsoluteIndexed, &Cpu6502::absoluteIndexedStore1, &Cpu6502::finishInstruction,
    // $9A -> TXS
    // $9B -> ?
    &Cpu6502::finishInstruction,
    // $9C -> ?
    &Cpu6502::finishInstruction,
    // $9D -> STA $ahal, X
    &Cpu6502::absoluteIndexed0, &Cpu6502::absoluteIndexedX1, &Cpu6502::staAbsoluteIndexed, &Cpu6502::absoluteIndexedStore1, &Cpu6502::finishInstruction,
    // $9E -> ?
    &Cpu6502::finishInstruction,
    // $9F -> ?
    &Cpu6502::finishInstruction,
    // $A0 -> LDY #$da
    // $A1 -> LDA ($az, X)
    // $A2 -> LDX #$da
    // $A3 -> ?
    &Cpu6502::finishInstruction,
    // $A4 -> LDY $az
    // $A5 -> LDA $az
    &Cpu6502::zeroPage, &Cpu6502::zeroPageLoad, &Cpu6502::lda,
    // $A6 -> LDX $az
    // $A7 -> ?
    &Cpu6502::finishInstruction,
    // $A8 -> TAY
    // $A9 -> LDA #$da
    &Cpu6502::immediate, &Cpu6502::lda,
    // $AA -> TAX
    // $AB -> ?
    &Cpu6502::finishInstruction,
    // $AC -> LDY $ahal
    // $AD -> LDA $ahal
    &Cpu6502::absolute0, &Cpu6502::absolute1, &Cpu6502::absoluteLoad, &Cpu6502::lda,
    // $AE -> LDX $ahal
    // $AF -> ?
    &Cpu6502::finishInstruction,
    // $B0 -> BCS $of
    &Cpu6502::relative, &Cpu6502::bcs, &Cpu6502::relativeBranch0, &Cpu6502::relativeBranch1,
    // $B1 -> LDA ($az), Y
    // $B2 -> ?
    &Cpu6502::finishInstruction,
    // $B3 -> ?
    &Cpu6502::finishInstruction,
    // $B4 -> LDY $az, X
    // $B5 -> LDA $az, X
    &Cpu6502::zeroPageIndexed0, &Cpu6502::zeroPageIndexedX1, &Cpu6502::zeroPageIndexedLoad, &Cpu6502::lda,
    // $B6 -> LDX $az, X
    // $B7 -> ?
    &Cpu6502::finishInstruction,
    // $B8 -> CLV
    &Cpu6502::implied, &Cpu6502::clv,
    // $B9 -> LDA $ahal, Y
    &Cpu6502::absoluteIndexed0, &Cpu6502::absoluteIndexedY1, &Cpu6502::absoluteIndexedLoad0, &Cpu6502::absoluteIndexedLoad1, &Cpu6502::lda,
    // $BA -> TSX
    // $BB -> ?
    &Cpu6502::finishInstruction,
    // $BC -> LDY $ahal, X
    // $BD -> LDA $ahal, X
    &Cpu6502::absoluteIndexed0, &Cpu6502::absoluteIndexedX1, &Cpu6502::absoluteIndexedLoad0, &Cpu6502::absoluteIndexedLoad1, &Cpu6502::lda,
    // $BE -> LDX $ahal, X
    // $BF -> ?
    &Cpu6502::finishInstruction,
    // $C0 -> CPY #$da
    // $C1 -> CMP ($az, X)
    // $C2 -> ?
    &Cpu6502::finishInstruction,
    // $C3 -> ?
    &Cpu6502::finishInstruction,
    // $C4 -> CPY $az
    // $C5 -> CMP $az
    // $C6 -> DEC $az
    // $C7 -> ?
    &Cpu6502::finishInstruction,
    // $C8 -> INY
    // $C9 -> CMP #$da
    // $CA -> DEX
    // $CB -> ?
    &Cpu6502::finishInstruction,
    // $CC -> CPY $ahal
    // $CD -> CMP $ahal
    // $CE -> DEC $ahal
    // $CF -> ?
    &Cpu6502::finishInstruction,
    // $D0 -> BNE $of
    // $D1 -> CMP ($az), Y
    // $D2 -> ?
    &Cpu6502::finishInstruction,
    // $D3 -> ?
    &Cpu6502::finishInstruction,
    // $D4 -> ?
    &Cpu6502::finishInstruction,
    // $D5 -> CMP $az, X
    // $D6 -> DEC $az, X
    // $D7 -> ?
    &Cpu6502::finishInstruction,
    // $D8 -> CLD
    // $D9 -> CMP $ahal, Y
    // $DA -> ?
    &Cpu6502::finishInstruction,
    // $DB -> ?
    &Cpu6502::finishInstruction,
    // $DC -> ?
    &Cpu6502::finishInstruction,
    // $DD -> CMP $ahal, X
    // $DE -> DEC $ahal, X
    // $DF -> ?
    &Cpu6502::finishInstruction,
    // $E0 -> CPX #$da
    // $E1 -> SBC ($az, X)
    // $E2 -> ?
    &Cpu6502::finishInstruction,
    // $E3 -> ?
    &Cpu6502::finishInstruction,
    // $E4 -> CPX $az
    // $E5 -> SBC $az
    // $E6 -> INC $az
    // $E7 -> ?
    &Cpu6502::finishInstruction,
    // $E8 -> INX
    // $E9 -> SBC #$da
    // $EA -> NOP
    // $EB -> ?
    &Cpu6502::finishInstruction,
    // $EC -> CPX $ahal
    // $ED -> SBC $ahal
    // $EE -> INC $ahal
    // $EF -> ?
    &Cpu6502::finishInstruction,
    // $F0 -> BEQ $of
    // $F1 -> SBC ($az), Y
    // $F2 -> ?
    &Cpu6502::finishInstruction,
    // $F3 -> ?
    &Cpu6502::finishInstruction,
    // $F4 -> ?
    &Cpu6502::finishInstruction,
    // $F5 -> SBC $az, X
    // $F6 -> INC $az, X
    // $F7 -> ?
    &Cpu6502::finishInstruction,
    // $F8 -> SED
    // $F9 -> SBC $ahal, Y
    // $FA -> ?
    &Cpu6502::finishInstruction,
    // $FB -> ?
    &Cpu6502::finishInstruction,
    // $FC -> ?
    &Cpu6502::finishInstruction,
    // $FD -> SBC $ahal, X
    // $FE -> INC $ahal, X
    // $FF -> ?
    &Cpu6502::finishInstruction
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
