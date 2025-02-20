import subprocess
import chess

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


async def run(executable_path: str, board: chess.Board, depth: int) -> str:
    try:
        fen = get_board_fen(board=board)
        command = f"{executable_path} \"{fen}\" {depth}"
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        output, error = process.communicate()
        print(f"Catmate output: {output.decode('utf-8').strip()}")
        return output.decode("utf-8").strip()
    except Exception as e:
        print(f"Error running Catmate: {e}")
        return ""
