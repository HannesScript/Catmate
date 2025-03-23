#ifndef BOARD
#define BOARD

#include <cstdint>
#include "constants.cpp"

// Color enumeration
enum Color
{
    White,
    Black
};

// Board structure describing all bitboards and the side to move.
struct Board
{
    uint64_t wp, wn, wb, wr, wq, wk;
    uint64_t bp, bn, bb, br, bq, bk;
    Color Turn;
};

// Applies a move on the board and returns the new board state.
inline Board applyMove(Board &b, const Move &move)
{
    uint64_t fromMask = 1ULL << move.from;
    uint64_t toMask = 1ULL << move.to;

    // Remove captured pieces.
    if (b.Turn == White)
    {
        // Remove any black piece on destination.
        if (b.bp & toMask)
            b.bp &= ~toMask;
        else if (b.bn & toMask)
            b.bn &= ~toMask;
        else if (b.bb & toMask)
            b.bb &= ~toMask;
        else if (b.br & toMask)
            b.br &= ~toMask;
        else if (b.bq & toMask)
            b.bq &= ~toMask;
        else if (b.bk & toMask)
            b.bk &= ~toMask;

        // Move the white piece.
        if (b.wp & fromMask)
        {
            b.wp &= ~fromMask;
            if (move.promo != '\0')
            { // Apply promotion
                if (move.promo == 'q')
                    b.wq |= toMask;
                else if (move.promo == 'r')
                    b.wr |= toMask;
                else if (move.promo == 'b')
                    b.wb |= toMask;
                else if (move.promo == 'n')
                    b.wn |= toMask;
            }
            else
            {
                b.wp |= toMask;
            }
        }
        else if (b.wn & fromMask)
        {
            b.wn &= ~fromMask;
            b.wn |= toMask;
        }
        else if (b.wb & fromMask)
        {
            b.wb &= ~fromMask;
            b.wb |= toMask;
        }
        else if (b.wr & fromMask)
        {
            b.wr &= ~fromMask;
            b.wr |= toMask;
        }
        else if (b.wq & fromMask)
        {
            b.wq &= ~fromMask;
            b.wq |= toMask;
        }
        else if (b.wk & fromMask)
        {
            b.wk &= ~fromMask;
            b.wk |= toMask;
        }
    }
    else
    { // Black to move.
        // Remove any white piece on destination.
        if (b.wp & toMask)
            b.wp &= ~toMask;
        else if (b.wn & toMask)
            b.wn &= ~toMask;
        else if (b.wb & toMask)
            b.wb &= ~toMask;
        else if (b.wr & toMask)
            b.wr &= ~toMask;
        else if (b.wq & toMask)
            b.wq &= ~toMask;
        else if (b.wk & toMask)
            b.wk &= ~toMask;

        // Move the black piece.
        if (b.bp & fromMask)
        {
            b.bp &= ~fromMask;
            if (move.promo != '\0')
            { // Apply promotion
                if (move.promo == 'q')
                    b.bq |= toMask;
                else if (move.promo == 'r')
                    b.br |= toMask;
                else if (move.promo == 'b')
                    b.bb |= toMask;
                else if (move.promo == 'n')
                    b.bn |= toMask;
            }
            else
            {
                b.bp |= toMask;
            }
        }
        else if (b.bn & fromMask)
        {
            b.bn &= ~fromMask;
            b.bn |= toMask;
        }
        else if (b.bb & fromMask)
        {
            b.bb &= ~fromMask;
            b.bb |= toMask;
        }
        else if (b.br & fromMask)
        {
            b.br &= ~fromMask;
            b.br |= toMask;
        }
        else if (b.bq & fromMask)
        {
            b.bq &= ~fromMask;
            b.bq |= toMask;
        }
        else if (b.bk & fromMask)
        {
            b.bk &= ~fromMask;
            b.bk |= toMask;
        }
    }
    // Switch turn.
    b.Turn = (b.Turn == White) ? Black : White;
    return b;
}

#endif
