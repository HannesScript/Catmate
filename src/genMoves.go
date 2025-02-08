package main

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
