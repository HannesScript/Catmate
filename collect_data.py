import chess
import chess.pgn
import chess.engine
import random
import json
import logging
from typing import List, Dict, Any
import hashlib

logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s")

games = []

def get_board_fen(board: chess.Board) -> str:
    fen = board.board_fen() + " " + ("w" if board.turn else "b") + " "
    if board.has_kingside_castling_rights(chess.WHITE):
        fen += "K"
    if board.has_queenside_castling_rights(chess.WHITE):
        fen += "Q"
    if board.has_kingside_castling_rights(chess.BLACK):
        fen += "k"
    if board.has_queenside_castling_rights(chess.BLACK):
        fen += "q"
    if not (board.has_kingside_castling_rights(chess.WHITE) or board.has_queenside_castling_rights(chess.WHITE) or board.has_kingside_castling_rights(chess.BLACK) or board.has_queenside_castling_rights(chess.BLACK)):
        fen += "-"
    return fen

def add_move(filename: str, inp: str, outp: str) -> None:
    try:
        with open(filename, 'a', encoding='utf-8') as f:
            f.write(f"{inp} : {outp}\n")
    except Exception as e:
        logging.error("Error saving database: %s", e)

def run_stockfish_vs_stockfish(engine_path: str) -> None:
    if not games:
        logging.error("No games found in the PGN file.")
        return

    selected_game = random.choice(games)
    # Check if selected game is stored in games_played_sf.txt
    # If it is, select another game
    # If it isn't, add it to games_played_sf.txt and play it
    
    def get_game_signature(game: chess.pgn.Game) -> str:
        exporter = chess.pgn.StringExporter(headers=True, variations=False, comments=False)
        pgn_str = game.accept(exporter)
        return hashlib.sha256(pgn_str.encode('utf-8')).hexdigest()

    try:
        with open("games_played_sf.txt", 'r', encoding='utf-8') as f:
            played_games = {line.strip() for line in f}
    except FileNotFoundError:
        played_games = set()

    attempts = 0
    while get_game_signature(selected_game) in played_games and attempts < 10:
        selected_game = random.choice(games)
        attempts += 1

    game_signature = get_game_signature(selected_game)
    with open("games_played_sf.txt", 'a', encoding='utf-8') as f:
        f.write(game_signature + "\n")
    
    moves = list(selected_game.mainline_moves())
    cutoff_index = random.randint(0, len(moves))
    
    board = chess.Board()
    for move in moves[:cutoff_index]:
        board.push(move)

    logging.info("Starting engine play from move %d.", cutoff_index)
    
    try:
        with chess.engine.SimpleEngine.popen_uci(engine_path) as engine:
            while not board.is_game_over():
                result = engine.play(board, limit=chess.engine.Limit(time=0.1))
                board_fen = get_board_fen(board)
                move_uci = result.move.uci()
                logging.info("For Board FEN '%s' Stockfish played '%s'", board_fen, move_uci)
                add_move("stockfish_moves.cdb", board_fen, move_uci)
                board.push(result.move)
    except Exception as e:
        logging.error("Error during engine play: %s", e)

if __name__ == "__main__":
    with open("lichess-data/lichess_db_standard_rated_2013-01.pgn", 'r', encoding='utf-8') as f:
            games = []
            while True:
                game = chess.pgn.read_game(f)
                if game is None:
                    break
                games.append(game)
    for i in range(len(games)):
        run_stockfish_vs_stockfish("train/stockfish-17.exe")
