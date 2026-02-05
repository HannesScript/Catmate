use crate::board::{Board, Move, Color};
use crate::moves;

/// Apply a move to the board (modifying it in place)
/// Returns the captured piece (if any) for proper undo
pub fn apply_move(board: &mut Board, mv: Move) -> Option<(Color, char)> {
    // Find which piece is moving
    let moving_piece = get_piece_at(board, mv.from);
    
    // Get captured piece at destination (if any)
    let captured = get_piece_at(board, mv.to);
    
    // Remove piece from source square
    clear_piece_at(board, mv.from);
    
    // Remove any captured piece from destination
    clear_piece_at(board, mv.to);
    
    // Place piece at destination
    set_piece_at(board, mv.to, moving_piece);
    
    // Switch turns
    board.turn = board.turn.flip();
    
    captured
}

/// Undo a move - restores piece and captured piece
pub fn undo_move(board: &mut Board, mv: Move, captured: Option<(Color, char)>) {
    // Get the piece that was moved (currently at mv.to)
    let moving_piece = get_piece_at(board, mv.to);
    
    // Move piece back to original square
    clear_piece_at(board, mv.to);
    set_piece_at(board, mv.from, moving_piece);
    
    // Restore captured piece
    if let Some(piece) = captured {
        set_piece_at(board, mv.to, Some(piece));
    }
    
    // Switch turn back
    board.turn = board.turn.flip();
}

// Helper functions
fn get_piece_at(board: &Board, sq: usize) -> Option<(Color, char)> {
    let bit = 1u64 << sq;
    
    if board.wp & bit != 0 { Some((Color::White, 'P')) }
    else if board.wn & bit != 0 { Some((Color::White, 'N')) }
    else if board.wb & bit != 0 { Some((Color::White, 'B')) }
    else if board.wr & bit != 0 { Some((Color::White, 'R')) }
    else if board.wq & bit != 0 { Some((Color::White, 'Q')) }
    else if board.wk & bit != 0 { Some((Color::White, 'K')) }
    else if board.bp & bit != 0 { Some((Color::Black, 'p')) }
    else if board.bn & bit != 0 { Some((Color::Black, 'n')) }
    else if board.bb & bit != 0 { Some((Color::Black, 'b')) }
    else if board.br & bit != 0 { Some((Color::Black, 'r')) }
    else if board.bq & bit != 0 { Some((Color::Black, 'q')) }
    else if board.bk & bit != 0 { Some((Color::Black, 'k')) }
    else { None }
}

fn clear_piece_at(board: &mut Board, sq: usize) {
    let bit = !(1u64 << sq);
    board.wp &= bit;
    board.wn &= bit;
    board.wb &= bit;
    board.wr &= bit;
    board.wq &= bit;
    board.wk &= bit;
    board.bp &= bit;
    board.bn &= bit;
    board.bb &= bit;
    board.br &= bit;
    board.bq &= bit;
    board.bk &= bit;
}

fn set_piece_at(board: &mut Board, sq: usize, piece: Option<(Color, char)>) {
    if let Some((color, piece_type)) = piece {
        let bit = 1u64 << sq;
        match (color, piece_type) {
            (Color::White, 'P') => board.wp |= bit,
            (Color::White, 'N') => board.wn |= bit,
            (Color::White, 'B') => board.wb |= bit,
            (Color::White, 'R') => board.wr |= bit,
            (Color::White, 'Q') => board.wq |= bit,
            (Color::White, 'K') => board.wk |= bit,
            (Color::Black, 'p') => board.bp |= bit,
            (Color::Black, 'n') => board.bn |= bit,
            (Color::Black, 'b') => board.bb |= bit,
            (Color::Black, 'r') => board.br |= bit,
            (Color::Black, 'q') => board.bq |= bit,
            (Color::Black, 'k') => board.bk |= bit,
            _ => {}
        }
    }
}

/// Check if a king is in check
pub fn is_king_in_check(board: &Board, color: Color) -> bool {
    let king_bb = match color {
        Color::White => board.wk,
        Color::Black => board.bk,
    };
    if king_bb == 0 {
        return false;
    }
    let king_sq = king_bb.trailing_zeros() as usize;
    
    // Check if any enemy piece can attack the king
    check_enemy_attacks(board, king_sq, color)
}

fn check_enemy_attacks(board: &Board, target_sq: usize, king_color: Color) -> bool {
    match king_color {
        Color::White => {
            // Check black pieces can attack target_sq
            check_pawn_attacks(target_sq, board.bp, Color::Black) ||
            check_knight_attacks(target_sq, board.bn) ||
            check_sliding_attacks(target_sq, board.bb, board.br, board.bq, board.occupied()) ||
            check_king_attacks(target_sq, board.bk)
        }
        Color::Black => {
            check_pawn_attacks(target_sq, board.wp, Color::White) ||
            check_knight_attacks(target_sq, board.wn) ||
            check_sliding_attacks(target_sq, board.wb, board.wr, board.wq, board.occupied()) ||
            check_king_attacks(target_sq, board.wk)
        }
    }
}

/// Check if it's checkmate
pub fn is_checkmate(board: &Board) -> bool {
    if !is_king_in_check(board, board.turn) {
        return false;
    }

    let moves = moves::generate_legal_moves(board);
    let mut temp_board = board.clone();
    for mv in moves {
        let captured = apply_move(&mut temp_board, mv);
        let in_check = is_king_in_check(&temp_board, board.turn);
        undo_move(&mut temp_board, mv, captured);
        
        if !in_check {
            return false;
        }
    }

    true
}

/// Check if it's stalemate
pub fn is_stalemate(board: &Board) -> bool {
    if is_king_in_check(board, board.turn) {
        return false;
    }

    let moves = moves::generate_legal_moves(board);
    moves.is_empty()
}

/// Check if there's insufficient material to checkmate
pub fn is_insufficient_material(board: &Board) -> bool {
    let white_non_king = board.wp.count_ones()
        + board.wn.count_ones()
        + board.wb.count_ones()
        + board.wr.count_ones()
        + board.wq.count_ones();

    let black_non_king = board.bp.count_ones()
        + board.bn.count_ones()
        + board.bb.count_ones()
        + board.br.count_ones()
        + board.bq.count_ones();

    white_non_king == 0 && black_non_king == 0
}

/// Check if it's a draw
pub fn is_draw(board: &Board) -> bool {
    is_stalemate(board) || is_insufficient_material(board)
}

/// Fork detection: returns (count, list of attacked piece types)
pub fn is_fork(board: &Board, color: Color) -> (u32, Vec<String>) {
    // Create a temporary board with the given color to move
    let mut temp = board.clone();
    temp.turn = color;

    let moves = moves::generate_legal_moves(&temp);
    let enemy = color.flip();

    // Build a map of enemy pieces
    let enemy_map = match enemy {
        Color::White => {
            [
                ("pawn", board.wp),
                ("knight", board.wn),
                ("bishop", board.wb),
                ("rook", board.wr),
                ("queen", board.wq),
            ]
        }
        Color::Black => {
            [
                ("pawn", board.bp),
                ("knight", board.bn),
                ("bishop", board.bb),
                ("rook", board.br),
                ("queen", board.bq),
            ]
        }
    };

    // Build enemy pieces bitboard
    let enemy_pieces: u64 = enemy_map.iter().map(|(_, bb)| bb).fold(0u64, |a, b| a | b);

    // Map from source square to set of attacked squares
    let mut fork_map: std::collections::HashMap<usize, std::collections::HashSet<usize>> =
        std::collections::HashMap::new();

    for mv in moves {
        if (enemy_pieces & (1 << mv.to)) != 0 {
            // Find the piece type at the destination
            let mut piece_type = "";
            for (typ, bb) in &enemy_map {
                if (bb & (1 << mv.to)) != 0 {
                    piece_type = typ;
                    break;
                }
            }

            if !piece_type.is_empty() {
                fork_map
                    .entry(mv.from)
                    .or_insert_with(std::collections::HashSet::new)
                    .insert(mv.to);
            }
        }
    }

    // Find the piece attacking the most enemy pieces
    let mut max_attacked = 0;
    let mut attacked_types = std::collections::HashSet::new();

    for attacked_squares in fork_map.values() {
        if attacked_squares.len() >= 2 && attacked_squares.len() > max_attacked as usize {
            max_attacked = attacked_squares.len() as u32;
            attacked_types.clear();

            // Identify piece types
            for &sq in attacked_squares {
                for (typ, bb) in &enemy_map {
                    if (bb & (1 << sq)) != 0 {
                        attacked_types.insert((*typ).to_string());
                        break;
                    }
                }
            }
        }
    }

    let types_vec: Vec<String> = attacked_types.into_iter().collect();
    (max_attacked, types_vec)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::fen;

    #[test]
    fn test_apply_move() {
        let fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq";
        let mut board = fen::parse_fen(fen);
        let original_wp = board.wp;
        let mv = Move::new(12, 20); // e2e3
        let _captured = apply_move(&mut board, mv);
        
        assert_eq!(board.turn, Color::Black);
        assert!(board.wp != original_wp);
    }

    #[test]
    fn test_is_king_in_check() {
        let board = fen::parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq");
        assert!(!is_king_in_check(&board, Color::White));
        assert!(!is_king_in_check(&board, Color::Black));
    }
}
// Attack checking helper functions

fn check_pawn_attacks(target_sq: usize, pawn_bb: u64, pawn_color: Color) -> bool {
    if pawn_bb == 0 {
        return false;
    }
    
    let target_bit = 1u64 << target_sq;
    let rank = target_sq / 8;
    let file = target_sq % 8;
    
    match pawn_color {
        Color::White => {
            // White pawns attack diagonally upward
            if rank > 0 {
                if file > 0 && (pawn_bb & (target_bit >> 9)) != 0 {
                    return true;
                }
                if file < 7 && (pawn_bb & (target_bit >> 7)) != 0 {
                    return true;
                }
            }
        }
        Color::Black => {
            // Black pawns attack diagonally downward
            if rank < 7 {
                if file > 0 && (pawn_bb & (target_bit << 7)) != 0 {
                    return true;
                }
                if file < 7 && (pawn_bb & (target_bit << 9)) != 0 {
                    return true;
                }
            }
        }
    }
    
    false
}

fn check_knight_attacks(target_sq: usize, knight_bb: u64) -> bool {
    if knight_bb == 0 {
        return false;
    }
    
    let rank = target_sq / 8;
    let file = target_sq % 8;
    
    let knight_offsets: [(i32, i32); 8] = [
        (-2, -1), (-2, 1), (-1, -2), (-1, 2),
        (1, -2), (1, 2), (2, -1), (2, 1),
    ];
    
    for (dr, df) in knight_offsets {
        let new_rank = rank as i32 + dr;
        let new_file = file as i32 + df;
        
        if new_rank >= 0 && new_rank < 8 && new_file >= 0 && new_file < 8 {
            let sq = (new_rank * 8 + new_file) as usize;
            if (knight_bb & (1u64 << sq)) != 0 {
                return true;
            }
        }
    }
    
    false
}

fn check_sliding_attacks(target_sq: usize, bishops: u64, rooks: u64, queens: u64, occupied: u64) -> bool {
    // Check diagonal attacks (bishops and queens)
    if bishops | queens != 0 {
        if check_diagonal_attacks(target_sq, bishops | queens, occupied) {
            return true;
        }
    }
    
    // Check straight attacks (rooks and queens)
    if rooks | queens != 0 {
        if check_straight_attacks(target_sq, rooks | queens, occupied) {
            return true;
        }
    }
    
    false
}

fn check_diagonal_attacks(target_sq: usize, attackers: u64, occupied: u64) -> bool {
    let rank = target_sq / 8;
    let file = target_sq % 8;
    
    // Four diagonal directions: NE, NW, SE, SW
    let directions: [(i32, i32); 4] = [(1, 1), (1, -1), (-1, 1), (-1, -1)];
    
    for (dr, df) in directions {
        let mut r = rank as i32 + dr;
        let mut f = file as i32 + df;
        
        while r >= 0 && r < 8 && f >= 0 && f < 8 {
            let sq = (r * 8 + f) as usize;
            let bit = 1u64 << sq;
            
            if (occupied & bit) != 0 {
                if (attackers & bit) != 0 {
                    return true;
                }
                break;
            }
            
            r += dr;
            f += df;
        }
    }
    
    false
}

fn check_straight_attacks(target_sq: usize, attackers: u64, occupied: u64) -> bool {
    let rank = target_sq / 8;
    let file = target_sq % 8;
    
    // Four straight directions: N, S, E, W
    let directions: [(i32, i32); 4] = [(1, 0), (-1, 0), (0, 1), (0, -1)];
    
    for (dr, df) in directions {
        let mut r = rank as i32 + dr;
        let mut f = file as i32 + df;
        
        while r >= 0 && r < 8 && f >= 0 && f < 8 {
            let sq = (r * 8 + f) as usize;
            let bit = 1u64 << sq;
            
            if (occupied & bit) != 0 {
                if (attackers & bit) != 0 {
                    return true;
                }
                break;
            }
            
            r += dr;
            f += df;
        }
    }
    
    false
}

fn check_king_attacks(target_sq: usize, king_bb: u64) -> bool {
    if king_bb == 0 {
        return false;
    }
    
    let king_sq = king_bb.trailing_zeros() as usize;
    let king_rank = king_sq / 8;
    let king_file = king_sq % 8;
    let target_rank = target_sq / 8;
    let target_file = target_sq % 8;
    
    let rank_diff = (king_rank as i32 - target_rank as i32).abs();
    let file_diff = (king_file as i32 - target_file as i32).abs();
    
    rank_diff <= 1 && file_diff <= 1
}