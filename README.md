# Catmate

Catmate is a free and fast UCI Chess Engine.

## Compile from Source

1. Download the source code to your local device.
2. Compile via `cargo build --release --target-dir ./build/` (make sure you have Rust and Cargo installed)
3. Now you will find an executable named Catmate in `./build/`.
4. To use Catmate now you have to execute `<PATH_TO_CATMATE_EXECUTABLE> "<FEN_STRING_WITHOUT_EN_PASSANT_AND_MOVE_COUNT>" <THINKING_DEPTH>`
   e.g. `./build/release/catmate "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq" 10`
