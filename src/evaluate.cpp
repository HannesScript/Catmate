#pragma once

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

    // --- Added blockage punishment ---
    // Lower rated friendly piece blocking an important square for a higher rated sliding piece
    // incurs a penalty. For white a penalty is subtracted and for black it is added (since black's
    // value is subtracted later).
    //
    // We'll check sliding pieces: rook, bishop, and queen. For each sliding piece, in each direction,
    // if the very first blocking friendly piece is lower rated than the sliding piece, we apply a penalty.
    // 
    // Helper lambda to get the value of a friendly piece at square 'sq' for the given color.
    auto getFriendlyPieceValue = [b](int sq, bool white) -> int {
        Bitboard mask = 1ULL << sq;
        if (white)
        {
            if (b->wp & mask) return pieceValues["p"];
            if (b->wn & mask) return pieceValues["n"];
            if (b->wb & mask) return pieceValues["b"];
            if (b->wr & mask) return pieceValues["r"];
            if (b->wq & mask) return pieceValues["q"];
            if (b->wk & mask) return pieceValues["k"];
        }
        else
        {
            if (b->bp & mask) return pieceValues["p"];
            if (b->bn & mask) return pieceValues["n"];
            if (b->bb & mask) return pieceValues["b"];
            if (b->br & mask) return pieceValues["r"];
            if (b->bq & mask) return pieceValues["q"];
            if (b->bk & mask) return pieceValues["k"];
        }
        return 0;
    };

    // Lambda to check sliding piece blockage for a set of pieces.
    // directions: array of directional offsets.
    // numDirs: number of directions.
    // highValue: value of the sliding piece.
    // penalty: penalty to apply per occurrence.
    auto evaluateBlockageSliding = [&](Bitboard pieces, int highValue, const int directions[], int numDirs, bool white) -> int {
        int penaltyAcc = 0;
        while (pieces)
        {
            int sq = trailingZeros(pieces);
            int r = sq / 8, f = sq % 8;
            // For each direction, ray-scan until hitting board edge.
            for (int d = 0; d < numDirs; d++)
            {
                int offset = directions[d];
                int curSq = sq;
                while (true)
                {
                    int nextSq = curSq + offset;
                    int nr = nextSq / 8, nf = nextSq % 8;
                    // Check boundaries. For horizontal moves, ensure rank remains the same change appropriately.
                    if (nextSq < 0 || nextSq >= 64)
                        break;
                    // For moves that change file, ensure we haven't wrapped around.
                    if (std::abs(nf - f) > 7)
                        break;
                    // For vertical moves, check that file difference is 0; for diagonal moves, the difference must be 1.
                    // A simple check: the difference in row should match the difference in column (for diagonals).
                    // We assume offset values are chosen so that illegal wrap-around is caught by file difference.
                    // If the move is legal, check occupancy.
                    Bitboard nextPos = (1ULL << nextSq);
                    // Check if any friendly piece occupies nextSq.
                    int pieceVal = getFriendlyPieceValue(nextSq, white);
                    if (pieceVal != 0)
                    {
                        // If lower rated than the sliding piece (and not a king)
                        if ((pieceVal != pieceValues["k"]) && (pieceVal < highValue))
                        {
                            penaltyAcc += 20; // fixed penalty for blockage
                        }
                        break; // stop scanning in this direction
                    }
                    // If occupied by enemy, we also stop.
                    // To check enemy occupancy, combine all friendly pieces and then invert.
                    Bitboard allPieces;
                    if (white)
                        allPieces = b->wp | b->wn | b->wb | b->wr | b->wq | b->wk;
                    else
                        allPieces = b->bp | b->bn | b->bb | b->br | b->bq | b->bk;
                    if (allPieces & nextPos)
                        break;
                    curSq = nextSq;
                }
            }
            pieces &= pieces - 1;
        }
        return penaltyAcc;
    };

    // Direction offsets for pieces.
    const int knightOffsets[8]= {17, 15, 10, 6, -6, -10, -15, -17};
    const int bishopOffsets[4]= {9, 7, -7, -9};
    const int rookOffsets[4]  = {8, -8, 1, -1};
    const int queenOffsets[8] = {8, -8, 1, -1, 9, 7, -7, -9};

    int whitePenalty = 0;
    int blackPenalty = 0;

    // Evaluate white sliding piece blockages.
    whitePenalty += evaluateBlockageSliding(b->wr, pieceValues["n"], knightOffsets, 4, true);
    whitePenalty += evaluateBlockageSliding(b->wr, pieceValues["r"], rookOffsets, 4, true);
    whitePenalty += evaluateBlockageSliding(b->wb, pieceValues["b"], bishopOffsets, 4, true);
    whitePenalty += evaluateBlockageSliding(b->wq, pieceValues["q"], queenOffsets, 8, true);

    // Evaluate black sliding piece blockages.
    blackPenalty += evaluateBlockageSliding(b->br, pieceValues["n"], knightOffsets, 4, false);
    blackPenalty += evaluateBlockageSliding(b->br, pieceValues["r"], rookOffsets, 4, false);
    blackPenalty += evaluateBlockageSliding(b->bb, pieceValues["b"], bishopOffsets, 4, false);
    blackPenalty += evaluateBlockageSliding(b->bq, pieceValues["q"], bishopOffsets, 8, false);

    // Subtract penalties from the side's score.
    score -= whitePenalty;
    score += blackPenalty; // since black's values are subtracted later

    return (b->Turn == White) ? score : -score;
}
