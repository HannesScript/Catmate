#pragma once

#include <iostream>
#include <string>
#include <regex>
#include <bits/stdc++.h>
#include <cstdint>
#include <climits>
#include <cstdlib>
#include <sstream>
#include <random>
#include <future>
#include <array>
#include <unordered_map>
#include <chrono>

#include "../constants.cpp"
#include "../util.cpp"
#include "../board.cpp"
#include "../move_gen.cpp"
#include "../transposition.cpp"
#include "../evaluate.cpp"
#include "../zobrist.cpp"
#include "../database.cpp"
#include "../search.cpp"

#include "history.cpp"

using namespace std;
using Bitboard = uint64_t;

class CatmateAPI
{
private:
    Board board = parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Color playingAs = Black;
    int depth = 20;
    GameHistory history = GameHistory();

    void findMove()
    {
        std::string fen = toFENString(board); // generate current FEN

        vector<string> possibleDBMoves;
        istringstream dbStream(db);
        string line;
        while (getline(dbStream, line))
        {
            if (line.empty() || line[0] == '*')
                continue;
            if (line.find(fen) == 0)
            {
                size_t pos = line.find(" : ");
                if (pos != string::npos && pos + 3 < line.size())
                    possibleDBMoves.push_back(line.substr(pos + 3));
            }
        }

        Move bestMove;
        if (!possibleDBMoves.empty())
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, possibleDBMoves.size() - 1);
            bestMove = bestMove.fromString(possibleDBMoves[dis(gen)]); // pick a random move from the DB moves
            cout << bestMove.toString() << endl;
            board = applyMove(&board, bestMove);
            history.push(&bestMove, 0, {});
        }
        else
        {
            bestMove = findBestMove(board, depth);
            cout << bestMove.toString() << endl;
            board = applyMove(&board, bestMove);
            history.push(&bestMove, 0, {});
        }
    }

public:
    CatmateAPI()
    {
        std::cout << "Catmate started..." << std::endl;
    }
    ~CatmateAPI() {}

    void processCmd(const std::string &command)
    {
        std::regex fenReg("-fen (.*)");
        std::smatch fenMatch;
        if (std::regex_match(command, fenMatch, fenReg))
        {
            board = parseFEN(fenMatch[1]);
        }

        std::regex playAsReg("-playAs (white|black|both)");
        std::smatch playAsMatch;
        if (std::regex_match(command, playAsMatch, playAsReg))
        {
            string playAs = playAsMatch[1];
            if (playAs == "white")
                playingAs = White;
            else if (playAs == "black")
                playingAs = Black;
            else if (playAs == "both")
                playingAs = Both;
            std::cout << "Playing as: " << playAs << std::endl;
            return;
        }

        std::regex depthReg("-depth ([0-9]+)");
        std::smatch depthMatch;
        if (std::regex_match(command, depthMatch, depthReg))
        {
            stringstream stream;
            stream << depthMatch[1];
            stream >> depth;
            return;
        }

        if (command == "-start")
        {
            std::cout << "Game started." << std::endl;
            if (playingAs == Black && board.Turn == Black)
            {
                findMove();
            }
            else if (playingAs == White && board.Turn == White)
            {
                findMove();
            }
            else if (playingAs == Both)
            {
                /*
This behavior suggests that the move-generation logic is always selecting the best move for white—even when the turn should alternate—because the board’s turn isn’t being updated as expected when findBestMove is invoked.

A couple of areas to check:

findBestMove Implementation:
Make sure that the function is using the board’s current turn (board.Turn) to generate moves. If it’s hardcoded or biased toward white’s moves, it will always return a white move, regardless of the actual turn.

Turn Update Flow:
In your loop you manually toggle the turn after applying the move:

Verify that applyMove doesn’t already modify board.Turn internally. If it does, your manual toggle might be causing an unexpected behavior. Ensure there’s a single, clear place where the turn is set based on the move.

Board State Mutation:
Check that board is updated properly. For instance, passing &board to applyMove might inadvertently lead to unexpected state if applyMove isn’t handling the pointer correctly.

In summary, the engine appears to select white’s moves because either the evaluation in findBestMove is always from white’s perspective or the board state
(specifically board.Turn) isn’t correctly propagated. Double-check that the turn used for move generation correctly reflects the alternation you expect.
                */
                while (true)
                {
                    findMove();
                }
            }

            return;
        }

        std::regex playReg("-play (.*)");
        std::smatch playMatch;
        if (std::regex_match(command, playMatch, playReg))
        {
            Move move;
            move = move.fromString(playMatch[1]);

            board = applyMove(&board, move);
            history.push(&move, 0, {});

            std::cout << toFENString(board) << std::endl;

            // Catmate's turn
            findMove();

            return;
        }
    }
};
