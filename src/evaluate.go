package main

import (
	"strings"
	"unicode"
)

func evaluateBoard(board *Board) int {
	score := 0
	for i := 0; i < 8; i++ {
		for j := 0; j < 8; j++ {
			piece := board.position[i][j]
			index := i*8 + j

			// Simple heuristic to detect endgame by counting non-king pieces
			endgame := func() bool {
				var count int
				for i := 0; i < 8; i++ {
					for j := 0; j < 8; j++ {
						p := board.position[i][j]
						if p != ' ' && unicode.ToLower(p) != 'k' {
							count++
						}
					}
				}
				// Adjust threshold to taste (example: <= 12)
				return count <= 10
			}()

			// Piece values
			for i := 0; i < 8; i++ {
				for j := 0; j < 8; j++ {
					p := board.position[i][j]
					if p != ' ' && unicode.ToLower(p) != 'k' {
						if unicode.IsUpper(p) {
							score += pieceValues[strings.ToLower(string(p))]
						} else {
							score -= pieceValues[strings.ToLower(string(p))]
						}
					}
				}
			}

			switch piece {
			case 'P':
				score += pawnTable[index]
			case 'p':
				score -= pawnTable[index]
			case 'N':
				score += knightTable[index]
			case 'n':
				score -= knightTable[index]
			case 'B':
				score += bishopTable[index]
			case 'b':
				score -= bishopTable[index]
			case 'R':
				score += rookTable[index]
			case 'r':
				score -= rookTable[index]
			case 'Q':
				score += queenTable[index]
			case 'q':
				score -= queenTable[index]
			case 'K':
				if endgame {
					score += kingEndgameTable[index]
				} else {
					score += kingOpeningTable[index]
				}
			case 'k':
				if endgame {
					score -= kingEndgameTable[index]
				} else {
					score -= kingOpeningTable[index]
				}
			}
		}
	}

	// ///// Handle Events (e.g. checkmate, castling, check, etc.)
	// // Checkmate
	//if isCheckmate(board) {
	//	if board.Turn == White {
	//		score -= eventValues["checkmate"]
	//	}
	//	score += eventValues["checkmate"]
	//}
	// // Stalemate
	//if isStalemate(board) {
	//	score += eventValues["stalemate"]
	//}
	// // Draw
	//if isDraw(board) {
	//	score += eventValues["draw"]
	//}
	// // Check
	//if board.IsKingInCheck(board.Turn) {
	//	if board.Turn == White {
	//		score -= eventValues["check"]
	//	}
	//	score += eventValues["check"]
	//}
	// // Castling
	//if isCastling(board) {
	//	if board.Turn == White {
	//		score -= eventValues["castling"]
	//	}
	//	score += eventValues["castling"]
	//}

	return score
}
