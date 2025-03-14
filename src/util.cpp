#pragma once

#include <cctype>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "constants.cpp"
#include "board.cpp"
#include "transposition.cpp"

// Inline helper functions for critical bit operations.
inline int popCount(Bitboard bb) {
    return __builtin_popcountll(bb);
}

inline int trailingZeros(Bitboard bb) {
    return __builtin_ctzll(bb);
}

Bitboard occupied(const Board &b) {
    return b.wp | b.wn | b.wb | b.wr | b.wq | b.wk |
           b.bp | b.bn | b.bb | b.br | b.bq | b.bk;
}

// ----- FEN Parsing ----- //
Board parseFEN(const std::string &fen) {
    Board board = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, White};
    std::istringstream iss(fen);
    std::string boardPart;
    iss >> boardPart;
    
    int rank = 7;
    int file = 0;
    for (char c : boardPart) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (std::isdigit(c)) {
            file += c - '0';
        } else {
            int square = rank * 8 + file;
            uint64_t bit = 1ULL << square;
            switch (c) {
                case 'P': board.wp |= bit; break;
                case 'N': board.wn |= bit; break;
                case 'B': board.wb |= bit; break;
                case 'R': board.wr |= bit; break;
                case 'Q': board.wq |= bit; break;
                case 'K': board.wk |= bit; break;
                case 'p': board.bp |= bit; break;
                case 'n': board.bn |= bit; break;
                case 'b': board.bb |= bit; break;
                case 'r': board.br |= bit; break;
                case 'q': board.bq |= bit; break;
                case 'k': board.bk |= bit; break;
                default: break;
            }
            file++;
        }
    }
    
    std::string turn;
    iss >> turn;
    board.Turn = (turn == "b") ? Black : White;
    return board;
}

// ----- TT Storing and Loading ----- //
void storeTranspositionTable(std::unordered_map<uint64_t, TTEntry> &tt, const std::string &path) {
    std::ofstream file(path);
    for (auto &entry : tt) {
        file << entry.first << " " 
             << entry.second.depth << " " 
             << entry.second.value << " " 
             << entry.second.flag << " " 
             << entry.second.bestMove << "\n";
    }
}

std::unordered_map<uint64_t, TTEntry> loadTranspositionTable(const std::string &path) {
    std::unordered_map<uint64_t, TTEntry> tt;
    std::ifstream file(path);
    uint64_t hash;
    int depth, value, flag;
    std::string bestMove;
    while (file >> hash >> depth >> value >> flag >> bestMove) {
        tt[hash] = {depth, value, (TTFlag)flag, bestMove};
    }
    return tt;
}
