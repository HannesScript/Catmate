#ifndef BOARD
#define BOARD

#include <cstdint>
#include <array>
#include "constants.cpp"

typedef uint64_t Bitboard;

enum Color
{ White, Black };

struct Board
{
    Bitboard wp, wn, wb, wr, wq, wk;
    Bitboard bp, bn, bb, br, bq, bk;
    Color Turn;
};

inline Board applyMove(Board b, const Move &move)
{
    Bitboard fromMask = 1ULL << move.from;
    Bitboard toMask   = 1ULL << move.to;

    if(b.Turn == White)
    {
        // Remove any captured black piece.
        std::array<Bitboard*, 6> blackPieces = { &b.bp, &b.bn, &b.bb, &b.br, &b.bq, &b.bk };
        for(auto piece : blackPieces)
            *piece &= ~toMask;

        // Move the white piece.
        if(b.wp & fromMask)       b.wp = (b.wp & ~fromMask) | toMask;
        else if(b.wn & fromMask)  b.wn = (b.wn & ~fromMask) | toMask;
        else if(b.wb & fromMask)  b.wb = (b.wb & ~fromMask) | toMask;
        else if(b.wr & fromMask)  b.wr = (b.wr & ~fromMask) | toMask;
        else if(b.wq & fromMask)  b.wq = (b.wq & ~fromMask) | toMask;
        else if(b.wk & fromMask)  b.wk = (b.wk & ~fromMask) | toMask;
    }
    else
    {
        // Remove any captured white piece.
        std::array<Bitboard*, 6> whitePieces = { &b.wp, &b.wn, &b.wb, &b.wr, &b.wq, &b.wk };
        for(auto piece : whitePieces)
            *piece &= ~toMask;

        // Move the black piece.
        if(b.bp & fromMask)       b.bp = (b.bp & ~fromMask) | toMask;
        else if(b.bn & fromMask)  b.bn = (b.bn & ~fromMask) | toMask;
        else if(b.bb & fromMask)  b.bb = (b.bb & ~fromMask) | toMask;
        else if(b.br & fromMask)  b.br = (b.br & ~fromMask) | toMask;
        else if(b.bq & fromMask)  b.bq = (b.bq & ~fromMask) | toMask;
        else if(b.bk & fromMask)  b.bk = (b.bk & ~fromMask) | toMask;
    }
    
    // Change turn.
    b.Turn = (b.Turn == White) ? Black : White;
    return b;
}

#endif
