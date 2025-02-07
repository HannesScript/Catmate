import math
import json
import chess
from util import cdb_to_dict, dict_to_cdb

# Piece-Square Tables
pawnTable = [
	0, 5, 5, -10, -10, 5, 5, 0,
	0, 10, -5, 0, 0, -5, 10, 0,
	0, 10, 10, 20, 20, 10, 10, 0,
	0, 5, 10, 25, 25, 10, 5, 0,
	5, 10, 20, 30, 30, 20, 10, 5,
	10, 20, 30, 40, 40, 30, 20, 10,
	50, 50, 50, 50, 50, 50, 50, 50,
	0, 0, 0, 0, 0, 0, 0, 0
]

knightTable = [
	-50, -40, -30, -30, -30, -30, -40, -50,
	-40, -20, 0, 0, 0, 0, -20, -40,
	-30, 0, 10, 15, 15, 10, 0, -30,
	-30, 5, 15, 20, 20, 15, 5, -30,
	-30, 0, 15, 20, 20, 15, 0, -30,
	-30, 5, 10, 15, 15, 10, 5, -30,
	-40, -20, 0, 5, 5, 0, -20, -40,
	-50, -40, -30, -30, -30, -30, -40, -50
]

bishopTable = [
	-20, -10, -10, -10, -10, -10, -10, -20,
	-10, 5, 0, 0, 0, 0, 5, -10,
	-10, 10, 10, 10, 10, 10, 10, -10,
	-10, 0, 10, 10, 10, 10, 0, -10,
	-10, 5, 5, 10, 10, 5, 5, -10,
	-10, 0, 5, 10, 10, 5, 0, -10,
	-10, 0, 0, 0, 0, 0, 0, -10,
	-20, -10, -10, -10, -10, -10, -10, -20
]

rookTable = [
	0, 0, 0, 5, 5, 0, 0, 0,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	5, 10, 10, 10, 10, 10, 10, 5,
	0, 0, 0, 0, 0, 0, 0, 0
]

queenTable = [
	-20, -10, -10, -5, -5, -10, -10, -20,
	-10, 0, 0, 0, 0, 0, 0, -10,
	-10, 0, 5, 5, 5, 5, 0, -10,
	-5, 0, 5, 5, 5, 5, 0, -5,
	0, 0, 5, 5, 5, 5, 0, -5,
	-10, 5, 5, 5, 5, 5, 0, -10,
	-10, 0, 5, 0, 0, 0, 0, -10,
	-20, -10, -10, -5, -5, -10, -10, -20
]

kingOpeningTable = [
	20, 30, 10, 0, 0, 10, 30, 20,
	20, 20, 0, 0, 0, 0, 20, 20,
	-10, -20, -20, -20, -20, -20, -20, -10,
	-20, -30, -30, -40, -40, -30, -30, -20,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30
]

kingEndgameTable = [
	-50, -30, -30, -30, -30, -30, -30, -50,
	-30, -30, 0, 0, 0, 0, -30, -30,
	-30, -10, 20, 30, 30, 20, -10, -30,
	-30, -10, 30, 40, 40, 30, -10, -30,
	-30, -10, 30, 40, 40, 30, -10, -30,
	-30, -10, 20, 30, 30, 20, -10, -30,
	-30, -20, -20, 0, 0, -20, -20, -30,
	-50, -40, -30, -20, -20, -30, -40, -50
]

pieceValues = {
    "P": 1,
    "N": 3,
    "B": 3,
    "R": 5,
    "Q": 9,
    "K": 1000,
}

DB = False

def getBoardFEN(board):
    return board.board_fen() + " " + ("w" if board.turn else "b")

# LoadDB(FILENAMES)
def loadDB(*filenames):
    db = []
    for filename in filenames:
        db += cdb_to_dict(filename)
    return db 

def get_move(db, inp):
    if not db:
        return ""
    return db[inp] if inp in db else ""

def evaluate_board(b):
    board_fen = getBoardFEN(b)
    score = 0
    # if fewer than or equal to 12 non-king pieces remain, call it endgame
    pieces_count = 0
    for row in board_fen.split('/'):
        for c in row:
            if c != ' ' and c.lower() != 'k':
                pieces_count += 1
    endgame = (pieces_count <= 12)
    is_white = b.turn  # True if it's white's turn, False otherwise
    for i in range(8):
        for j in range(8):
            square = chess.square(j, 7 - i)
            piece_obj = b.piece_at(square)
            piece = piece_obj.symbol() if piece_obj else ' '
            index = i * 8 + j
            if piece == ' ':
                continue
            value = pieceValues.get(piece.upper(), 0)
            if piece.isupper():
                score += value
            else:
                score -= value
            if piece.upper() == 'P':
                score += pawnTable[index] if piece.isupper() else -pawnTable[index]
            elif piece.upper() == 'N':
                score += knightTable[index] if piece.isupper() else -knightTable[index]
            elif piece.upper() == 'B':
                score += bishopTable[index] if piece.isupper() else -bishopTable[index]
            elif piece.upper() == 'R':
                score += rookTable[index] if piece.isupper() else -rookTable[index]
            elif piece.upper() == 'Q':
                score += queenTable[index] if piece.isupper() else -queenTable[index]
            elif piece.upper() == 'K':
                if endgame:
                    score += kingEndgameTable[index] if piece.isupper() else -kingEndgameTable[index]
                else:
                    score += kingOpeningTable[index] if piece.isupper() else -kingOpeningTable[index]
    return score if is_white else -score

def negamax(board, depth, alpha, beta, color):
    pos_fen = getBoardFEN(board)
    db_move = get_move(DB, pos_fen)
    if db_move:
        board.push_uci(db_move)
        val = color * evaluate_board(board)
        board.pop()
        return val
    
    score = evaluate_board(board)
    if depth == 0 or board.is_game_over():
        return color * score

    max_value = -math.inf
    moves = sorted(board.legal_moves, key=lambda move: board.is_capture(move) or board.gives_check(move), reverse=True)
    for move in moves:
        board.push(move)
        value = -negamax(board, depth - 1, -beta, -alpha, -color)
        board.pop()
        
        if value > max_value:
            max_value = value
        alpha = max(alpha, value)
        
        if alpha >= beta:
            break
    return max_value

def find_best_move(board, depth):
    best_move = None
    best_score = -math.inf
    alpha, beta = -math.inf, math.inf
    color = 1 if board.turn else -1
    moves = sorted(board.legal_moves, key=lambda move: board.is_capture(move) or board.gives_check(move), reverse=True)
    
    for move in moves:
        board.push(move)
        score = -negamax(board, depth - 1, -beta, -alpha, -color)
        board.pop()
    
        if score > best_score:
            best_score = score
            best_move = move
            
        alpha = max(alpha, score)
        if alpha >= beta:
            break
    return best_move

#def log_move(board: chess.Board, move):
#    with open("log.log", "a") as f:
#        f.write(f"{board.fen()} : {move}\n")

def prepare(*filenames):
    global DB
    DB = loadDB(*filenames)

def run(board: chess.Board, depth=8):
    global DB
    board_fen = getBoardFEN(board)
    candidate = get_move(DB, board_fen)
    if candidate:
        return candidate

    board = chess.Board(board.board_fen())
    move = find_best_move(board, depth)
    if move is None:
        return False

    move_uci = move.uci()
    return move_uci
