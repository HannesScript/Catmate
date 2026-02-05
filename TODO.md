# Catmate Chess Engine - TODO List

## Core Board Representation
- [x] Bitboard representation for all piece types
- [x] Board struct with white and black pieces
- [x] Color enum (White/Black) with flip functionality
- [x] Move struct with from/to squares
- [x] Helper functions (file, rank, is_on_board)
- [x] Occupied squares calculation
- [x] Friendly/enemy pieces identification
- [x] Move display in UCI notation (e.g., "e2e4")

## Move Generation
- [x] Pawn move generation (single push, double push)
- [x] Pawn capture generation (left and right captures)
- [x] Knight move generation
- [x] Bishop move generation (diagonal sliding)
- [x] Rook move generation (horizontal/vertical sliding)
- [x] Queen move generation (all sliding directions)
- [x] King move generation (one square in all directions)
- [ ] En passant capture
- [ ] Castling (kingside and queenside)
- [ ] Pawn promotion
- [ ] Legal move validation (checking if move leaves king in check)

## Position Evaluation
- [x] Material evaluation (piece values)
- [x] Piece-square tables for positional evaluation
  - [x] Pawn table
  - [x] Knight table
  - [x] Bishop table
  - [x] Rook table
  - [x] Queen table
  - [x] King opening table
  - [x] King endgame table
- [x] Endgame detection (based on piece count)
- [x] Checkmate detection and scoring
- [x] Stalemate detection
- [x] Draw detection (stalemate + insufficient material)
- [x] Insufficient material detection
- [x] Check detection and bonus
- [x] Castling bonus (detected by king position)
- [x] Fork detection and scoring
- [ ] Pin detection
- [ ] Skewer detection
- [ ] Discovered attack detection
- [ ] Passed pawn evaluation
- [ ] King safety evaluation
- [ ] Pawn structure evaluation (doubled, isolated, backward pawns)
- [ ] Mobility evaluation
- [ ] Center control evaluation
- [ ] Development evaluation (piece activity in opening)

## Search Algorithm
- [x] Minimax algorithm
- [x] Alpha-beta pruning
- [x] Move ordering (MVV-LVA - Most Valuable Victim, Least Valuable Attacker)
- [x] Best move finder with configurable depth
- [ ] Iterative deepening
- [ ] Transposition table
- [ ] Quiescence search
- [ ] Null move pruning
- [ ] Late move reduction
- [ ] Aspiration windows
- [ ] Principal variation search
- [ ] Killer move heuristic
- [ ] History heuristic
- [ ] Search extensions (check extension, pawn to 7th rank, etc.)

## Game State Management
- [x] Apply move (modify board)
- [x] Undo move (restore board state)
- [x] Captured piece tracking for undo
- [x] Turn switching
- [ ] Castling rights tracking
- [ ] En passant square tracking
- [ ] Halfmove clock (for 50-move rule)
- [ ] Fullmove number tracking
- [ ] Threefold repetition detection
- [ ] Zobrist hashing for position comparison

## FEN Support
- [x] Parse FEN string to board state
- [x] Parse piece placement
- [x] Parse active color
- [ ] Parse castling rights from FEN
- [ ] Parse en passant square from FEN
- [ ] Parse halfmove clock from FEN
- [ ] Parse fullmove number from FEN
- [ ] Generate FEN string from board state

## Opening Book
- [x] Opening book database (database.txt)
- [x] Database parser
- [x] FEN to opening moves lookup
- [x] Caching with OnceLock
- [x] Integration in main search routine
- [x] Support for multiple openings:
  - [x] Queen's Gambit
  - [x] King's Gambit
  - [x] Italian Game
  - [x] Ruy Lopez (Spanish Opening)
  - [x] Caro-Kann Defense
  - [x] Scandinavian Defense
  - [x] French Defense
  - [x] Sicilian Defense
  - [x] Nimzo-Indian Defense
  - [x] King's Indian Defense
- [ ] Expand opening book with more variations
- [ ] Opening book randomization (select from multiple book moves)

## CLI Interface
- [x] Command-line argument parsing
- [x] FEN input from command line
- [x] Depth parameter
- [x] UCI move output
- [x] Error handling for invalid input
- [ ] UCI protocol support (full implementation)
- [ ] Interactive mode
- [ ] Move history display
- [ ] Board visualization in terminal
- [ ] Performance statistics (nodes searched, time taken, etc.)

## Code Organization
- [x] Modular structure (board, moves, search, evaluate, utils, fen, database)
- [x] Public API exports in lib.rs
- [x] Unit tests for core functions
- [ ] Integration tests
- [ ] Benchmark suite
- [ ] Documentation (rustdoc comments)
- [ ] Example usage in README

## Performance Optimizations
- [x] Bitboard operations for fast move generation
- [x] Move ordering for better alpha-beta pruning
- [x] Cached database lookup
- [ ] Bitboard magic numbers for sliding pieces
- [ ] Parallel search (lazy SMP)
- [ ] SIMD optimizations
- [ ] Profile-guided optimization

## Special Rules & Edge Cases
- [ ] En passant (generation and execution)
- [ ] Castling (kingside and queenside)
- [ ] Pawn promotion (to Queen, Rook, Bishop, Knight)
- [ ] 50-move rule
- [ ] Threefold repetition
- [ ] Insufficient material (various combinations)
- [ ] Handling of illegal positions

## Testing & Quality
- [x] Basic unit tests (move display, color flip, etc.)
- [x] FEN parsing test
- [x] Move generation test for starting position
- [ ] Perft testing (move generation verification)
- [ ] Tactical test suite (mate in N, etc.)
- [ ] EPD test suite support
- [ ] Integration with chess problem databases
- [ ] Fuzzing for robustness

## Build & Deployment
- [x] Cargo.toml configuration
- [x] Debug build
- [x] Release build
- [x] Build script (test_engine.sh)
- [ ] Cross-platform compatibility testing
- [ ] CI/CD pipeline
- [ ] Release packaging
- [ ] Performance benchmarks in CI

## Future Enhancements
- [ ] Neural network evaluation (NNUE)
- [ ] Endgame tablebase support (Syzygy)
- [ ] Multi-PV (principal variation) output
- [ ] Chess960 (Fischer Random) support
- [ ] Time management
- [ ] Pondering (thinking on opponent's time)
- [ ] GUI integration (e.g., with python frontend)
- [ ] Web assembly build for browser play
- [ ] ELO rating estimation
- [ ] Self-play for training
- [ ] Game analysis mode
- [ ] PGN (Portable Game Notation) support
