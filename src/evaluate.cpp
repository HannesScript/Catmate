#ifndef EVALUATE
#define EVALUATE

#include "util.cpp"
#include "constants.cpp"
#include "board.cpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <algorithm> // For std::max and std::min

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

// Passed pawn evaluation helper.
// For each pawn, we build a mask representing the squares ahead (and diagonally adjacent).
// If none of these squares contains an enemy pawn, the pawn is passed.
inline int evaluatePassedPawns(Bitboard pawns, Bitboard enemyPawns, bool white)
{
    int bonus = 0;
    while (pawns)
    {
        int sq = trailingZeros(pawns);
        int rank = sq / 8;
        int file = sq % 8;
        Bitboard blockMask = 0;
        if (white)
        {
            for (int r = rank + 1; r < 8; r++)
            {
                int fmin = std::max(0, file - 1);
                int fmax = std::min(7, file + 1);
                for (int f = fmin; f <= fmax; f++)
                {
                    blockMask |= (1ULL << (r * 8 + f));
                }
            }
            // Bonus increases with advancement.
            if ((enemyPawns & blockMask) == 0)
            {
                bonus += rank * 10;
            }
        }
        else
        {
            for (int r = rank - 1; r >= 0; r--)
            {
                int fmin = std::max(0, file - 1);
                int fmax = std::min(7, file + 1);
                for (int f = fmin; f <= fmax; f++)
                {
                    blockMask |= (1ULL << (r * 8 + f));
                }
            }
            // For black, bonus increases as the pawn advances down the board.
            if ((enemyPawns & blockMask) == 0)
            {
                bonus += (7 - rank) * 10;
            }
        }
        pawns &= pawns - 1; // Clear the lowest set pawn
    }
    return bonus;
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
    
    // Add bonus for passed white pawns.
    score += evaluatePassedPawns(b->wp, b->bp, true);

    // Evaluate black pieces (subtracting the values).
    score -= evaluatePawn(b->bp, endgame);
    score -= evaluatePiece(b->bn, pieceValues["n"], knightTable);
    score -= evaluatePiece(b->bb, pieceValues["b"], bishopTable);
    score -= evaluatePiece(b->br, pieceValues["r"], rookTable);
    score -= evaluatePiece(b->bq, pieceValues["q"], queenTable);
    score -= evaluateKing(b->bk, endgame);

    // Subtract bonus for passed black pawns.
    score -= evaluatePassedPawns(b->bp, b->wp, false);

    return score;
}

#endif
