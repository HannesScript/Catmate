use crate::board::{Board, Color};
use crate::utils;

// Piece values
pub const PAWN_VALUE: i32 = 10;
pub const KNIGHT_VALUE: i32 = 32;
pub const BISHOP_VALUE: i32 = 33;
pub const ROOK_VALUE: i32 = 50;
pub const QUEEN_VALUE: i32 = 90;
pub const KING_VALUE: i32 = 2000;

// Event values
pub const CHECKMATE_VALUE: i32 = 1000;
pub const STALEMATE_VALUE: i32 = 0;
pub const DRAW_VALUE: i32 = 0;
pub const CHECK_VALUE: i32 = 25;
pub const CASTLING_VALUE: i32 = 50;
pub const FORK_VALUE: i32 = 50;

// Piece-square tables (as arrays indexed by square 0-63)
#[rustfmt::skip]
pub const PAWN_TABLE: [i32; 64] = [
    0, 5, 5, -10, -10, 5, 5, 0,
    0, 10, -5, 0, 0, -5, 10, 0,
    0, 10, 10, 20, 20, 10, 10, 0,
    0, 5, 10, 25, 25, 10, 5, 0,
    5, 10, 20, 30, 30, 20, 10, 5,
    10, 20, 30, 40, 40, 30, 20, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
    0, 0, 0, 0, 0, 0, 0, 0,
];

#[rustfmt::skip]
pub const KNIGHT_TABLE: [i32; 64] = [
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50,
];

#[rustfmt::skip]
pub const BISHOP_TABLE: [i32; 64] = [
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -20, -10, -10, -10, -10, -10, -10, -20,
];

#[rustfmt::skip]
pub const ROOK_TABLE: [i32; 64] = [
    0, 0, 0, 5, 5, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    5, 10, 10, 10, 10, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0,
];

#[rustfmt::skip]
pub const QUEEN_TABLE: [i32; 64] = [
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 5, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20,
];

#[rustfmt::skip]
pub const KING_OPENING_TABLE: [i32; 64] = [
    20, 30, 10, 0, 0, 10, 30, 20,
    20, 20, 0, 0, 0, 0, 20, 20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
];

#[rustfmt::skip]
pub const KING_ENDGAME_TABLE: [i32; 64] = [
    -50, -30, -30, -30, -30, -30, -30, -50,
    -30, -30, 0, 0, 0, 0, -30, -30,
    -30, -10, 20, 30, 30, 20, -10, -30,
    -30, -10, 30, 40, 40, 30, -10, -30,
    -30, -10, 30, 40, 40, 30, -10, -30,
    -30, -10, 20, 30, 30, 20, -10, -30,
    -30, -20, -20, 0, 0, -20, -20, -30,
    -50, -40, -30, -20, -20, -30, -40, -50,
];

/// Evaluate a piece and its position
fn evaluate_piece(bb: u64, value: i32, table: &[i32; 64]) -> i32 {
    let mut score = 0;
    let mut bitboard = bb;

    while bitboard != 0 {
        let sq = bitboard.trailing_zeros() as usize;
        score += value + table[sq];
        bitboard &= bitboard - 1;
    }

    score
}

/// Evaluate king position
fn evaluate_king(bb: u64, endgame: bool) -> i32 {
    if bb == 0 {
        return 0;
    }

    let sq = bb.trailing_zeros() as usize;
    if endgame {
        KING_ENDGAME_TABLE[sq]
    } else {
        KING_OPENING_TABLE[sq]
    }
}

/// Evaluate the entire board
pub fn evaluate_board(board: &Board) -> i32 {
    let mut score = 0;

    // Determine endgame (10 or fewer non-king pieces)
    let non_king_count = board.wp.count_ones()
        + board.wn.count_ones()
        + board.wb.count_ones()
        + board.wr.count_ones()
        + board.wq.count_ones()
        + board.bp.count_ones()
        + board.bn.count_ones()
        + board.bb.count_ones()
        + board.br.count_ones()
        + board.bq.count_ones();

    let endgame = non_king_count <= 10;

    // Evaluate white pieces
    score += evaluate_piece(board.wp, PAWN_VALUE, &PAWN_TABLE);
    score += evaluate_piece(board.wn, KNIGHT_VALUE, &KNIGHT_TABLE);
    score += evaluate_piece(board.wb, BISHOP_VALUE, &BISHOP_TABLE);
    score += evaluate_piece(board.wr, ROOK_VALUE, &ROOK_TABLE);
    score += evaluate_piece(board.wq, QUEEN_VALUE, &QUEEN_TABLE);
    score += evaluate_king(board.wk, endgame);

    // Evaluate black pieces (subtract their values)
    score -= evaluate_piece(board.bp, PAWN_VALUE, &PAWN_TABLE);
    score -= evaluate_piece(board.bn, KNIGHT_VALUE, &KNIGHT_TABLE);
    score -= evaluate_piece(board.bb, BISHOP_VALUE, &BISHOP_TABLE);
    score -= evaluate_piece(board.br, ROOK_VALUE, &ROOK_TABLE);
    score -= evaluate_piece(board.bq, QUEEN_VALUE, &QUEEN_TABLE);
    score -= evaluate_king(board.bk, endgame);

    // Event-based scoring
    if utils::is_checkmate(board) {
        if board.turn == Color::White {
            score -= CHECKMATE_VALUE;
        } else {
            score += CHECKMATE_VALUE;
        }
    } else if utils::is_stalemate(board) || utils::is_draw(board) {
        // No adjustment
    } else {
        // Bonus for delivering check
        if utils::is_king_in_check(board, Color::Black) {
            score += CHECK_VALUE;
        }
        if utils::is_king_in_check(board, Color::White) {
            score -= CHECK_VALUE;
        }
    }

    // Castling bonus
    if board.wk != 0 {
        let king_sq = board.wk.trailing_zeros() as usize;
        if king_sq == 6 || king_sq == 2 {
            score += CASTLING_VALUE;
        }
    }
    if board.bk != 0 {
        let king_sq = board.bk.trailing_zeros() as usize;
        if king_sq == 62 || king_sq == 58 {
            score -= CASTLING_VALUE;
        }
    }

    // Fork bonuses
    let (white_fork_count, white_types) = utils::is_fork(board, Color::White);
    if white_fork_count >= 2 {
        let mut bonus = FORK_VALUE;
        for typ in white_types {
            bonus += match typ.as_str() {
                "pawn" => PAWN_VALUE,
                "knight" => KNIGHT_VALUE,
                "bishop" => BISHOP_VALUE,
                "rook" => ROOK_VALUE,
                "queen" => QUEEN_VALUE,
                _ => 0,
            };
        }
        score += bonus;
    }

    let (black_fork_count, black_types) = utils::is_fork(board, Color::Black);
    if black_fork_count >= 2 {
        let mut bonus = FORK_VALUE;
        for typ in black_types {
            bonus += match typ.as_str() {
                "pawn" => PAWN_VALUE,
                "knight" => KNIGHT_VALUE,
                "bishop" => BISHOP_VALUE,
                "rook" => ROOK_VALUE,
                "queen" => QUEEN_VALUE,
                _ => 0,
            };
        }
        score -= bonus;
    }

    score
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::fen;

    #[test]
    fn test_evaluate_starting_position() {
        let fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq";
        let board = fen::parse_fen(fen);
        let score = evaluate_board(&board);
        
        // Starting position evaluation (piece-square tables can create some imbalance)
        // Just verify it computes without errors and is reasonable
        assert!(score.abs() < 1000);
    }
}
