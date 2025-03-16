#ifndef CONSTANTS
#define CONSTANTS

#include <array>
#include <string>
#include <unordered_map>
#include <cstdint>

using Bitboard = uint64_t;

// Evaluation tables (64-element arrays)
const std::array<int, 64> pawnTable = {{0, 5, 5, -10, -10, 5, 5, 0,
                                        0, 10, -5, 0, 0, -5, 10, 0,
                                        0, 10, 10, 20, 20, 10, 10, 0,
                                        0, 5, 10, 35, 35, 10, 5, 0,
                                        5, 10, 20, 35, 35, 20, 10, 30,
                                        10, 20, 30, 15, 15, 30, 20, 20,
                                        50, 50, 15, 5, 5, 35, 50, 10,
                                        0, 0, 0, 0, 0, 0, 0, 0}};

const std::array<int, 64> pawnEndTable = {{50, 50, 50, 50, 50, 50, 50, 50,
                                           45, 45, 45, 45, 45, 45, 45, 45,
                                           40, 40, 40, 40, 40, 40, 40, 40,
                                           35, 35, 35, 35, 35, 35, 35, 35,
                                           20, 20, 20, 20, 20, 20, 20, 20,
                                           10, 10, 10, 10, 10, 10, 10, 10,
                                           5, 5, 5, 5, 5, 5, 5, 5,
                                           0, 0, 0, 0, 0, 0, 0, 0}};

const std::array<int, 64> knightTable = {{-50, -40, -30, -30, -30, -30, -40, -50,
                                          -40, -20, 0, 0, 0, 0, -20, -40,
                                          -30, 0, 10, 15, 15, 10, 0, -30,
                                          -30, 5, 15, 20, 20, 15, 5, -30,
                                          -30, 0, 15, 20, 20, 15, 0, -30,
                                          -30, 5, 10, 15, 15, 10, 5, -30,
                                          -40, -20, 0, 5, 5, 0, -20, -40,
                                          -50, -40, -30, -30, -30, -30, -40, -50}};

const std::array<int, 64> bishopTable = {{-20, -10, -10, -10, -10, -10, -10, -20,
                                          -10, 5, 0, 0, 0, 0, 5, -10,
                                          -10, 10, 10, 10, 10, 10, 10, -10,
                                          -10, 0, 10, 10, 10, 10, 0, -10,
                                          -10, 5, 5, 10, 10, 5, 5, -10,
                                          -10, 0, 5, 10, 10, 5, 0, -10,
                                          -10, 0, 0, 0, 0, 0, 0, -10,
                                          -20, -10, -10, -10, -10, -10, -10, -20}};

const std::array<int, 64> rookTable = {{0, 0, 0, 5, 5, 0, 0, 0,
                                        -5, 0, 0, 0, 0, 0, 0, -5,
                                        -5, 0, 0, 0, 0, 0, 0, -5,
                                        -5, 0, 0, 0, 0, 0, 0, -5,
                                        -5, 0, 0, 0, 0, 0, 0, -5,
                                        -5, 0, 0, 0, 0, 0, 0, -5,
                                        5, 10, 10, 10, 10, 10, 10, 5,
                                        0, 0, 0, 0, 0, 0, 0, 0}};

const std::array<int, 64> queenTable = {{-20, -10, -10, -5, -5, -10, -10, -20,
                                         -10, 0, 0, 0, 0, 0, 0, -10,
                                         -10, 0, 5, 5, 5, 5, 0, -10,
                                         -5, 0, 5, 5, 5, 5, 0, -5,
                                         0, 0, 5, 5, 5, 5, 0, -5,
                                         -10, 5, 5, 5, 5, 5, 0, -10,
                                         -10, 0, 5, 0, 0, 0, 0, -10,
                                         -20, -10, -10, -5, -5, -10, -10, -20}};

const std::array<int, 64> kingOpeningTable = {{20, 30, 10, 0, 0, 10, 30, 20,
                                               20, 20, 0, 0, 0, 0, 20, 20,
                                               -10, -20, -20, -20, -20, -20, -20, -10,
                                               -20, -30, -30, -40, -40, -30, -30, -20,
                                               -30, -40, -40, -50, -50, -40, -40, -30,
                                               -30, -40, -40, -50, -50, -40, -40, -30,
                                               -30, -40, -40, -50, -50, -40, -40, -30,
                                               -30, -40, -40, -50, -50, -40, -40, -30}};

const std::array<int, 64> kingEndgameTable = {{-50, -30, -30, -30, -30, -30, -30, -50,
                                               -30, -30, 0, 0, 0, 0, -30, -30,
                                               -30, -10, 20, 30, 30, 20, -10, -30,
                                               -30, -10, 30, 40, 40, 30, -10, -30,
                                               -30, -10, 30, 40, 40, 30, -10, -30,
                                               -30, -10, 20, 30, 30, 20, -10, -30,
                                               -30, -20, -20, 0, 0, -20, -20, -30,
                                               -50, -40, -30, -20, -20, -30, -40, -50}};

// ----- Move Offset Tables ----- //
const std::array<int, 4> pawnOffsets = {{
    16,  // Double Push
    8,   // Single Push
    9, 7 // Captures Diagonally
}};

const std::array<int, 8> knightOffsets = {{-17, -15, -10, -6, 6, 10, 15, 17}};

const std::array<int, 4> bishopOffsets = {{-9, -7, 7, 9}};

const std::array<int, 4> rookOffsets = {{-8, -1, 1, 8}};

const std::array<int, 8> queenOffsets = {{-9, -8, -7, -1, 1, 7, 8, 9}};

const std::array<int, 8> kingOffsets = {{-9, -8, -7, -1, 1, 7, 8, 9}};

// ----- Values ----- //

std::unordered_map<std::string, int> pieceValues = {
    {"p", 10}, {"n", 32}, {"b", 33}, {"r", 50}, {"q", 90}, {"k", 0}};

std::unordered_map<std::string, int> eventValues = {
    {"checkmate", 2000},
    {"stalemate", 0},
    {"draw", 0},
    {"check", 50},
    {"promotion", 100},
    {"castling", 50},
    {"fork", 60},
    {"capture", 10}};

// ----- Move Types ----- //

struct Move
{
    int from;
    int to;
    // Inline conversion using a fixed-size char buffer.
    inline std::string toString() const
    {
        char s[5];
        s[0] = 'a' + (from % 8);
        s[1] = '1' + (from / 8);
        s[2] = 'a' + (to % 8);
        s[3] = '1' + (to / 8);
        s[4] = '\0';
        return std::string(s);
    }
};

inline bool operator==(const Move &a, const Move &b)
{
    return a.from == b.from && a.to == b.to;
}

#endif
