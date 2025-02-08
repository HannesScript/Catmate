import chess
import pygame
import numpy as np
import time
from catmate import run
from util import get_board_fen

# GUI-Konfiguration
BOARD_SIZE = 800
SQUARE_SIZE = BOARD_SIZE // 8
FPS = 30

# Farben
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
LIGHT_BROWN = (240, 217, 181)
DARK_BROWN = (170, 126, 89)
SELECT_COLOR = (0, 255, 0)

# Tiefe der Suche
DEPTH = 5

# Bilder der Figuren laden
PIECE_IMAGES = {}
for piece in "prnbqkPRNBQK":
    if piece.islower():
        PIECE_IMAGES[piece] = pygame.image.load(f"assets/b{piece}.png")
    else:
        PIECE_IMAGES[piece] = pygame.image.load(f"assets/w{piece.lower()}.png")

# GUI initialisieren
def draw_board(screen, board, selected_square=None):
    """Zeichnet das Schachbrett und die Figuren."""
    for rank in range(8):
        for file in range(8):
            rect = pygame.Rect(file * SQUARE_SIZE, rank * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE)
            color = LIGHT_BROWN if (rank + file) % 2 == 0 else DARK_BROWN
            pygame.draw.rect(screen, color, rect)

            if selected_square == chess.square(file, 7 - rank):
                pygame.draw.rect(screen, SELECT_COLOR, rect, 3)

            piece = board.piece_at(chess.square(file, 7 - rank))
            if piece:
                scaled_image = pygame.transform.scale(
                    PIECE_IMAGES[piece.symbol()],
                    (int(SQUARE_SIZE * 0.7), int(SQUARE_SIZE * 0.7))
                )
                image_rect = scaled_image.get_rect(center=rect.center)
                screen.blit(scaled_image, image_rect)

def main():
    pygame.init()
    screen = pygame.display.set_mode((BOARD_SIZE, BOARD_SIZE))
    pygame.display.set_caption("User vs Catmate")
    clock = pygame.time.Clock()

    # Initialisiere das Schachbrett
    board = chess.Board()
    selected_square = None
    player_turn = board.turn  # Assume player is white

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            #if player_turn:
            #    time.sleep(0.5)
            #    move = get_ai_move(model, board)
            #    if move in board.legal_moves:
            #        board.push(move)
            #    player_turn = False
            if player_turn and event.type == pygame.MOUSEBUTTONDOWN:
                x, y = event.pos
                file = x // SQUARE_SIZE
                rank = 7 - (y // SQUARE_SIZE)
                square = chess.square(file, rank)

                if selected_square is None:
                    piece = board.piece_at(square)
                    if piece and piece.color == chess.WHITE:
                        selected_square = square
                else:
                    move = chess.Move(selected_square, square)
                    if move in board.legal_moves:
                        board.push(move)
                        player_turn = False
                    selected_square = None

        # Zeichne das Schachbrett
        draw_board(screen, board, selected_square)
        pygame.display.flip()

        if not board.is_game_over() and not player_turn:
            time.sleep(0.5)
            move = chess.Move.from_uci(run(executable_path="../bin/Catmate.exe", fen_string=get_board_fen(board), depth=DEPTH, database="../data/db.cdb"))
            if move in board.legal_moves:
                board.push(move)
            player_turn = True
        elif board.is_game_over():
            print("Spiel beendet:", board.result())
            running = False

        clock.tick(FPS)

    pygame.quit()

if __name__ == "__main__":
        main()
