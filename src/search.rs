use crate::board::{Board, Move, Color};
use crate::moves;
use crate::evaluate;
use crate::utils;

/// Piece values for move ordering
const PIECE_VALUES: [i32; 6] = [100, 320, 330, 500, 900, 20000]; // P, N, B, R, Q, K

/// Get piece value for move ordering
fn get_piece_value(piece_char: char) -> i32 {
    match piece_char.to_ascii_uppercase() {
        'P' => PIECE_VALUES[0],
        'N' => PIECE_VALUES[1],
        'B' => PIECE_VALUES[2],
        'R' => PIECE_VALUES[3],
        'Q' => PIECE_VALUES[4],
        'K' => PIECE_VALUES[5],
        _ => 0,
    }
}

/// Order moves for better alpha-beta pruning
/// Returns a score where higher is better (search first)
fn score_move(board: &Board, mv: Move) -> i32 {
    let mut score = 0;
    
    // Check if this move captures a piece
    if let Some(captured_piece) = get_piece_at(board, mv.to) {
        let victim_value = get_piece_value(captured_piece.1);
        
        // Get the attacking piece
        if let Some(attacker_piece) = get_piece_at(board, mv.from) {
            let attacker_value = get_piece_value(attacker_piece.1);
            
            // MVV-LVA: Most Valuable Victim - Least Valuable Attacker
            // Multiply victim by 10 to prioritize higher value captures
            score = victim_value * 10 - attacker_value;
        } else {
            score = victim_value * 10;
        }
    }
    
    score
}

/// Helper function to get piece at a square (duplicated from utils for performance)
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

/// Minimax with alpha-beta pruning
fn minimax(
    board: &mut Board,
    depth: i32,
    mut alpha: i32,
    mut beta: i32,
    maximizing: bool,
) -> i32 {
    if depth == 0 {
        return evaluate::evaluate_board(&board);
    }

    let moves_list = moves::generate_legal_moves(&board);

    if maximizing {
        let mut best = i32::MIN;
        for mv in moves_list {
            let captured = utils::apply_move(board, mv);
            let val = minimax(board, depth - 1, alpha, beta, false);
            utils::undo_move(board, mv, captured);
            
            best = best.max(val);
            alpha = alpha.max(best);
            if beta <= alpha {
                break;
            }
        }
        best
    } else {
        let mut best = i32::MAX;
        for mv in moves_list {
            let captured = utils::apply_move(board, mv);
            let val = minimax(board, depth - 1, alpha, beta, true);
            utils::undo_move(board, mv, captured);
            
            best = best.min(val);
            beta = beta.min(best);
            if beta <= alpha {
                break;
            }
        }
        best
    }
}

/// Find the best move using minimax with alpha-beta pruning
pub fn find_best_move(mut board: Board, depth: i32) -> Move {
    let mut best_score = i32::MIN;
    let mut best_move = Move::new(0, 0);

    let mut moves_list = moves::generate_legal_moves(&board);
    
    // Order moves for better pruning (captures first)
    moves_list.sort_by_cached_key(|mv| -score_move(&board, *mv));
    
    let mut alpha = i32::MIN;
    let beta = i32::MAX;

    for mv in moves_list {
        let captured = utils::apply_move(&mut board, mv);
        let score = minimax(&mut board, depth - 1, alpha, beta, false);
        utils::undo_move(&mut board, mv, captured);

        if score > best_score {
            best_score = score;
            best_move = mv;
        }

        alpha = alpha.max(best_score);
        if beta <= alpha {
            break;
        }
    }

    best_move
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::fen;

    #[test]
    fn test_find_best_move() {
        let fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq";
        let board = fen::parse_fen(fen);
        let mv = find_best_move(board, 2);
        
        // Should find some reasonable move
        assert!(mv.from != mv.to);
    }
}
