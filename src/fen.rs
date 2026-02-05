use crate::board::{Board, Color};

/// Parse a simplified FEN string (without en passant and move count)
/// Format: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq"
pub fn parse_fen(fen: &str) -> Board {
    let mut board = Board::new();
    
    let parts: Vec<&str> = fen.split_whitespace().collect();
    if parts.is_empty() {
        return board;
    }

    // Parse piece placement (first part of FEN)
    let ranks: Vec<&str> = parts[0].split('/').collect();
    for (r, rank_str) in ranks.iter().enumerate() {
        let rank = 7 - r; // FEN is top to bottom, we work bottom to top
        let mut file = 0;
        
        for ch in rank_str.chars() {
            if ch.is_ascii_digit() {
                file += ch as usize - b'0' as usize;
            } else {
                let square = rank * 8 + file;
                match ch {
                    'P' => board.wp |= 1 << square,
                    'N' => board.wn |= 1 << square,
                    'B' => board.wb |= 1 << square,
                    'R' => board.wr |= 1 << square,
                    'Q' => board.wq |= 1 << square,
                    'K' => board.wk |= 1 << square,
                    'p' => board.bp |= 1 << square,
                    'n' => board.bn |= 1 << square,
                    'b' => board.bb |= 1 << square,
                    'r' => board.br |= 1 << square,
                    'q' => board.bq |= 1 << square,
                    'k' => board.bk |= 1 << square,
                    _ => {}
                }
                file += 1;
            }
        }
    }

    // Parse color to move
    if parts.len() >= 2 {
        board.turn = if parts[1] == "b" {
            Color::Black
        } else {
            Color::White
        };
    }

    board
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_starting_position() {
        let fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq";
        let board = parse_fen(fen);
        
        // White pieces should be on rank 0 and 1
        assert!(board.wp != 0); // White pawns on rank 1
        assert!(board.wr != 0); // White rooks on rank 0
        assert!(board.wk != 0); // White king on rank 0
        
        // Black pieces should be on rank 6 and 7
        assert!(board.bp != 0); // Black pawns on rank 6
        assert!(board.bk != 0); // Black king on rank 7
        
        assert_eq!(board.turn, Color::White);
    }
}
