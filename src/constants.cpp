#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <cstdint>

using Bitboard = uint64_t;

// Evaluation tables (64-element arrays)
const std::array<int, 64> pawnTable = {{
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 15, 5, 5, 35, 50, 10,
    10, 20, 30, 15, 15, 30, 20, 20,
    5, 10, 20, 35, 35, 20, 10, 30,
    0, 5, 10, 35, 35, 10, 5, 0,
    0, 10, 10, 20, 20, 10, 10, 0,
    0, 10, -5, 0, 0, -5, 10, 0,
    0, 5, 5, -10, -10, 5, 5, 0
}};

const std::array<int, 64> pawnEndTable = {{
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 5, 5, 5, 5, 5, 5, 5,
    10, 10, 10, 10, 10, 10, 10, 10,
    20, 20, 20, 20, 20, 20, 20, 20,
    35, 35, 35, 35, 35, 35, 35, 35,
    40, 40, 40, 40, 40, 40, 40, 40,
    45, 45, 45, 45, 45, 45, 45, 45,
    50, 50, 50, 50, 50, 50, 50, 50
}};

const std::array<int, 64> knightTable = {{
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
}};

const std::array<int, 64> bishopTable = {{
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
}};

const std::array<int, 64> rookTable = {{
    0,  0,  3,  5,  5,  3,  0,  0,
   -10,  0,  0,  0,  0,  0,  0, -10,
   -10,  0,  0,  0,  0,  0,  0, -10,
   -10,  0,  0,  0,  0,  0,  0, -10,
   -10,  0,  0,  0,  0,  0,  0, -10,
    -5,  0,  0,  0,  0,  0,  0, -5,
    10, 10, 10, 10, 10, 10, 10, 10,
    0,  0,  0,  0,  0,  0,  0,  0
}};

const std::array<int, 64> queenTable = {{
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10,  0,   5,  0,  0,  0,   0, -10,
    -10,  5,   5,  5,  5,  5,   0, -10,
     0,   0,   5,  5,  5,  5,   0, -5,
    -5,   0,   5,  5,  5,  5,   0, -5,
    -10,  0,   5,  5,  5,  5,   0, -10,
    -10,  0,   0,  0,  0,  0,   0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20
}};

const std::array<int, 64> kingOpeningTable = {{
    20,  30,  10,   0,   0,  10,  30,  20,
    20,  20,   0,   0,   0,   0,  20,  20,
   -10, -20, -20, -20, -20, -20, -20, -10,
   -20, -30, -30, -40, -40, -30, -30, -20,
   -30, -40, -40, -50, -50, -40, -40, -30,
   -30, -40, -40, -50, -50, -40, -40, -30,
   -30, -40, -40, -50, -50, -40, -40, -30,
   -30, -40, -40, -50, -50, -40, -40, -30
}};

const std::array<int, 64> kingEndgameTable = {{
   -50, -30, -30, -30, -30, -30, -30, -50,
   -30, -30,   0,   0,   0,   0, -30, -30,
   -30, -10,  20,  30,  30,  20, -10, -30,
   -30, -10,  30,  40,  40,  30, -10, -30,
   -30, -10,  30,  40,  40,  30, -10, -30,
   -30, -10,  20,  30,  30,  20, -10, -30,
   -30, -20, -20,   0,   0, -20, -20, -30,
   -50, -40, -30, -20, -20, -30, -40, -50
}};

// ----- Values ----- //

std::unordered_map<std::string, int> pieceValues = {
    {"p", 10}, {"n", 30}, {"b", 32}, {"r", 50}, {"q", 90}, {"k", 0}};

std::unordered_map<std::string, int> eventValues = {
    {"checkmate", 20000},
    {"stalemate", 0},
    {"draw", 0},
    {"check", 500},
    {"promotion", 1000},
    {"castling", 500},
    {"fork", 600},
    {"capture", 100}};

// ----- Move Types ----- //

struct Move {
    int from;
    int to;
    char promo; // '\0' if no promotion; else 'q', 'r', 'b', or 'n'
    inline std::string toString() const {
        char s[6];
        s[0] = 'a' + (from % 8);
        s[1] = '1' + (from / 8);
        s[2] = 'a' + (to % 8);
        s[3] = '1' + (to / 8);
        s[4] = promo ? promo : ' '; // show promotion if any
        s[5] = '\0';
        return std::string(s);
    }

    inline Move fromString(std::string uciMove) const {
        Move move;

        auto squareFrom = [](char file, char rank) -> int {
            return (rank - '1') * 8 + (file - 'a');
        };
        move.from = squareFrom(uciMove[0], uciMove[1]);
        move.to = squareFrom(uciMove[2], uciMove[3]);
        move.promo = (uciMove.size() == 5) ? uciMove[4] : '\0';

        return move;
    }
};

inline bool operator==(const Move &a, const Move &b)
{
    return a.from == b.from && a.to == b.to && a.promo == b.promo;
}
