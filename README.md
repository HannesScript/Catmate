# Catmate

Catmate is a free and fast UCI Chess Engine.

## Compile from Source

1. Download the source code to your local device.
2. Switch into `/src/`
3. Compile via `g++ -std=c++17 -Ofast -Wall -DNDEBUG ./main.cpp -o ../bin/Catmate` (make sure you have GCC installed)
4. Now you will find an executable named Catmate in `/bin/`.
5. To use Catmate now you have to execute `<PATH_TO_CATMATE_EXECUTABLE> "<FEN_STRING_WITHOUT_EN_PASSANT_AND_MOVE_COUNT>" <THINKING_DEPTH> -tt "<TRANSPOSITION_TABLE_FILE>"` (note that `-tt <TRANSPOSITION_TABLE_FILE>` is optional) e.g. `./Catmate.exe "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq" 24 -tt "../transposition table.txt"`

<!--
## Use  with Python

To use Catmate with Python you first have to compile Catmate from source or download a released executable.

Once you've done that, you can import from `catmate.py`, the function `run`,  in your Python program. And call
`run(executable_path="<path_to_catmate_executable>", board=<chess.Board board>, depth=<depth>)` )
-->
