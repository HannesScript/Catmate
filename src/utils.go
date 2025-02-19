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

// If any piece (of color) is attacking 2 or more pieces, this function returns the number of pieces attacked
// and the types of the pieces attacked. If no such piece exists, it returns 0 and a nil slice.
// This simple implementation only checks for forks on the next move.
func isFork(b *Board, color int) (int, []string) {
	// Map enemy piece types to their bitboards (excluding the enemy king).
	enemyMap := map[string]Bitboard{}
	if color == White {
		enemyMap["pawn"] = b.bp
		enemyMap["knight"] = b.bn
		enemyMap["bishop"] = b.bb
		enemyMap["rook"] = b.br
		enemyMap["queen"] = b.bq
	} else {
		enemyMap["pawn"] = b.wp
		enemyMap["knight"] = b.wn
		enemyMap["bishop"] = b.wb
		enemyMap["rook"] = b.wr
		enemyMap["queen"] = b.wq
	}

	// Create a combined enemy pieces bitboard.
	var enemyPieces Bitboard
	for _, bb := range enemyMap {
		enemyPieces |= bb
	}

	// Set up a temporary board so that move generation returns moves for the given color.
	temp := *b
	temp.Turn = color
	moves := generateMovesBB(&temp)

	// Map from an attacker's square to a mapping of attacked destination squares and their piece types.
	forkMap := make(map[int]map[int]string)
	for _, m := range moves {
		// Check if the destination square contains an enemy piece.
		if enemyPieces&(1<<uint(m.to)) != 0 {
			var pieceType string
			// Identify the type of enemy piece at the destination.
			for typ, bb := range enemyMap {
				if bb&(1<<uint(m.to)) != 0 {
					pieceType = typ
					break
				}
			}
			if pieceType == "" {
				continue
			}
			if _, exists := forkMap[m.from]; !exists {
				forkMap[m.from] = make(map[int]string)
			}
			forkMap[m.from][m.to] = pieceType
		}
	}

	maxCount := 0
	var forkedTypes []string
	// Search for any attacker that attacks 2 or more enemy pieces.
	for _, targets := range forkMap {
		if len(targets) >= 2 && len(targets) > maxCount {
			maxCount = len(targets)
			// Build a slice of unique piece types attacked.
			typesSet := make(map[string]struct{})
			for _, typ := range targets {
				typesSet[typ] = struct{}{}
			}
			forkedTypes = forkedTypes[:0] // reset the slice
			for typ := range typesSet {
				forkedTypes = append(forkedTypes, typ)
			}
		}
	}

	if maxCount >= 2 {
		return maxCount, forkedTypes
	}
	return 0, nil
}
