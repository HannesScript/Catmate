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
	X     int
	Y     int
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
	piece := b.GetPiece(move.FromX, move.FromY)
	if piece != nil {
		piece.X = move.ToX
		piece.Y = move.ToY
	}
	b.position[move.ToY][move.ToX] = b.position[move.FromY][move.FromX]
	b.position[move.FromY][move.FromX] = ' '
}

func isMoveLegal(board *Board, move Move) bool {
	cpy := board.Copy()
	cpy.ApplyMove(move)
	return !cpy.IsKingInCheck(board.Turn)
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
