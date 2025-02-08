package main

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

// Should return true if any of the kings are in checkmate
func isCheckmate(board *Board) bool {
	// Find the kings
	var whiteKing, blackKing *Piece
	for i := 0; i < 8; i++ {
		for j := 0; j < 8; j++ {
			piece := board.GetPiece(i, j)
			if piece != nil && piece.Type == King {
				if piece.Color == White {
					whiteKing = piece
				} else {
					blackKing = piece
				}
			}
		}
	}
	if whiteKing == nil || blackKing == nil {
		panic("Could not find both kings")
	}

	// Check if any of the kings are in checkmate
	if isCheckmateForKing(board, whiteKing) {
		return true
	}
	if isCheckmateForKing(board, blackKing) {
		return true
	}
	return false
}

// Should return true if the given king is in checkmate
func isCheckmateForKing(board *Board, king *Piece) bool {
	// Find all moves for the king
	kingX, kingY := -1, -1
	for i := 0; i < 8; i++ {
		for j := 0; j < 8; j++ {
			piece := board.GetPiece(i, j)
			if piece == king {
				kingX, kingY = i, j
				break
			}
		}
	}
	if kingX == -1 || kingY == -1 {
		panic("Could not find king on board")
	}
	kingMoves := generateKingMoves(board, kingX, kingY)

	// Check if any of the moves are legal
	for _, move := range kingMoves {
		newBoard := board.Copy()
		newBoard.ApplyMove(move)
		if !newBoard.IsKingInCheck(king.Color) {
			return false
		}
	}
	return true
}

// Should return true if any king is in stalemate
func isStalemate(board *Board) bool {
	// Find the kings
	var whiteKing, blackKing *Piece
	for i := 0; i < 8; i++ {
		for j := 0; j < 8; j++ {
			piece := board.GetPiece(i, j)
			if piece != nil && piece.Type == King {
				if piece.Color == White {
					whiteKing = piece
				} else {
					blackKing = piece
				}
			}
		}
	}
	if whiteKing == nil || blackKing == nil {
		panic("Could not find both kings")
	}

	// Check if any of the kings are in stalemate
	if isStalemateForKing(board, whiteKing) {
		return true
	}
	if isStalemateForKing(board, blackKing) {
		return true
	}
	return false
}

// Should return true if the given king is in stalemate
func isStalemateForKing(board *Board, king *Piece) bool {
	// Find all moves for the king
	kingX, kingY := -1, -1
	for i := 0; i < 8; i++ {
		for j := 0; j < 8; j++ {
			piece := board.GetPiece(i, j)
			if piece == king {
				kingX, kingY = i, j
				break
			}
		}
	}
	if kingX == -1 || kingY == -1 {
		panic("Could not find king on board")
	}
	kingMoves := generateKingMoves(board, kingX, kingY)

	// Check if any of the moves are legal
	for _, move := range kingMoves {
		newBoard := board.Copy()
		newBoard.ApplyMove(move)
		if !newBoard.IsKingInCheck(king.Color) {
			return false
		}
	}
	return true
}

// Should return true if the game is a draw
func isDraw(board *Board) bool {
	// Check if the game is a draw
	if isStalemate(board) {
		return true
	}
	if isInsufficientMaterial(board) {
		return true
	}
	return false
}

// Should return true if the game is a draw due to insufficient material
func isInsufficientMaterial(board *Board) bool {
	// Check if the game is a draw due to insufficient material
	// Only kings are left
	for i := 0; i < 8; i++ {
		for j := 0; j < 8; j++ {
			piece := board.GetPiece(i, j)
			if piece != nil && piece.Type != King {
				return false
			}
		}
	}
	return true
}

// Should return true if any king has castled
func isCastling(board *Board) bool {
	// Check if any king has castled
	// White king-side
	if board.GetPiece(4, 0).Type == King && board.GetPiece(7, 0).Type == Rook {
		return true
	}
	// White queen-side
	if board.GetPiece(4, 0).Type == King && board.GetPiece(0, 0).Type == Rook {
		return true
	}
	// Black king-side
	if board.GetPiece(4, 7).Type == King && board.GetPiece(7, 7).Type == Rook {
		return true
	}
	// Black queen-side
	if board.GetPiece(4, 7).Type == King && board.GetPiece(0, 7).Type == Rook {
		return true
	}
	return false
}
