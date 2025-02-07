# Catmate

Catmate is a free and fast UCI Chess Engine.

## Compile from Source

1. Download the source code to your local device.
2. Switch into `/src`
3. Compile via `go build -o ../bin` (make sure you have GO installed)
4. Now you will find an executable named Catmate in `/bin`.
5. To use Catmate now you have to execute 
   `<PATH_TO_CATMATE_EXECUTABLE> "<FEN_STRING_WITHOUT_EN_PASSANT_AND_MOVE_COUNT>" <THINKIN_DEPTH> "<PATH_TO_CDB_DATABASE>"`
   e.g. `./Catmate.exe "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq" 8  "./database.cdb"` (if you don't wan't to use a database you can set it to `"None"` e.g.
   `./Catmate.exe "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq" 8  "None"`)

## Use  with Python

To use Catmate with Python you first have to compile Catmate from source or download a released executable.

Once you've done that, you can import from `catmate.py`, the function `run`,  in your Python program. And call
`run(board=chess.Board, depth=<depth>, db="<path_to_cdb_database>")` (Note: You must use a board of type `chess.Board`, which can be imported by installing `chess` via PIP and importing it like this: `import chess` or
 `from chess import Board` (In this case you have to write `Board` instead of `chess.Board`)! )
