use crate::board::{Board, Move, Color, Bitboard, file, rank, is_on_board};
use crate::utils;

/// Generate pawn moves using bitboards
fn generate_pawn_moves(board: &Board) -> Vec<Move> {
    let mut moves = Vec::new();
    let occ = board.occupied();

    match board.turn {
        Color::White => {
            // Single push
            let single = (board.wp << 8) & !occ;
            let mut bb = single;
            while bb != 0 {
                let to_sq = bb.trailing_zeros() as usize;
                let from_sq = to_sq - 8;
                moves.push(Move::new(from_sq, to_sq));
                bb &= bb - 1;
            }

            // Double push from rank 2
            let rank2: Bitboard = 0x000000000000FF00;
            let mut double = ((board.wp & rank2) << 8) & !occ;
            double = (double << 8) & !occ;
            bb = double;
            while bb != 0 {
                let to_sq = bb.trailing_zeros() as usize;
                let from_sq = to_sq - 16;
                moves.push(Move::new(from_sq, to_sq));
                bb &= bb - 1;
            }

            // Left captures
            let enemy = board.enemy_pieces();
            let left_capture = ((board.wp & 0xFEFEFEFEFEFEFEFE) << 7) & enemy;
            bb = left_capture;
            while bb != 0 {
                let to_sq = bb.trailing_zeros() as usize;
                let from_sq = to_sq - 7;
                moves.push(Move::new(from_sq, to_sq));
                bb &= bb - 1;
            }

            // Right captures
            let right_capture = ((board.wp & 0x7F7F7F7F7F7F7F7F) << 9) & enemy;
            bb = right_capture;
            while bb != 0 {
                let to_sq = bb.trailing_zeros() as usize;
                let from_sq = to_sq - 9;
                moves.push(Move::new(from_sq, to_sq));
                bb &= bb - 1;
            }
        }
        Color::Black => {
            // Single push
            let single = (board.bp >> 8) & !occ;
            let mut bb = single;
            while bb != 0 {
                let to_sq = bb.trailing_zeros() as usize;
                let from_sq = to_sq + 8;
                moves.push(Move::new(from_sq, to_sq));
                bb &= bb - 1;
            }

            // Double push from rank 7
            let rank7: Bitboard = 0x00FF000000000000;
            let mut double = ((board.bp & rank7) >> 8) & !occ;
            double = (double >> 8) & !occ;
            bb = double;
            while bb != 0 {
                let to_sq = bb.trailing_zeros() as usize;
                let from_sq = to_sq + 16;
                moves.push(Move::new(from_sq, to_sq));
                bb &= bb - 1;
            }

            // Left captures
            let enemy = board.enemy_pieces();
            let left_capture = ((board.bp & 0x7F7F7F7F7F7F7F7F) >> 9) & enemy;
            bb = left_capture;
            while bb != 0 {
                let to_sq = bb.trailing_zeros() as usize;
                let from_sq = to_sq + 9;
                moves.push(Move::new(from_sq, to_sq));
                bb &= bb - 1;
            }

            // Right captures
            let right_capture = ((board.bp & 0xFEFEFEFEFEFEFEFE) >> 7) & enemy;
            bb = right_capture;
            while bb != 0 {
                let to_sq = bb.trailing_zeros() as usize;
                let from_sq = to_sq + 7;
                moves.push(Move::new(from_sq, to_sq));
                bb &= bb - 1;
            }
        }
    }

    moves
}

/// Knight move offsets
const KNIGHT_OFFSETS: &[(i32, i32)] = &[
    (1, 2),
    (2, 1),
    (2, -1),
    (1, -2),
    (-1, -2),
    (-2, -1),
    (-2, 1),
    (-1, 2),
];

/// Generate knight moves
fn generate_knight_moves(board: &Board) -> Vec<Move> {
    let mut moves = Vec::new();
    let friendly = board.friendly_pieces();

    let knights = match board.turn {
        Color::White => board.wn,
        Color::Black => board.bn,
    };

    let mut bb = knights;
    while bb != 0 {
        let sq = bb.trailing_zeros() as usize;
        let f = file(sq) as i32;
        let r = rank(sq) as i32;

        for &(df, dr) in KNIGHT_OFFSETS {
            let to_f = f + df;
            let to_r = r + dr;
            if is_on_board(to_f, to_r) {
                let to_sq = (to_r as usize * 8) + (to_f as usize);
                if (friendly & (1 << to_sq)) == 0 {
                    moves.push(Move::new(sq, to_sq));
                }
            }
        }

        bb &= bb - 1;
    }

    moves
}

/// Diagonal directions for bishops
const BISHOP_DIRS: &[(i32, i32)] = &[(1, 1), (1, -1), (-1, 1), (-1, -1)];

/// Cardinal directions for rooks
const ROOK_DIRS: &[(i32, i32)] = &[(1, 0), (-1, 0), (0, 1), (0, -1)];

/// All directions for queens
const QUEEN_DIRS: &[(i32, i32)] = &[
    (1, 0),
    (-1, 0),
    (0, 1),
    (0, -1),
    (1, 1),
    (1, -1),
    (-1, 1),
    (-1, -1),
];

/// Generate sliding moves (for bishops, rooks, queens)
fn generate_sliding_moves(
    board: &Board,
    pieces: Bitboard,
    directions: &[(i32, i32)],
) -> Vec<Move> {
    let mut moves = Vec::new();
    let friendly = board.friendly_pieces();
    let occ = board.occupied();

    let mut bb = pieces;
    while bb != 0 {
        let sq = bb.trailing_zeros() as usize;
        let f = file(sq) as i32;
        let r = rank(sq) as i32;

        for &(df, dr) in directions {
            let mut to_f = f + df;
            let mut to_r = r + dr;

            while is_on_board(to_f, to_r) {
                let to_sq = (to_r as usize * 8) + (to_f as usize);
                let dest_mask = 1u64 << to_sq;

                if (occ & dest_mask) == 0 {
                    // Empty square, add move and continue sliding
                    moves.push(Move::new(sq, to_sq));
                } else {
                    // Occupied square
                    if (friendly & dest_mask) == 0 {
                        // Enemy piece, add capture and break
                        moves.push(Move::new(sq, to_sq));
                    }
                    break;
                }

                to_f += df;
                to_r += dr;
            }
        }

        bb &= bb - 1;
    }

    moves
}

/// Generate bishop moves
fn generate_bishop_moves(board: &Board) -> Vec<Move> {
    let bishops = match board.turn {
        Color::White => board.wb,
        Color::Black => board.bb,
    };
    generate_sliding_moves(board, bishops, BISHOP_DIRS)
}

/// Generate rook moves
fn generate_rook_moves(board: &Board) -> Vec<Move> {
    let rooks = match board.turn {
        Color::White => board.wr,
        Color::Black => board.br,
    };
    generate_sliding_moves(board, rooks, ROOK_DIRS)
}

/// Generate queen moves
fn generate_queen_moves(board: &Board) -> Vec<Move> {
    let queens = match board.turn {
        Color::White => board.wq,
        Color::Black => board.bq,
    };
    generate_sliding_moves(board, queens, QUEEN_DIRS)
}

/// Generate king moves
fn generate_king_moves(board: &Board) -> Vec<Move> {
    let mut moves = Vec::new();
    let friendly = board.friendly_pieces();

    let king = match board.turn {
        Color::White => board.wk,
        Color::Black => board.bk,
    };

    if king == 0 {
        return moves;
    }

    let sq = king.trailing_zeros() as usize;
    let f = file(sq) as i32;
    let r = rank(sq) as i32;

    for df in -1..=1 {
        for dr in -1..=1 {
            if df == 0 && dr == 0 {
                continue;
            }
            let to_f = f + df;
            let to_r = r + dr;
            if is_on_board(to_f, to_r) {
                let to_sq = (to_r as usize * 8) + (to_f as usize);
                if (friendly & (1 << to_sq)) == 0 {
                    moves.push(Move::new(sq, to_sq));
                }
            }
        }
    }

    moves
}

/// Generate all legal moves for the current position
pub fn generate_moves(board: &Board) -> Vec<Move> {
    let mut moves = generate_pawn_moves(board);
    moves.extend(generate_knight_moves(board));
    moves.extend(generate_bishop_moves(board));
    moves.extend(generate_rook_moves(board));
    moves.extend(generate_queen_moves(board));
    moves.extend(generate_king_moves(board));
    moves
}

pub fn generate_legal_moves(board: &Board) -> Vec<Move> {
    let pseudo_legal = generate_moves(board);
    let mut legal = Vec::with_capacity(pseudo_legal.len());
    let mut temp = board.clone();
    
    for mv in pseudo_legal {
        let captured = utils::apply_move(&mut temp, mv);
        if !utils::is_king_in_check(&temp, board.turn) {
            legal.push(mv);
        }
        utils::undo_move(&mut temp, mv, captured);
    }
    legal
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::fen;

    #[test]
    fn test_generate_starting_moves() {
        let fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq";
        let board = fen::parse_fen(fen);
        let moves = generate_moves(&board);
        
        // White should have 20 legal moves from starting position (16 pawn + 4 knight)
        assert_eq!(moves.len(), 20);
    }
}
