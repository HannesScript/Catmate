import chess
import chess.engine
import asyncio
from typing import Dict, List
from catmate import run

async def run_game(engine_path: str, opponent_path: str, time_limit: float) -> str:
    """
    Run a single game between the engine at engine_path and the local opponent.
    
    Returns:
        The game result in UCI ('1-0', '0-1', or '1/2-1/2').
    """
    
    opponent = await chess.engine.popen_uci(opponent_path)

    board = chess.Board()

    while not board.is_game_over():
        if board.turn == chess.WHITE:
            move_str = await run(engine_path, board, 20)
            board.push(board.parse_uci(move_str))
        else:
            result = await opponent.play(board, chess.engine.Limit(time=time_limit))
            board.push(result.move)

    await opponent.quit()
    return board.result()

async def gauntlet_tournament(catmate_path: str, opponents: List[str], games_per_opponent: int, time_limit: float) -> Dict[str, Dict[str, int]]:
    """
    Run a tournament for the given Catmate engine against multiple opponents.
    
    Returns:
        A dictionary containing results for each opponent.
    """
    results: Dict[str, Dict[str, int]] = {
        opponent: {'Catmate': 0, 'Opponent': 0, 'Draws': 0} for opponent in opponents
    }

    for opponent in opponents:
        for _ in range(games_per_opponent):
            game_result = await run_game(catmate_path, opponent, time_limit)
            if game_result == '1-0':
                results[opponent]['Catmate'] += 1
            elif game_result == '0-1':
                results[opponent]['Opponent'] += 1
            else:
                results[opponent]['Draws'] += 1

    return results

async def main() -> None:
    catmate_path = "../bin/Catmate.exe"
    opponents = ["./stockfish-17.exe"]
    games_per_opponent = 10
    time_limit = 2  # seconds

    tournament_results = await gauntlet_tournament(catmate_path, opponents, games_per_opponent, time_limit)
    print(tournament_results)

if __name__ == "__main__":
    asyncio.run(main())
