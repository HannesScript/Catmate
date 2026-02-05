use std::env;
use std::process;

use catmate::Move;
use catmate::fen;
use catmate::search;
use catmate::database;

fn convert_uci_to_move(uci: &str) -> Option<Move> {
    if uci.len() < 4 {
        return None;
    }

    let from_file = (uci.as_bytes()[0] - b'a') as usize;
    let from_rank = (uci.as_bytes()[1] - b'1') as usize;
    let to_file = (uci.as_bytes()[2] - b'a') as usize;
    let to_rank = (uci.as_bytes()[3] - b'1') as usize;

    let from_sq = from_rank * 8 + from_file;
    let to_sq = to_rank * 8 + to_file;

    if from_sq < 64 && to_sq < 64 {
        Some(Move::new(from_sq, to_sq))
    } else {
        None
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() != 3 {
        eprintln!("Usage: catmate \"<FEN>\" <depth>");
        eprintln!("Example: catmate \"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq\" 6");
        process::exit(1);
    }

    let fen = &args[1];
    let depth: i32 = match args[2].parse() {
        Ok(d) => d,
        Err(_) => {
            eprintln!("Error: depth must be an integer");
            process::exit(1);
        }
    };

    // Parse the board
    let board = fen::parse_fen(fen);

    // Try to get a move from the opening book
    if let Some(moves) = database::get_opening_moves(fen) {
        if !moves.is_empty() {
            // Use the first move from the opening book
            if let Some(mv) = convert_uci_to_move(&moves[0]) {
                println!("{}", mv);
                return;
            }
        }
    }

    // Otherwise, use minimax to find the best move
    let best_move = search::find_best_move(board, depth);
    println!("{}", best_move);
}
