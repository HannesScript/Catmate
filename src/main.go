package main

import (
	"bufio"
	"fmt"
	"log"
	"math"
	"os"
	"strconv"
	"strings"
	"unicode"
)

var DB map[string]string

type Move struct {
	FromX, FromY, ToX, ToY int
}

type Color int

const (
	White Color = iota
	Black
)

type PieceType int

const (
	Pawn PieceType = iota
	Knight
	Bishop
	Rook
	Queen
	King
)

type Piece struct {
	Type  PieceType
	Color Color
}

type Board struct {
	position       [8][8]rune
	Turn           Color
	CastlingRights string
}

func (b *Board) IsOnBoard(x, y int) bool {
	return x >= 0 && x < 8 && y >= 0 && y < 8
}

func (b *Board) GetPiece(x, y int) *Piece {
	if !b.IsOnBoard(x, y) {
		return nil
	}
	r := b.position[y][x]
	if r == ' ' {
		return nil
	}
	p := &Piece{}
	if r >= 'A' && r <= 'Z' {
		p.Color = White
	} else {
		p.Color = Black
	}
	switch r {
	case 'P', 'p':
		p.Type = Pawn
	case 'N', 'n':
		p.Type = Knight
	case 'B', 'b':
		p.Type = Bishop
	case 'R', 'r':
		p.Type = Rook
	case 'Q', 'q':
		p.Type = Queen
	case 'K', 'k':
		p.Type = King
	}
	return p
}

func (b *Board) Copy() *Board {
	newBoard := *b
	return &newBoard
}

func (b *Board) ApplyMove(move Move) {
	b.position[move.ToY][move.ToX] = b.position[move.FromY][move.FromX]
	b.position[move.FromY][move.FromX] = ' '
}

func (b *Board) IsKingInCheck(color Color) bool {
	var kingX, kingY int
	for y := 0; y < 8; y++ {
		for x := 0; x < 8; x++ {
			p := b.GetPiece(x, y)
			if p != nil && p.Type == King && p.Color == color {
				kingX, kingY = x, y
				break
			}
		}
	}
	enemy := White
	if color == White {
		enemy = Black
	}
	for y := 0; y < 8; y++ {
		for x := 0; x < 8; x++ {
			p := b.GetPiece(x, y)
			if p != nil && p.Color == enemy {
				for _, m := range generatePieceMoves(b, x, y) {
					if m.ToX == kingX && m.ToY == kingY {
						return true
					}
				}
			}
		}
	}
	return false
}

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
	return score
}

func generatePawnMoves(board *Board, x, y int) []Move {
	moves := []Move{}
	dir := 1
	if board.Turn == Black {
		dir = -1
	}
	if board.IsOnBoard(x, y+dir) && board.GetPiece(x, y+dir) == nil {
		moves = append(moves, Move{FromX: x, FromY: y, ToX: x, ToY: y + dir})
		if (y == 1 && board.Turn == White || y == 6 && board.Turn == Black) &&
			board.IsOnBoard(x, y+2*dir) && board.GetPiece(x, y+2*dir) == nil {
			moves = append(moves, Move{FromX: x, FromY: y, ToX: x, ToY: y + 2*dir})
		}
	}
	for _, dx := range []int{-1, 1} {
		if board.IsOnBoard(x+dx, y+dir) {
			if target := board.GetPiece(x+dx, y+dir); target != nil && target.Color != board.Turn {
				moves = append(moves, Move{FromX: x, FromY: y, ToX: x + dx, ToY: y + dir})
			}
		}
	}
	return moves
}

func generateKnightMoves(board *Board, x, y int) []Move {
	var offsets = []struct{ dx, dy int }{
		{1, 2}, {2, 1}, {2, -1}, {1, -2},
		{-1, -2}, {-2, -1}, {-2, 1}, {-1, 2},
	}
	moves := []Move{}
	for _, o := range offsets {
		nx, ny := x+o.dx, y+o.dy
		if board.IsOnBoard(nx, ny) {
			t := board.GetPiece(nx, ny)
			if t == nil || t.Color != board.Turn {
				moves = append(moves, Move{FromX: x, FromY: y, ToX: nx, ToY: ny})
			}
		}
	}
	return moves
}

func generateSlidingMoves(board *Board, x, y int, dirs []struct{ dx, dy int }) []Move {
	moves := []Move{}
	for _, d := range dirs {
		nx, ny := x+d.dx, y+d.dy
		for board.IsOnBoard(nx, ny) {
			t := board.GetPiece(nx, ny)
			if t == nil {
				moves = append(moves, Move{FromX: x, FromY: y, ToX: nx, ToY: ny})
			} else {
				if t.Color != board.Turn {
					moves = append(moves, Move{FromX: x, FromY: y, ToX: nx, ToY: ny})
				}
				break
			}
			nx += d.dx
			ny += d.dy
		}
	}
	return moves
}

func generateBishopMoves(board *Board, x, y int) []Move {
	return generateSlidingMoves(board, x, y,
		[]struct{ dx, dy int }{{1, 1}, {1, -1}, {-1, 1}, {-1, -1}})
}

func generateRookMoves(board *Board, x, y int) []Move {
	return generateSlidingMoves(board, x, y,
		[]struct{ dx, dy int }{{1, 0}, {-1, 0}, {0, 1}, {0, -1}})
}

func generateQueenMoves(board *Board, x, y int) []Move {
	return generateSlidingMoves(board, x, y,
		[]struct{ dx, dy int }{
			{1, 0}, {-1, 0}, {0, 1}, {0, -1},
			{1, 1}, {1, -1}, {-1, 1}, {-1, -1},
		})
}

func generateKingMoves(board *Board, x, y int) []Move {
	var offsets = []struct{ dx, dy int }{
		{1, 0}, {-1, 0}, {0, 1}, {0, -1},
		{1, 1}, {1, -1}, {-1, 1}, {-1, -1},
	}
	moves := []Move{}
	for _, o := range offsets {
		nx, ny := x+o.dx, y+o.dy
		if board.IsOnBoard(nx, ny) {
			t := board.GetPiece(nx, ny)
			if t == nil || t.Color != board.Turn {
				moves = append(moves, Move{FromX: x, FromY: y, ToX: nx, ToY: ny})
			}
		}
	}
	return moves
}

func isMoveLegal(board *Board, move Move) bool {
	cpy := board.Copy()
	cpy.ApplyMove(move)
	return !cpy.IsKingInCheck(board.Turn)
}

func generatePieceMoves(board *Board, x, y int) []Move {
	p := board.GetPiece(x, y)
	if p == nil || p.Color != board.Turn {
		return nil
	}
	switch p.Type {
	case Pawn:
		return generatePawnMoves(board, x, y)
	case Knight:
		return generateKnightMoves(board, x, y)
	case Bishop:
		return generateBishopMoves(board, x, y)
	case Rook:
		return generateRookMoves(board, x, y)
	case Queen:
		return generateQueenMoves(board, x, y)
	case King:
		return generateKingMoves(board, x, y)
	}
	return nil
}

func generateMoves(board *Board) []Move {
	var moves []Move
	for y := 0; y < 8; y++ {
		for x := 0; x < 8; x++ {
			p := board.GetPiece(x, y)
			if p != nil && p.Color == board.Turn {
				for _, m := range generatePieceMoves(board, x, y) {
					if isMoveLegal(board, m) {
						moves = append(moves, m)
					}
				}
			}
		}
	}
	return moves
}

func minimax(board *Board, depth, alpha, beta int, maximizing bool) int {
	if depth == 0 {
		return evaluateBoard(board)
	}
	moves := generateMoves(board)
	if maximizing {
		best := math.MinInt
		for _, move := range moves {
			board.ApplyMove(move)
			val := minimax(board, depth-1, alpha, beta, false)
			board.position[move.FromY][move.FromX] = board.position[move.ToY][move.ToX]
			board.position[move.ToY][move.ToX] = ' '
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
	}
	best := math.MaxInt
	for _, move := range moves {
		board.ApplyMove(move)
		val := minimax(board, depth-1, alpha, beta, true)
		board.position[move.FromY][move.FromX] = board.position[move.ToY][move.ToX]
		board.position[move.ToY][move.ToX] = ' '
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

func findBestMove(board *Board, depth int) Move {
	bestMove := Move{}
	bestScore := math.MinInt
	alpha, beta := math.MinInt, math.MaxInt
	moves := generateMoves(board)
	for _, move := range moves {
		board.ApplyMove(move)
		score := minimax(board, depth-1, alpha, beta, false)
		board.position[move.FromY][move.FromX] = board.position[move.ToY][move.ToX]
		board.position[move.ToY][move.ToX] = ' '
		if score > bestScore {
			bestScore = score
			bestMove = move
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

// loadDB reads a file and returns a mapping from FEN to move (in UCI format).
func loadDB(filename string) map[string]string {
	f, err := os.Open(filename)
	if err != nil {
		log.Printf("Could not open DB file: %v", err)
		return nil
	}
	defer f.Close()

	db := make(map[string]string)
	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		parts := strings.SplitN(line, ":", 2)
		if len(parts) != 2 {
			continue
		}
		fen := strings.TrimSpace(parts[0])
		move := strings.TrimSpace(parts[1])
		db[fen] = move
	}
	return db
}

func parseFEN(fen_string string) Board {
	// This function should take in a FEN-String like "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq"
	// and it should return a Board with castling rights support.
	// Note: Ensure that the Board struct is updated to include a CastlingRights field,
	// e.g., CastlingRights string

	board := Board{}
	parts := strings.Split(fen_string, " ")
	ranks := strings.Split(parts[0], "/")
	for row := 0; row < 8; row++ {
		file := 0
		for _, c := range ranks[row] {
			if unicode.IsDigit(c) {
				skip := int(c - '0')
				for i := 0; i < skip; i++ {
					board.position[row][file] = ' '
					file++
				}
			} else {
				board.position[row][file] = rune(c)
				file++
			}
		}
	}
	if len(parts) > 1 && parts[1] == "w" {
		board.Turn = White
	} else {
		board.Turn = Black
	}

	// Parse castling rights if provided. If no castling rights, FEN provides "-"
	if len(parts) > 2 && parts[2] != "-" {
		board.CastlingRights = parts[2]
	} else {
		board.CastlingRights = ""
	}
	return board
}

func parseMove(move Move) string {
	// Parse something like "{1 0 2 2}" into something like "e2e4"
	fileFrom := 'a' + rune(move.FromX)
	rankFrom := '8' - rune(move.FromY)
	fileTo := 'a' + rune(move.ToX)
	rankTo := '8' - rune(move.ToY)
	return fmt.Sprintf("%c%d%c%d", fileFrom, rankFrom-'0', fileTo, rankTo-'0')
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
		log.Fatalf("Invalid depth: %v", err)
	}
	dbFile := os.Args[3]

	if dbFile != "None" {
		DB = loadDB(dbFile)
	}

	gameFen := parseFEN(fen)

	if dbFile != "None" {
		if candidate, ok := DB[fen]; ok && candidate != "" {
			fmt.Println(candidate)
			return
		}
	}

	move := findBestMove(&gameFen, depth)
	if move == (Move{}) {
		fmt.Println("No valid move found")
	} else {
		moveString := parseMove(move)
		fmt.Println(moveString)
	}
}
