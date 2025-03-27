#pragma once

#include <vector>
#include <cstdint>
#include <cmath>
#include <cassert>
#include "board.cpp"
#include "util.cpp"

using namespace std;
using Bitboard = uint64_t;

enum Piece { Pawn, Knight, Bishop, Rook, Queen, King };

const int pawnOffsets[4]  = {16, 8, 9, 7};
const int knightOffsets[8]= {17, 15, 10, 6, -6, -10, -15, -17};
const int bishopOffsets[4]= {9, 7, -7, -9};
const int rookOffsets[4]  = {8, -8, 1, -1};
const int queenOffsets[8] = {8, -8, 1, -1, 9, 7, -7, -9};
const int kingOffsets[8]  = {8, -8, 1, -1, 9, 7, -7, -9};

// Square helper functions.
inline int fileOf(int sq) { return sq % 8; }
inline int rankOf(int sq) { return sq / 8; }

// Forward declarations.
bool kingIsInCheck(const Board *b, int side);

// Templated sliding-move generator to reduce duplicate code.
template<typename Condition>
void generateSlidingMoves(const Board* b, Bitboard pieces, const int* offsets, int numOffsets,
                          Bitboard friendOcc, Bitboard enemyOcc, vector<Move>& moves,
                          Condition condition) {
    while (pieces) {
        int fromSquare = trailingZeros(pieces);
        for (int i = 0; i < numOffsets; ++i) {
            int offset = offsets[i];
            int toSquare = fromSquare;
            while (true) {
                toSquare += offset;
                if (toSquare < 0 || toSquare >= 64)
                    break;
                if (!condition(fromSquare, toSquare, offset))
                    break;
                if (friendOcc & (1ULL << toSquare))
                    break;
                moves.push_back({fromSquare, toSquare, '\0'});
                if (enemyOcc & (1ULL << toSquare))
                    break;
            }
        }
        pieces &= pieces - 1;
    }
}

vector<Move> generatePseudoLegalMoves(const Board *b) {
    vector<Move> moves;
    Bitboard occ = occupied(*b);

    Bitboard friendOcc, enemyOcc, pawns, knights, bishops, rooks, queens, kingBB;
    bool isWhite = (b->Turn == White);
    if (isWhite) {
        friendOcc = b->wp | b->wn | b->wb | b->wr | b->wq | b->wk;
        enemyOcc  = b->bp | b->bn | b->bb | b->br | b->bq | b->bk;
        // White pawn moves
        pawns = b->wp;
        while (pawns) {
            int fromSquare = trailingZeros(pawns);
            int r = rankOf(fromSquare), f = fileOf(fromSquare);
            int toSq = fromSquare + pawnOffsets[1]; // single push
            if (toSq < 64 && !(occ & (1ULL << toSq))) {
                if (rankOf(toSq) == 7) {
                    for (char promo : {'q', 'r', 'b', 'n'})
                        moves.push_back({fromSquare, toSq, promo});
                } else {
                    moves.push_back({fromSquare, toSq, '\0'});
                }
                if (r == 1) { // double push
                    int toSq2 = fromSquare + pawnOffsets[0];
                    if (toSq2 < 64 && !(occ & (1ULL << toSq2)))
                        moves.push_back({fromSquare, toSq2, '\0'});
                }
            }
            if (f > 0) {
                int capSq = fromSquare + pawnOffsets[3];
                if (capSq < 64 && rankOf(capSq) == r + 1 && (enemyOcc & (1ULL << capSq))) {
                    if (rankOf(capSq) == 7) {
                        for (char promo : {'q', 'r', 'b', 'n'})
                            moves.push_back({fromSquare, capSq, promo});
                    } else {
                        moves.push_back({fromSquare, capSq, '\0'});
                    }
                }
            }
            if (f < 7) {
                int capSq = fromSquare + pawnOffsets[2];
                if (capSq < 64 && rankOf(capSq) == r + 1 && (enemyOcc & (1ULL << capSq))) {
                    if (rankOf(capSq) == 7) {
                        for (char promo : {'q', 'r', 'b', 'n'})
                            moves.push_back({fromSquare, capSq, promo});
                    } else {
                        moves.push_back({fromSquare, capSq, '\0'});
                    }
                }
            }
            pawns &= pawns - 1;
        }
        // White knight moves
        knights = b->wn;
        while (knights) {
            int fromSquare = trailingZeros(knights);
            int f = fileOf(fromSquare);
            for (int offset : knightOffsets) {
                int toSquare = fromSquare + offset;
                if (toSquare < 0 || toSquare >= 64)
                    continue;
                if (abs(fileOf(toSquare) - f) > 2)
                    continue;
                if (friendOcc & (1ULL << toSquare))
                    continue;
                moves.push_back({fromSquare, toSquare, '\0'});
            }
            knights &= knights - 1;
        }
        // White sliding moves: bishops, rooks, queens.
        bishops = b->wb;
        generateSlidingMoves(b, bishops, bishopOffsets, 4, friendOcc, enemyOcc, moves,
            [](int from, int to, int) -> bool {
                return abs(fileOf(to) - fileOf(from)) == abs(rankOf(to) - rankOf(from));
            });
        rooks = b->wr;
        generateSlidingMoves(b, rooks, rookOffsets, 4, friendOcc, enemyOcc, moves,
            [](int from, int to, int offset) -> bool {
                if (offset == 1 || offset == -1)
                    return rankOf(to) == rankOf(from);
                return fileOf(to) == fileOf(from);
            });
        queens = b->wq;
        generateSlidingMoves(b, queens, queenOffsets, 8, friendOcc, enemyOcc, moves,
            [](int from, int to, int offset) -> bool {
                if (offset == 1 || offset == -1)
                    return rankOf(to) == rankOf(from);
                if (offset == 8 || offset == -8)
                    return fileOf(to) == fileOf(from);
                return abs(fileOf(to) - fileOf(from)) == abs(rankOf(to) - rankOf(from));
            });
        // White king moves
        kingBB = b->wk;
        if (kingBB) {
            int fromSquare = trailingZeros(kingBB);
            int f = fileOf(fromSquare);
            for (int offset : kingOffsets) {
                int toSquare = fromSquare + offset;
                if (toSquare < 0 || toSquare >= 64)
                    continue;
                if (abs(fileOf(toSquare) - f) > 1)
                    continue;
                if (friendOcc & (1ULL << toSquare))
                    continue;
                moves.push_back({fromSquare, toSquare, '\0'});
            }
        }
    } else {
        friendOcc = b->bp | b->bn | b->bb | b->br | b->bq | b->bk;
        enemyOcc  = b->wp | b->wn | b->wb | b->wr | b->wq | b->wk;
        // Black pawn moves
        pawns = b->bp;
        while (pawns) {
            int fromSquare = trailingZeros(pawns);
            int r = rankOf(fromSquare), f = fileOf(fromSquare);
            int toSq = fromSquare - pawnOffsets[1]; // single push
            if (toSq >= 0 && !(occ & (1ULL << toSq))) {
                if (rankOf(toSq) == 0) {
                    for (char promo : {'q', 'r', 'b', 'n'})
                        moves.push_back({fromSquare, toSq, promo});
                } else {
                    moves.push_back({fromSquare, toSq, '\0'});
                }
                if (r == 6) { // double push
                    int toSq2 = fromSquare - pawnOffsets[0];
                    if (toSq2 >= 0 && !(occ & (1ULL << toSq2)))
                        moves.push_back({fromSquare, toSq2, '\0'});
                }
            }
            if (f > 0) {
                int capSq = fromSquare - pawnOffsets[2];
                if (capSq >= 0 && rankOf(capSq) == r - 1 && (enemyOcc & (1ULL << capSq))) {
                    if (rankOf(capSq) == 0) {
                        for (char promo : {'q', 'r', 'b', 'n'})
                            moves.push_back({fromSquare, capSq, promo});
                    } else {
                        moves.push_back({fromSquare, capSq, '\0'});
                    }
                }
            }
            if (f < 7) {
                int capSq = fromSquare - pawnOffsets[3];
                if (capSq >= 0 && rankOf(capSq) == r - 1 && (enemyOcc & (1ULL << capSq))) {
                    if (rankOf(capSq) == 0) {
                        for (char promo : {'q', 'r', 'b', 'n'})
                            moves.push_back({fromSquare, capSq, promo});
                    } else {
                        moves.push_back({fromSquare, capSq, '\0'});
                    }
                }
            }
            pawns &= pawns - 1;
        }
        // Black knight moves
        knights = b->bn;
        while (knights) {
            int fromSquare = trailingZeros(knights);
            int f = fileOf(fromSquare);
            for (int offset : knightOffsets) {
                int toSquare = fromSquare + offset;
                if (toSquare < 0 || toSquare >= 64)
                    continue;
                if (abs(fileOf(toSquare) - f) > 2)
                    continue;
                if (friendOcc & (1ULL << toSquare))
                    continue;
                moves.push_back({fromSquare, toSquare, '\0'});
            }
            knights &= knights - 1;
        }
        // Black sliding moves: bishops, rooks, queens.
        bishops = b->bb;
        generateSlidingMoves(b, bishops, bishopOffsets, 4, friendOcc, enemyOcc, moves,
            [](int from, int to, int) -> bool {
                return abs(fileOf(to) - fileOf(from)) == abs(rankOf(to) - rankOf(from));
            });
        rooks = b->br;
        generateSlidingMoves(b, rooks, rookOffsets, 4, friendOcc, enemyOcc, moves,
            [](int from, int to, int offset) -> bool {
                if (offset == 1 || offset == -1)
                    return rankOf(to) == rankOf(from);
                return fileOf(to) == fileOf(from);
            });
        queens = b->bq;
        generateSlidingMoves(b, queens, queenOffsets, 8, friendOcc, enemyOcc, moves,
            [](int from, int to, int offset) -> bool {
                if (offset == 1 || offset == -1)
                    return rankOf(to) == rankOf(from);
                if (offset == 8 || offset == -8)
                    return fileOf(to) == fileOf(from);
                return abs(fileOf(to) - fileOf(from)) == abs(rankOf(to) - rankOf(from));
            });
        // Black king moves
        kingBB = b->bk;
        if (kingBB) {
            int fromSquare = trailingZeros(kingBB);
            int f = fileOf(fromSquare);
            for (int offset : kingOffsets) {
                int toSquare = fromSquare + offset;
                if (toSquare < 0 || toSquare >= 64)
                    continue;
                if (abs(fileOf(toSquare) - f) > 1)
                    continue;
                if (friendOcc & (1ULL << toSquare))
                    continue;
                moves.push_back({fromSquare, toSquare, '\0'});
            }
        }
    }
    return moves;
}

bool kingIsInCheck(const Board *b, int side) {
    Board board = *b;
    board.Turn = (side == White ? Black : White);
    vector<Move> possibleMovesByOpponent = generatePseudoLegalMoves(&board);
    for (Move &m : possibleMovesByOpponent) {
        Board temp = applyMove(&board, m);
        if ((side == White && temp.wk == 0ULL) ||
            (side == Black && temp.bk == 0ULL))
            return true;
    }
    return false;
}

vector<Move> generateMoves(const Board *b) {
    vector<Move> pseudoMoves = generatePseudoLegalMoves(b);
    vector<Move> legalMoves;
    for (auto &m : pseudoMoves) {
        Board board = *b;
        Board temp = applyMove(&board, m);

        if (!kingIsInCheck(&temp, b->Turn))
            legalMoves.push_back(m);
    }
    return legalMoves;
}
