use std::sync::OnceLock;
use std::collections::HashMap;

pub const DATABASE: &str = include_str!("database.txt");
static DB_CACHE: OnceLock<HashMap<String, Vec<String>>> = OnceLock::new();

/// Parse the opening book database and return a HashMap of FEN -> Vec<moves>
pub fn parse_database() -> HashMap<String, Vec<String>> {
    let mut db = HashMap::new();

    for line in DATABASE.lines() {
        if line.trim().is_empty() || line.starts_with('*') {
            continue;
        }

        if let Some((fen, moves_part)) = line.split_once(" : ") {
            let fen = fen.trim().to_string();
            let moves: Vec<String> = moves_part
                .trim()
                .split_whitespace()
                .map(|s| s.to_string())
                .collect();

            db.entry(fen)
                .or_insert_with(Vec::new)
                .extend(moves);
        }
    }

    db
}

/// Get opening moves for a given FEN position
pub fn get_opening_moves(fen: &str) -> Option<Vec<String>> {
    let db = DB_CACHE.get_or_init(parse_database);
    db.get(fen).cloned()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_database() {
        let db = parse_database();
        assert!(!db.is_empty());
    }
}
