import chess
import pygame
import asyncio
from catmate import run

# GUI-Konfiguration
BOARD_SIZE = 800
INPUT_HEIGHT = 40  # Höhe des Eingabefelds
SQUARE_SIZE = BOARD_SIZE // 8
FPS = 30

# Farben
LIGHT_BROWN = (240, 217, 181)
DARK_BROWN = (170, 126, 89)
SELECT_COLOR = (0, 255, 0)

# Tiefe der Suche
DEPTH = 6

def load_piece_images():
    """Lädt und gibt ein Dictionary mit den Figurenbildern zurück."""
    images = {}
    for piece in "prnbqkPRNBQK":
        filename = f"assets/b{piece}.png" if piece.islower() else f"assets/w{piece.lower()}.png"
        images[piece] = pygame.image.load(filename)
    return images

def draw_board(screen, board, piece_images):
    """Zeichnet das Schachbrett und die Figuren."""
    for rank in range(8):
        for file in range(8):
            rect = pygame.Rect(file * SQUARE_SIZE, rank * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE)
            color = LIGHT_BROWN if (rank + file) % 2 == 0 else DARK_BROWN
            pygame.draw.rect(screen, color, rect)

            square = chess.square(file, 7 - rank)
            piece = board.piece_at(square)
            if piece:
                image = piece_images[piece.symbol()]
                scaled = pygame.transform.scale(image, (int(SQUARE_SIZE * 0.7), int(SQUARE_SIZE * 0.7)))
                image_rect = scaled.get_rect(center=rect.center)
                screen.blit(scaled, image_rect)

async def fetch_best_move(board):
    """Ruft den besten Zug von der Catmate-Engine ab."""
    best_move_uci = await run(executable_path="../bin/Catmate.exe", board=board, depth=DEPTH)
    print("Best move:", best_move_uci)
    return chess.Move.from_uci(best_move_uci)

def main():
    pygame.init()
    # Fenstergröße anpassen, sodass unten ein Eingabefeld enthalten ist
    screen = pygame.display.set_mode((BOARD_SIZE, BOARD_SIZE + INPUT_HEIGHT))
    pygame.display.set_caption("User vs Catmate")
    clock = pygame.time.Clock()
    piece_images = load_piece_images()

    board = chess.Board()
    player_turn = board.turn  # Spieler als Weiß

    user_input = ""
    font = pygame.font.Font(None, 32)

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            # Nur weiße Züge per Tastatureingabe, wenn es an der Reihe des Spielers ist
            elif player_turn and event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN:
                    try:
                        move = chess.Move.from_uci(user_input)
                    except Exception:
                        move = None
                    if move and move in board.legal_moves:
                        board.push(move)
                        player_turn = False
                    else:
                        print("Illegaler Zug:", user_input)
                    user_input = ""
                elif event.key == pygame.K_BACKSPACE:
                    user_input = user_input[:-1]
                else:
                    user_input += event.unicode

        # Zeichne Schachbrett
        draw_board(screen, board, piece_images)

        # Zeichne das Eingabefeld unten
        input_box_rect = pygame.Rect(0, BOARD_SIZE, BOARD_SIZE, INPUT_HEIGHT)
        pygame.draw.rect(screen, (255, 255, 255), input_box_rect)
        text_surface = font.render(user_input, True, (0, 0, 0))
        screen.blit(text_surface, (input_box_rect.x + 5, input_box_rect.y + 5))
        pygame.draw.rect(screen, (0, 0, 0), input_box_rect, 2)

        pygame.display.flip()

        if board.is_game_over():
            print("Spiel beendet:", board.result())
            running = False
        elif not player_turn:
            best_move = asyncio.run(fetch_best_move(board))
            if best_move in board.legal_moves:
                board.push(best_move)
            player_turn = True

        clock.tick(FPS)

    pygame.quit()

if __name__ == "__main__":
    main()
