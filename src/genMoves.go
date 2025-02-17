package main

import (
	"math/bits"
)

// Helpers for bitboard moves.
func file(sq int) int { return sq % 8 }
func rank(sq int) int { return sq / 8 }

// friendlyPieces returns all pieces for the side to move.
func friendlyPieces(b *Board) Bitboard {
	if b.Turn == White {
		return b.wp | b.wn | b.wb | b.wr | b.wq | b.wk
	}
	return b.bp | b.bn | b.bb | b.br | b.bq | b.bk
}

// enemyPieces returns all enemy pieces.
func enemyPieces(b *Board) Bitboard {
	if b.Turn == White {
		return b.bp | b.bn | b.bb | b.br | b.bq | b.bk
	}
	return b.wp | b.wn | b.wb | b.wr | b.wq | b.wk
}

// isOnBoardIndex checks whether (f, r) is on board.
func isOnBoardIndex(f, r int) bool {
	return f >= 0 && f < 8 && r >= 0 && r < 8
}

// generatePawnMovesBB generates pawn moves using bitboards.
func generatePawnMovesBB(b *Board) []Move {
	var moves []Move
	occ := occupied(b)
	if b.Turn == White {
		// single pawn push
		single := (b.wp << 8) &^ occ
		for single != 0 {
			toSq := bits.TrailingZeros64(uint64(single))
			fromSq := toSq - 8
			moves = append(moves, Move{from: fromSq, to: toSq})
			single &= single - 1
		}
		// Double push (only from rank2)
		rank2 := Bitboard(0x000000000000FF00)
		double := ((b.wp & rank2) << 8) &^ occ
		double = (double << 8) &^ occ
		for double != 0 {
			toSq := bits.TrailingZeros64(uint64(double))
			fromSq := toSq - 16
			moves = append(moves, Move{from: fromSq, to: toSq})
			double &= double - 1
		}
		// Captures
		leftCapture := ((b.wp & 0xfefefefefefefefe) << 7) & enemyPieces(b)
		for leftCapture != 0 {
			toSq := bits.TrailingZeros64(uint64(leftCapture))
			fromSq := toSq - 7
			moves = append(moves, Move{from: fromSq, to: toSq})
			leftCapture &= leftCapture - 1
		}
		rightCapture := ((b.wp & 0x7f7f7f7f7f7f7f7f) << 9) & enemyPieces(b)
		for rightCapture != 0 {
			toSq := bits.TrailingZeros64(uint64(rightCapture))
			fromSq := toSq - 9
			moves = append(moves, Move{from: fromSq, to: toSq})
			rightCapture &= rightCapture - 1
		}
	} else {
		// Black pawn moves (mirrored shifts)
		single := (b.bp >> 8) &^ occ
		for single != 0 {
			toSq := bits.TrailingZeros64(uint64(single))
			fromSq := toSq + 8
			moves = append(moves, Move{from: fromSq, to: toSq})
			single &= single - 1
		}
		// Double push (from rank7)
		rank7 := Bitboard(0x00FF000000000000)
		double := ((b.bp & rank7) >> 8) &^ occ
		double = (double >> 8) &^ occ
		for double != 0 {
			toSq := bits.TrailingZeros64(uint64(double))
			fromSq := toSq + 16
			moves = append(moves, Move{from: fromSq, to: toSq})
			double &= double - 1
		}
		// Captures
		leftCapture := ((b.bp & 0x7f7f7f7f7f7f7f7f) >> 9) & enemyPieces(b)
		for leftCapture != 0 {
			toSq := bits.TrailingZeros64(uint64(leftCapture))
			fromSq := toSq + 9
			moves = append(moves, Move{from: fromSq, to: toSq})
			leftCapture &= leftCapture - 1
		}
		rightCapture := ((b.bp & 0xfefefefefefefefe) >> 7) & enemyPieces(b)
		for rightCapture != 0 {
			toSq := bits.TrailingZeros64(uint64(rightCapture))
			fromSq := toSq + 7
			moves = append(moves, Move{from: fromSq, to: toSq})
			rightCapture &= rightCapture - 1
		}
	}
	return moves
}

// knight move offsets relative to a square.
var knightOffsets = []struct{ dx, dy int }{
	{1, 2}, {2, 1}, {2, -1}, {1, -2},
	{-1, -2}, {-2, -1}, {-2, 1}, {-1, 2},
}

// generateKnightMovesBB generates knight moves using bitboards.
func generateKnightMovesBB(b *Board) []Move {
	var moves []Move
	var knights Bitboard
	if b.Turn == White {
		knights = b.wn
	} else {
		knights = b.bn
	}
	friend := friendlyPieces(b)
	for knights != 0 {
		sq := bits.TrailingZeros64(uint64(knights))
		fromF, fromR := file(sq), rank(sq)
		for _, off := range knightOffsets {
			toF, toR := fromF+off.dx, fromR+off.dy
			if isOnBoardIndex(toF, toR) {
				toSq := toR*8 + toF
				// allow move if destination not occupied by own piece
				if (friend & (Bitboard(1) << toSq)) == 0 {
					moves = append(moves, Move{from: sq, to: toSq})
				}
			}
		}
		knights &= knights - 1
	}
	return moves
}

// sliding directions for bishops, rooks and queens.
var bishopDirs = []struct{ dx, dy int }{
	{1, 1}, {1, -1}, {-1, 1}, {-1, -1},
}
var rookDirs = []struct{ dx, dy int }{
	{1, 0}, {-1, 0}, {0, 1}, {0, -1},
}
var queenDirs = []struct{ dx, dy int }{
	{1, 0}, {-1, 0}, {0, 1}, {0, -1},
	{1, 1}, {1, -1}, {-1, 1}, {-1, -1},
}

// generateSlidingMovesBB is generic for sliding pieces.
func generateSlidingMovesBB(b *Board, pieces Bitboard, directions []struct{ dx, dy int }) []Move {
	var moves []Move
	friend := friendlyPieces(b)
	occ := occupied(b)
	for pieces != 0 {
		sq := bits.TrailingZeros64(uint64(pieces))
		fromF, fromR := file(sq), rank(sq)
		for _, d := range directions {
			toF, toR := fromF+d.dx, fromR+d.dy
			for isOnBoardIndex(toF, toR) {
				toSq := toR*8 + toF
				destMask := Bitboard(1) << toSq
				// if not occupied, add move and continue sliding.
				if occ&destMask == 0 {
					moves = append(moves, Move{from: sq, to: toSq})
				} else {
					// If enemy piece, add move then break.
					if friend&destMask == 0 {
						moves = append(moves, Move{from: sq, to: toSq})
					}
					break
				}
				toF += d.dx
				toR += d.dy
			}
		}
		pieces &= pieces - 1
	}
	return moves
}

func generateBishopMovesBB(b *Board) []Move {
	var bishops Bitboard
	if b.Turn == White {
		bishops = b.wb
	} else {
		bishops = b.bb
	}
	return generateSlidingMovesBB(b, bishops, bishopDirs)
}

func generateRookMovesBB(b *Board) []Move {
	var rooks Bitboard
	if b.Turn == White {
		rooks = b.wr
	} else {
		rooks = b.br
	}
	return generateSlidingMovesBB(b, rooks, rookDirs)
}

func generateQueenMovesBB(b *Board) []Move {
	var queens Bitboard
	if b.Turn == White {
		queens = b.wq
	} else {
		queens = b.bq
	}
	return generateSlidingMovesBB(b, queens, queenDirs)
}

func generateKingMovesBB(b *Board) []Move {
	var moves []Move
	var king Bitboard
	if b.Turn == White {
		king = b.wk
	} else {
		king = b.bk
	}
	friend := friendlyPieces(b)
	if king == 0 {
		return moves
	}
	sq := bits.TrailingZeros64(uint64(king))
	fromF, fromR := file(sq), rank(sq)
	// king moves in all 8 directions.
	for dx := -1; dx <= 1; dx++ {
		for dy := -1; dy <= 1; dy++ {
			if dx == 0 && dy == 0 {
				continue
			}
			toF, toR := fromF+dx, fromR+dy
			if isOnBoardIndex(toF, toR) {
				toSq := toR*8 + toF
				if friend&(Bitboard(1)<<toSq) == 0 {
					moves = append(moves, Move{from: sq, to: toSq})
				}
			}
		}
	}
	return moves
}

// generateMovesBB aggregates all moves for the side to move.
func generateMovesBB(b *Board) []Move {
	moves := generatePawnMovesBB(b)
	moves = append(moves, generateKnightMovesBB(b)...)
	moves = append(moves, generateBishopMovesBB(b)...)
	moves = append(moves, generateRookMovesBB(b)...)
	moves = append(moves, generateQueenMovesBB(b)...)
	moves = append(moves, generateKingMovesBB(b)...)
	return moves
}
