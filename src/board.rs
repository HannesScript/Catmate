use std::fmt;

/// Bitboard representation (u64)
pub type Bitboard = u64;

/// Color constants
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub enum Color {
    White,
    Black,
}

impl Color {
    pub fn flip(&self) -> Color {
        match self {
            Color::White => Color::Black,
            Color::Black => Color::White,
        }
    }
}

/// Move representation: from square to square
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub struct Move {
    pub from: usize,
    pub to: usize,
    pub captured: Option<(Color, char)>, // Store captured piece for undo
}

impl Move {
    pub fn new(from: usize, to: usize) -> Self {
        Move { from, to, captured: None }
    }
    
    pub fn with_capture(from: usize, to: usize, captured: (Color, char)) -> Self {
        Move { from, to, captured: Some(captured) }
    }
}

impl fmt::Display for Move {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let from_file = (self.from % 8) as u8;
        let from_rank = (self.from / 8) as u8;
        let to_file = (self.to % 8) as u8;
        let to_rank = (self.to / 8) as u8;

        write!(
            f,
            "{}{}{}{}",
            (b'a' + from_file) as char,
            (b'1' + from_rank) as char,
            (b'a' + to_file) as char,
            (b'1' + to_rank) as char
        )
    }
}

/// Board representation using bitboards
#[derive(Debug, Clone)]
pub struct Board {
    // White pieces
    pub wp: Bitboard, // white pawns
    pub wn: Bitboard, // white knights
    pub wb: Bitboard, // white bishops
    pub wr: Bitboard, // white rooks
    pub wq: Bitboard, // white queens
    pub wk: Bitboard, // white king

    // Black pieces
    pub bp: Bitboard, // black pawns
    pub bn: Bitboard, // black knights
    pub bb: Bitboard, // black bishops
    pub br: Bitboard, // black rooks
    pub bq: Bitboard, // black queens
    pub bk: Bitboard, // black king

    pub turn: Color,
}

impl Board {
    pub fn new() -> Self {
        Board {
            wp: 0,
            wn: 0,
            wb: 0,
            wr: 0,
            wq: 0,
            wk: 0,
            bp: 0,
            bn: 0,
            bb: 0,
            br: 0,
            bq: 0,
            bk: 0,
            turn: Color::White,
        }
    }

    /// Get all occupied squares
    pub fn occupied(&self) -> Bitboard {
        self.wp
            | self.wn
            | self.wb
            | self.wr
            | self.wq
            | self.wk
            | self.bp
            | self.bn
            | self.bb
            | self.br
            | self.bq
            | self.bk
    }

    /// Get all friendly pieces for the side to move
    pub fn friendly_pieces(&self) -> Bitboard {
        match self.turn {
            Color::White => self.wp | self.wn | self.wb | self.wr | self.wq | self.wk,
            Color::Black => self.bp | self.bn | self.bb | self.br | self.bq | self.bk,
        }
    }

    /// Get all enemy pieces
    pub fn enemy_pieces(&self) -> Bitboard {
        match self.turn {
            Color::White => self.bp | self.bn | self.bb | self.br | self.bq | self.bk,
            Color::Black => self.wp | self.wn | self.wb | self.wr | self.wq | self.wk,
        }
    }
}

impl Default for Board {
    fn default() -> Self {
        Self::new()
    }
}

/// Helper functions for bitboard manipulation
pub fn pop_count(bb: Bitboard) -> u32 {
    bb.count_ones()
}

pub fn file(sq: usize) -> usize {
    sq % 8
}

pub fn rank(sq: usize) -> usize {
    sq / 8
}

pub fn is_on_board(file: i32, rank: i32) -> bool {
    file >= 0 && file < 8 && rank >= 0 && rank < 8
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_color_flip() {
        assert_eq!(Color::White.flip(), Color::Black);
        assert_eq!(Color::Black.flip(), Color::White);
    }

    #[test]
    fn test_move_display() {
        let m = Move::new(0, 8);
        assert_eq!(m.to_string(), "a1a2");
    }

    #[test]
    fn test_pop_count() {
        assert_eq!(pop_count(0b1111), 4);
        assert_eq!(pop_count(0b0000), 0);
    }
}
