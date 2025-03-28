#pragma once

#include <cstdint>
#include "util.cpp"
#include "board.cpp"
#include <random>

const int NUM_PIECES = 12; // e.g. WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK
uint64_t zobristTable[NUM_PIECES][64];
uint64_t zobristTurn;

// Initialize the Zobrist table with random 64-bit values.
void initZobrist()
{
    std::mt19937_64 rng(915378694376); // fixed seed for reproducibility or use random_device for randomness
    std::uniform_int_distribution<uint64_t> dist;
    for (int p = 0; p < NUM_PIECES; p++)
    {
        for (int sq = 0; sq < 64; sq++)
        {
            zobristTable[p][sq] = dist(rng);
        }
    }
    zobristTurn = dist(rng);
}

// Compute the Zobrist hash for a board.
uint64_t computeZobrist(const Board &b)
{
    uint64_t h = 0;

    auto processBitboard = [&](Bitboard bb, int pieceIdx) {
        while (bb)
        {
            int sq = trailingZeros(bb);
            h ^= zobristTable[pieceIdx][sq];
            bb &= bb - 1;
        }
    };

    // Process White Pieces
    processBitboard(b.wp, 0);
    processBitboard(b.wn, 1);
    processBitboard(b.wb, 2);
    processBitboard(b.wr, 3);
    processBitboard(b.wq, 4);
    processBitboard(b.wk, 5);

    // Process Black Pieces
    processBitboard(b.bp, 6);
    processBitboard(b.bn, 7);
    processBitboard(b.bb, 8);
    processBitboard(b.br, 9);
    processBitboard(b.bq, 10);
    processBitboard(b.bk, 11);

    // Add turn.
    if (b.Turn == White)
        h ^= zobristTurn;
    return h;
}
