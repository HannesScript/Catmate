package main

import (
	"bufio"
	"fmt"
	"math/bits"
	"os"
	"strconv"
	"strings"
)

// Bitboard type
type Bitboard uint64

// Turn constants
const (
	White = 0
	Black = 1
)

// Board holds one bitboard per piece type.
type Board struct {
	// White pieces
	wp, wn, wb, wr, wq, wk Bitboard
	// Black pieces
	bp, bn, bb, br, bq, bk Bitboard
	Turn                   int
}

// parseFEN is a simplified parser that only recognizes the starting position.
// A full FEN parser must set each bitboard appropriately.
func parseFEN(fen string) Board {
	var board Board

	// Set the bitboards for the position specified in the FEN.
	// Recognize the FEN and initialize bitboards accordingly.
	fields := strings.Fields(fen)
	ranks := strings.Split(fields[0], "/")
	for r, row := range ranks {
		file := 0
		for _, c := range row {
			if c >= '1' && c <= '8' {
				file += int(c - '0')
			} else {
				square := (7-r)*8 + file
				switch c {
				case 'P':
					board.wp |= 1 << square
				case 'N':
					board.wn |= 1 << square
				case 'B':
					board.wb |= 1 << square
				case 'R':
					board.wr |= 1 << square
				case 'Q':
					board.wq |= 1 << square
				case 'K':
					board.wk |= 1 << square
				case 'p':
					board.bp |= 1 << square
				case 'n':
					board.bn |= 1 << square
				case 'b':
					board.bb |= 1 << square
				case 'r':
					board.br |= 1 << square
				case 'q':
					board.bq |= 1 << square
				case 'k':
					board.bk |= 1 << square
				}
				file++
			}
		}
	}

	// Determine turn from FEN; assume standard starting FEN.
	if len(fields) >= 2 && fields[1] == "b" {
		board.Turn = Black
	} else {
		board.Turn = White
	}
	return board
}

// Move represents a move from one square to another.
// Here squares are numbered 0..63 (a1 is 0, h8 is 63).
type Move struct {
	from, to int
}

func (m Move) String() string {
	return fmt.Sprintf("%c%d%c%d", 'a'+m.from%8, 1+m.from/8, 'a'+m.to%8, 1+m.to/8)
}

// occupied returns a bitboard of all occupied squares.
func occupied(b *Board) Bitboard {
	return b.wp | b.wn | b.wb | b.wr | b.wq | b.wk |
		b.bp | b.bn | b.bb | b.br | b.bq | b.bk
}

// generateMoves creates a move list; here we demonstrate only white pawn single pushes.
func generateMoves(b *Board) []Move {
	var moves []Move
	if b.Turn == White {
		// White pawns move one square up (i.e. shift left by 8 bits)
		singlePush := (b.wp << 8) &^ occupied(b)
		for singlePush != 0 {
			toSquare := bits.TrailingZeros64(uint64(singlePush))
			fromSquare := toSquare - 8
			moves = append(moves, Move{from: fromSquare, to: toSquare})
			singlePush &= singlePush - 1
		}
	} else {
		// Similarly you would generate moves for black pawns (shift right by 8)
		singlePush := (b.bp >> 8) &^ occupied(b)
		for singlePush != 0 {
			toSquare := bits.TrailingZeros64(uint64(singlePush))
			fromSquare := toSquare + 8
			moves = append(moves, Move{from: fromSquare, to: toSquare})
			singlePush &= singlePush - 1
		}
	}
	return moves
}

// applyMove returns a new board after applying the given move.
// For demonstration, we update only white pawn moves.
func applyMove(b Board, move Move) Board {
	fromMask := Bitboard(1) << move.from
	toMask := Bitboard(1) << move.to

	// Remove any captured piece (basic example – in a full engine you would check all piece bitboards)
	if b.Turn == White {
		if b.bp&toMask != 0 {
			b.bp &^= toMask
		}
		// Update white pawn bitboard:
		if b.wp&fromMask != 0 {
			b.wp &^= fromMask
			b.wp |= toMask
		}
	} else {
		if b.wp&toMask != 0 {
			b.wp &^= toMask
		}
		if b.bp&fromMask != 0 {
			b.bp &^= fromMask
			b.bp |= toMask
		}
	}
	// Switch turn
	if b.Turn == White {
		b.Turn = Black
	} else {
		b.Turn = White
	}
	return b
}

func popCount(bb Bitboard) int {
	return bits.OnesCount64(uint64(bb))
}

// minimax with alpha-beta pruning.
func minimax(b Board, depth, alpha, beta int, maximizing bool) int {
	if depth == 0 {
		return evaluateBoard(&b)
	}
	moves := generateMoves(&b)
	if maximizing {
		best := -1000000
		for _, m := range moves {
			newB := applyMove(b, m)
			val := minimax(newB, depth-1, alpha, beta, false)
			if val > best {
				best = val
			}
			if best > alpha {
				alpha = best
			}
			if beta <= alpha {
				break
			}
		}
		return best
	} else {
		best := 1000000
		for _, m := range moves {
			newB := applyMove(b, m)
			val := minimax(newB, depth-1, alpha, beta, true)
			if val < best {
				best = val
			}
			if best < beta {
				beta = best
			}
			if beta <= alpha {
				break
			}
		}
		return best
	}
}

// findBestMove determines the move with the best evaluation.
func findBestMove(b Board, depth int) Move {
	bestScore := -1000000
	var bestMove Move
	moves := generateMoves(&b)
	alpha, beta := -1000000, 1000000
	for _, m := range moves {
		newB := applyMove(b, m)
		score := minimax(newB, depth-1, alpha, beta, false)
		if score > bestScore {
			bestScore = score
			bestMove = m
		}
		if bestScore > alpha {
			alpha = bestScore
		}
		if beta <= alpha {
			break
		}
	}
	return bestMove
}

func main() {
	// Command line args: "<FEN>" <depth> <dbFile>
	if len(os.Args) != 4 {
		fmt.Println("Usage: \"<FEN>\" <depth> <dbFile>")
		return
	}
	fen := os.Args[1]
	depth, err := strconv.Atoi(os.Args[2])
	if err != nil {
		fmt.Printf("Invalid depth: %v\n", err)
		return
	}

	dbFile := os.Args[3]

	if dbFile != "None" {
		file, err := os.Open(dbFile)
		if err != nil {
			fmt.Println("Can't open DB:", err)
			return
		}
		defer file.Close()

		scanner := bufio.NewScanner(file)
		var foundMove string
		for scanner.Scan() {
			line := scanner.Text()
			parts := strings.Split(line, " : ")
			if len(parts) == 2 && strings.HasPrefix(parts[0], fen) {
				foundMove = parts[1]
				break
			}
		}

		if foundMove != "" {
			fmt.Println(foundMove)
			return
		}
	}

	board := parseFEN(fen)
	move := findBestMove(board, depth)
	fmt.Println(move.String())
}
