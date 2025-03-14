#ifndef MOVE_GEN
#define MOVE_GEN

#include <vector>
#include <bits/stdc++.h>

#include "board.cpp"
#include "util.cpp"

using namespace std;
using Bitboard = uint64_t;

vector<Move> generateMoves(const Board *b) {
    vector<Move> moves;
    Bitboard occ = occupied(*b);
    auto fileOf = [](int sq) { return sq % 8; };
    auto rankOf = [](int sq) { return sq / 8; };

    if(b->Turn == White) {
        Bitboard friendOcc = b->wp | b->wn | b->wb | b->wr | b->wq | b->wk;
        Bitboard enemyOcc  = b->bp | b->bn | b->bb | b->br | b->bq | b->bk;

        // White Pawn Moves using offsets
        {
            Bitboard pawns = b->wp;
            while(pawns) {
                int fromSquare = trailingZeros(pawns);
                int fromRank = rankOf(fromSquare);
                int fromFile = fileOf(fromSquare);
                // Single push (offset 8)
                int toSquare = fromSquare + pawnOffsets[1];
                if(toSquare < 64 && !(occ & (1ULL << toSquare))) {
                    moves.push_back({fromSquare, toSquare});
                    // Double push from starting rank (rank 2, index 1)
                    if(fromRank == 1) {
                        int toSquare2 = fromSquare + pawnOffsets[0]; // offset 16
                        if(toSquare2 < 64 && !(occ & (1ULL << toSquare2)))
                            moves.push_back({fromSquare, toSquare2});
                    }
                }
                // Captures:
                // Capture left (offset 7) if not on file 0
                if(fromFile > 0) {
                    int capSquare = fromSquare + pawnOffsets[3]; // 7
                    if(capSquare < 64 && rankOf(capSquare) == fromRank + 1 &&
                       (enemyOcc & (1ULL << capSquare)))
                        moves.push_back({fromSquare, capSquare});
                }
                // Capture right (offset 9) if not on file 7
                if(fromFile < 7) {
                    int capSquare = fromSquare + pawnOffsets[2]; // 9
                    if(capSquare < 64 && rankOf(capSquare) == fromRank + 1 &&
                       (enemyOcc & (1ULL << capSquare)))
                        moves.push_back({fromSquare, capSquare});
                }
                pawns &= pawns - 1;
            }
        }

        // White Knight Moves using knightOffsets
        {
            Bitboard knights = b->wn;
            while(knights) {
                int fromSquare = trailingZeros(knights);
                int fromFile = fileOf(fromSquare);
                for(auto offset : knightOffsets) {
                    int toSquare = fromSquare + offset;
                    if(toSquare < 0 || toSquare >= 64)
                        continue;
                    int toFile = fileOf(toSquare);
                    if(abs(toFile - fromFile) > 2)
                        continue;
                    if(friendOcc & (1ULL << toSquare))
                        continue;
                    moves.push_back({fromSquare, toSquare});
                }
                knights &= knights - 1;
            }
        }

        // White Bishop Moves using bishopOffsets (sliding)
        {
            Bitboard bishops = b->wb;
            while(bishops) {
                int fromSquare = trailingZeros(bishops);
                int fromFile = fileOf(fromSquare), fromRank = rankOf(fromSquare);
                for(auto offset : bishopOffsets) {
                    int toSquare = fromSquare;
                    while(true) {
                        toSquare += offset;
                        if(toSquare < 0 || toSquare >= 64)
                            break;
                        int toFile = fileOf(toSquare), toRank = rankOf(toSquare);
                        if(abs(toFile - fromFile) != abs(toRank - fromRank))
                            break;
                        if(friendOcc & (1ULL << toSquare))
                            break;
                        moves.push_back({fromSquare, toSquare});
                        if(enemyOcc & (1ULL << toSquare))
                            break;
                    }
                }
                bishops &= bishops - 1;
            }
        }

        // White Rook Moves using rookOffsets (sliding)
        {
            Bitboard rooks = b->wr;
            while(rooks) {
                int fromSquare = trailingZeros(rooks);
                int fromFile = fileOf(fromSquare), fromRank = rankOf(fromSquare);
                for(auto offset : rookOffsets) {
                    int toSquare = fromSquare;
                    while(true) {
                        toSquare += offset;
                        if(toSquare < 0 || toSquare >= 64)
                            break;
                        int toFile = fileOf(toSquare), toRank = rankOf(toSquare);
                        // For horizontal moves, remain on same rank;
                        if((offset == -1 || offset == 1) && toRank != fromRank)
                            break;
                        // For vertical moves, remain on same file;
                        if((offset == -8 || offset == 8) && toFile != fromFile)
                            break;
                        if(friendOcc & (1ULL << toSquare))
                            break;
                        moves.push_back({fromSquare, toSquare});
                        if(enemyOcc & (1ULL << toSquare))
                            break;
                    }
                }
                rooks &= rooks - 1;
            }
        }

        // White Queen Moves using queenOffsets (sliding)
        {
            Bitboard queens = b->wq;
            while(queens) {
                int fromSquare = trailingZeros(queens);
                int fromFile = fileOf(fromSquare), fromRank = rankOf(fromSquare);
                for(auto offset : queenOffsets) {
                    int toSquare = fromSquare;
                    while(true) {
                        toSquare += offset;
                        if(toSquare < 0 || toSquare >= 64)
                            break;
                        int toFile = fileOf(toSquare), toRank = rankOf(toSquare);
                        // For diagonal moves, ensure proper file/rank difference.
                        if(offset == -9 || offset == -7 || offset == 7 || offset == 9) {
                            if(abs(toFile - fromFile) != abs(toRank - fromRank))
                                break;
                        } else if(offset == -8 || offset == 8) {
                            if(toFile != fromFile)
                                break;
                        } else if(offset == -1 || offset == 1) {
                            if(toRank != fromRank)
                                break;
                        }
                        if(friendOcc & (1ULL << toSquare))
                            break;
                        moves.push_back({fromSquare, toSquare});
                        if(enemyOcc & (1ULL << toSquare))
                            break;
                    }
                }
                queens &= queens - 1;
            }
        }

        // White King Moves using kingOffsets (one step)
        {
            Bitboard king = b->wk;
            if(king) {
                int fromSquare = trailingZeros(king);
                int fromFile = fileOf(fromSquare);
                for(auto offset : kingOffsets) {
                    int toSquare = fromSquare + offset;
                    if(toSquare < 0 || toSquare >= 64)
                        continue;
                    int toFile = fileOf(toSquare);
                    if(abs(toFile - fromFile) > 1)
                        continue;
                    if(friendOcc & (1ULL << toSquare))
                        continue;
                    moves.push_back({fromSquare, toSquare});
                }
            }
        }
    } else {
        // Black Moves
        Bitboard friendOcc = b->bp | b->bn | b->bb | b->br | b->bq | b->bk;
        Bitboard enemyOcc  = b->wp | b->wn | b->wb | b->wr | b->wq | b->wk;

        // Black Pawn Moves using offsets (mirrored direction)
        {
            Bitboard pawns = b->bp;
            while(pawns) {
                int fromSquare = trailingZeros(pawns);
                int fromRank = rankOf(fromSquare);
                int fromFile = fileOf(fromSquare);
                // Single push (offset -8)
                int toSquare = fromSquare - pawnOffsets[1];
                if(toSquare >= 0 && !(occ & (1ULL << toSquare))) {
                    moves.push_back({fromSquare, toSquare});
                    // Double push from starting rank (rank 7, index 6)
                    if(fromRank == 6) {
                        int toSquare2 = fromSquare - pawnOffsets[0];
                        if(toSquare2 >= 0 && !(occ & (1ULL << toSquare2)))
                            moves.push_back({fromSquare, toSquare2});
                    }
                }
                // Captures:
                // Capture left for Black: moving down-left (offset -9)
                if(fromFile > 0) {
                    int capSquare = fromSquare - (int)pawnOffsets[2]; // -9
                    if(capSquare >= 0 && rankOf(capSquare) == fromRank - 1 &&
                       (enemyOcc & (1ULL << capSquare)))
                        moves.push_back({fromSquare, capSquare});
                }
                // Capture right for Black: moving down-right (offset -7)
                if(fromFile < 7) {
                    int capSquare = fromSquare - (int)pawnOffsets[3]; // -7
                    if(capSquare >= 0 && rankOf(capSquare) == fromRank - 1 &&
                       (enemyOcc & (1ULL << capSquare)))
                        moves.push_back({fromSquare, capSquare});
                }
                pawns &= pawns - 1;
            }
        }

        // Black Knight Moves using knightOffsets
        {
            Bitboard knights = b->bn;
            while(knights) {
                int fromSquare = trailingZeros(knights);
                int fromFile = fileOf(fromSquare);
                for(auto offset : knightOffsets) {
                    int toSquare = fromSquare + offset;
                    if(toSquare < 0 || toSquare >= 64)
                        continue;
                    int toFile = fileOf(toSquare);
                    if(abs(toFile - fromFile) > 2)
                        continue;
                    if(friendOcc & (1ULL << toSquare))
                        continue;
                    moves.push_back({fromSquare, toSquare});
                }
                knights &= knights - 1;
            }
        }

        // Black Bishop Moves using bishopOffsets (sliding)
        {
            Bitboard bishops = b->bb;
            while(bishops) {
                int fromSquare = trailingZeros(bishops);
                int fromFile = fileOf(fromSquare), fromRank = rankOf(fromSquare);
                for(auto offset : bishopOffsets) {
                    int toSquare = fromSquare;
                    while(true) {
                        toSquare += offset;
                        if(toSquare < 0 || toSquare >= 64)
                            break;
                        int toFile = fileOf(toSquare), toRank = rankOf(toSquare);
                        if(abs(toFile - fromFile) != abs(toRank - fromRank))
                            break;
                        if(friendOcc & (1ULL << toSquare))
                            break;
                        moves.push_back({fromSquare, toSquare});
                        if(enemyOcc & (1ULL << toSquare))
                            break;
                    }
                }
                bishops &= bishops - 1;
            }
        }

        // Black Rook Moves using rookOffsets (sliding)
        {
            Bitboard rooks = b->br;
            while(rooks) {
                int fromSquare = trailingZeros(rooks);
                int fromFile = fileOf(fromSquare), fromRank = rankOf(fromSquare);
                for(auto offset : rookOffsets) {
                    int toSquare = fromSquare;
                    while(true) {
                        toSquare += offset;
                        if(toSquare < 0 || toSquare >= 64)
                            break;
                        int toFile = fileOf(toSquare), toRank = rankOf(toSquare);
                        if((offset == -1 || offset == 1) && toRank != fromRank)
                            break;
                        if((offset == -8 || offset == 8) && toFile != fromFile)
                            break;
                        if(friendOcc & (1ULL << toSquare))
                            break;
                        moves.push_back({fromSquare, toSquare});
                        if(enemyOcc & (1ULL << toSquare))
                            break;
                    }
                }
                rooks &= rooks - 1;
            }
        }

        // Black Queen Moves using queenOffsets (sliding)
        {
            Bitboard queens = b->bq;
            while(queens) {
                int fromSquare = trailingZeros(queens);
                int fromFile = fileOf(fromSquare), fromRank = rankOf(fromSquare);
                for(auto offset : queenOffsets) {
                    int toSquare = fromSquare;
                    while(true) {
                        toSquare += offset;
                        if(toSquare < 0 || toSquare >= 64)
                            break;
                        int toFile = fileOf(toSquare), toRank = rankOf(toSquare);
                        if(offset == -9 || offset == -7 || offset == 7 || offset == 9) {
                            if(abs(toFile - fromFile) != abs(toRank - fromRank))
                                break;
                        } else if(offset == -8 || offset == 8) {
                            if(toFile != fromFile)
                                break;
                        } else if(offset == -1 || offset == 1) {
                            if(toRank != fromRank)
                                break;
                        }
                        if(friendOcc & (1ULL << toSquare))
                            break;
                        moves.push_back({fromSquare, toSquare});
                        if(enemyOcc & (1ULL << toSquare))
                            break;
                    }
                }
                queens &= queens - 1;
            }
        }

        // Black King Moves using kingOffsets (one step)
        {
            Bitboard king = b->bk;
            if(king) {
                int fromSquare = trailingZeros(king);
                int fromFile = fileOf(fromSquare);
                for(auto offset : kingOffsets) {
                    int toSquare = fromSquare + offset;
                    if(toSquare < 0 || toSquare >= 64)
                        continue;
                    int toFile = fileOf(toSquare);
                    if(abs(toFile - fromFile) > 1)
                        continue;
                    if(friendOcc & (1ULL << toSquare))
                        continue;
                    moves.push_back({fromSquare, toSquare});
                }
            }
        }
    }
    return moves;
}

#endif
