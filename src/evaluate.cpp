#ifndef EVALUATE
#define EVALUATE

#include "util.cpp"
#include "constants.cpp"
#include "board.cpp"

#include <array>
#include <cstdint>
#include <iostream>

using Bitboard = uint64_t;

// Inline helper that assumes trailingZeros is optimized (or can use __builtin_ctzll)
inline int evaluatePiece(Bitboard bb, int value, const std::array<int, 64> &table)
{
    int score = 0;
    // Loop while there are bits set.
    while (bb)
    {
        // Use an intrinsic if available for the trailing-zero count:
        int sq = trailingZeros(bb);
        score += value + table[sq];
        bb &= bb - 1; // Clear the lowest set bit.
    }
    return score;
}

inline int evaluateKing(Bitboard bb, bool endgame)
{
    if (bb == 0)
    {
        // King missing: huge penalty (signals checkmate).
        return eventValues["checkmate"];
    }
    // If more than one bit is set, warn and pick the first.
    if (bb & (bb - 1))
    {
        std::cerr << "Warning: king bitboard has multiple bits set" << std::endl;
    }
    int sq = trailingZeros(bb);
    return endgame ? kingEndgameTable[sq] : kingOpeningTable[sq];
}

inline int evaluatePawn(Bitboard bb, bool endgame)
{
    if (bb == 0)
    {
        // Pawn missing (signals capture)
        return eventValues["capture"];
    }

    int sq = trailingZeros(bb);
    return endgame ? pawnEndTable[sq] : pawnTable[sq];
}

int evaluateBoard(const Board *b)
{
    int score = 0;
    // Count non-king pieces for endgame detection.
    int nonKingCount = popCount(b->wp) + popCount(b->wn) + popCount(b->wb) +
                       popCount(b->wr) + popCount(b->wq) +
                       popCount(b->bp) + popCount(b->bn) + popCount(b->bb) +
                       popCount(b->br) + popCount(b->bq);
    bool endgame = (nonKingCount <= 12);

    // Evaluate white pieces.
    score += evaluatePawn(b->wp, endgame);
    score += evaluatePiece(b->wn, pieceValues["n"], knightTable);
    score += evaluatePiece(b->wb, pieceValues["b"], bishopTable);
    score += evaluatePiece(b->wr, pieceValues["r"], rookTable);
    score += evaluatePiece(b->wq, pieceValues["q"], queenTable);
    score += evaluateKing(b->wk, endgame);

    // Evaluate black pieces (subtracting the values).
    score -= evaluatePawn(b->bp, endgame);
    score -= evaluatePiece(b->bn, pieceValues["n"], knightTable);
    score -= evaluatePiece(b->bb, pieceValues["b"], bishopTable);
    score -= evaluatePiece(b->br, pieceValues["r"], rookTable);
    score -= evaluatePiece(b->bq, pieceValues["q"], queenTable);
    score -= evaluateKing(b->bk, endgame);

    return score;
}

#endif