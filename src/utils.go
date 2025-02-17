package main

import "math/bits"

// IsKingInCheck returns true if the king for the given color is under attack.
func (b *Board) IsKingInCheck(color int) bool {
	var kingBB Bitboard
	if color == White {
		kingBB = b.wk
	} else {
		kingBB = b.bk
	}
	if kingBB == 0 {
		// Should never happen
		return false
	}
	kingSq := bits.TrailingZeros64(uint64(kingBB))
	// Set enemy color.
	enemy := Black
	if color == Black {
		enemy = White
	}
	// Create a temporary board with enemy's turn to generate moves.
	temp := *b
	temp.Turn = enemy
	moves := generateMovesBB(&temp)
	for _, m := range moves {
		if m.to == kingSq {
			return true
		}
	}
	return false
}

// isCheckmate returns true if the side to move has no legal moves and is in check.
func isCheckmate(b *Board) bool {
	// Only the side to move can be checkmated.
	if !b.IsKingInCheck(b.Turn) {
		return false
	}
	moves := generateMovesBB(b)
	for _, m := range moves {
		newBoard := applyMove(*b, m)
		if !newBoard.IsKingInCheck(b.Turn) {
			return false
		}
	}
	return true
}

// isStalemate returns true if the side to move is not in check but has no legal moves.
func isStalemate(b *Board) bool {
	if b.IsKingInCheck(b.Turn) {
		return false
	}
	moves := generateMovesBB(b)
	return len(moves) == 0
}

// isDraw returns true if the game is drawn (by stalemate or insufficient material).
func isDraw(b *Board) bool {
	if isStalemate(b) {
		return true
	}
	if isInsufficientMaterial(b) {
		return true
	}
	return false
}

// isInsufficientMaterial returns true if neither side has enough material to force a checkmate.
func isInsufficientMaterial(b *Board) bool {
	// Count non-king pieces.
	whiteNonKing := popCount(b.wp) + popCount(b.wn) + popCount(b.wb) + popCount(b.wr) + popCount(b.wq)
	blackNonKing := popCount(b.bp) + popCount(b.bn) + popCount(b.bb) + popCount(b.br) + popCount(b.bq)
	return whiteNonKing == 0 && blackNonKing == 0
}

// isCastling returns true if either king is in a castled position.
// (This implementation assumes that castling leaves the king on g1/c1 or g8/c8.)
func isCastling(b *Board) bool {
	if b.wk != 0 {
		kingSq := bits.TrailingZeros64(uint64(b.wk))
		// White king on g1 (6) or c1 (2)
		if kingSq == 6 || kingSq == 2 {
			return true
		}
	}
	if b.bk != 0 {
		kingSq := bits.TrailingZeros64(uint64(b.bk))
		// Black king on g8 (62) or c8 (58)
		if kingSq == 62 || kingSq == 58 {
			return true
		}
	}
	return false
}
