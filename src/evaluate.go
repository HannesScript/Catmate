package main

import (
	"log"
	"math/bits"
)

func evaluateBoard(b *Board) int {
	score := 0

	// Determine endgame by counting all non-king pieces.
	nonKingCount := bits.OnesCount64(uint64(b.wp)) +
		bits.OnesCount64(uint64(b.wn)) +
		bits.OnesCount64(uint64(b.wb)) +
		bits.OnesCount64(uint64(b.wr)) +
		bits.OnesCount64(uint64(b.wq)) +
		bits.OnesCount64(uint64(b.bp)) +
		bits.OnesCount64(uint64(b.bn)) +
		bits.OnesCount64(uint64(b.bb)) +
		bits.OnesCount64(uint64(b.br)) +
		bits.OnesCount64(uint64(b.bq))
	endgame := nonKingCount <= 10

	// Evaluate white pieces.
	whitePieces := []struct {
		bb    Bitboard
		value int
		table [64]int
	}{
		{b.wp, pieceValues["p"], pawnTable},
		{b.wn, pieceValues["n"], knightTable},
		{b.wb, pieceValues["b"], bishopTable},
		{b.wr, pieceValues["r"], rookTable},
		{b.wq, pieceValues["q"], queenTable},
	}

	for _, piece := range whitePieces {
		score += evaluatePiece(piece.bb, piece.value, piece.table)
	}
	score += evaluateKing(b.wk, endgame)

	// Evaluate black pieces (subtract their values).
	blackPieces := []struct {
		bb    Bitboard
		value int
		table [64]int
	}{
		{b.bp, pieceValues["p"], pawnTable},
		{b.bn, pieceValues["n"], knightTable},
		{b.bb, pieceValues["b"], bishopTable},
		{b.br, pieceValues["r"], rookTable},
		{b.bq, pieceValues["q"], queenTable},
	}

	for _, piece := range blackPieces {
		score -= evaluatePiece(piece.bb, piece.value, piece.table)
	}
	score -= evaluateKing(b.bk, endgame)

	return score
}

// evaluatePiece iterates over a bitboard for a given piece type and returns its material value plus table bonus.
func evaluatePiece(bb Bitboard, value int, table [64]int) int {
	score := 0
	for bb != 0 {
		sq := bits.TrailingZeros64(uint64(bb))
		score += value + table[sq]
		bb &= bb - 1 // clear the lowest set bit
	}
	return score
}

// evaluateKing returns the king's positional bonus.
// Logs a warning if the king bitboard is empty or holds more than one bit.
func evaluateKing(bb Bitboard, endgame bool) int {
	if bb == 0 {
		log.Println("Warning: king bitboard is empty")
		return 0
	}
	if bb&(bb-1) != 0 {
		log.Println("Warning: king bitboard has multiple bits set")
	}
	sq := bits.TrailingZeros64(uint64(bb))
	if endgame {
		return kingEndgameTable[sq]
	}
	return kingOpeningTable[sq]
}
