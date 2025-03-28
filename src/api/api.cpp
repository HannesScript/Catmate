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
        // Compute the board's Zobrist hash
        uint64_t boardHash = computeZobrist(board);
        auto it = db.find(boardHash);
        Move bestMove;
        if (it != db.end())
        {
            const vector<string>& possibleDBMoves = it->second;
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
