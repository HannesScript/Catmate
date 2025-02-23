#include <bits/stdc++.h>
#include <cstdint>
#include <climits>
#include <cstdlib>
#include <sstream>
#include <random>
#include <future>

#include "constants.cpp"
#include "database.cpp"

using namespace std;
using Bitboard = uint64_t;

// ----- Utility Functions ----- //
int popCount(Bitboard bb) {
    return __builtin_popcountll(bb);
}

int trailingZeros(Bitboard bb) {
    return __builtin_ctzll(bb);
}

Bitboard occupied(const Board &b) {
    return b.wp | b.wn | b.wb | b.wr | b.wq | b.wk |
           b.bp | b.bn | b.bb | b.br | b.bq | b.bk;
}

// ----- FEN Parsing ----- //
Board parseFEN(const string &fen) {
    Board board;
    istringstream iss(fen);
    vector<string> fields;
    string token;
    while (iss >> token)
        fields.push_back(token);
    vector<string> ranks;
    {
        istringstream rankStream(fields[0]);
        string rankToken;
        while(getline(rankStream, rankToken, '/'))
            ranks.push_back(rankToken);
    }
    // Process rows: top row is rank8 (r==0) to rank1 (r==7)
    for (size_t r = 0; r < ranks.size(); r++) {
        int file = 0;
        for (char c : ranks[r]) {
            if (c >= '1' && c <= '8') {
                file += c - '0';
            } else {
                int square = (7 - r) * 8 + file;
                switch (c) {
                    case 'P': board.wp |= (1ULL << square); break;
                    case 'N': board.wn |= (1ULL << square); break;
                    case 'B': board.wb |= (1ULL << square); break;
                    case 'R': board.wr |= (1ULL << square); break;
                    case 'Q': board.wq |= (1ULL << square); break;
                    case 'K': board.wk |= (1ULL << square); break;
                    case 'p': board.bp |= (1ULL << square); break;
                    case 'n': board.bn |= (1ULL << square); break;
                    case 'b': board.bb |= (1ULL << square); break;
                    case 'r': board.br |= (1ULL << square); break;
                    case 'q': board.bq |= (1ULL << square); break;
                    case 'k': board.bk |= (1ULL << square); break;
                }
                file++;
            }
        }
    }
    if (fields.size() >= 2 && fields[1] == "b")
        board.Turn = Black;
    else
        board.Turn = White;

    if(board.wk == 0)
        board.wk = 1ULL << 4;    // default white king on e1
    if(board.bk == 0)
        board.bk = 1ULL << 60;   // default black king on e8

    return board;
}

// ----- Move Generation ----- //
vector<Move> generateMoves(const Board *b) {
    vector<Move> moves;
    Bitboard occ = occupied(*b);
    auto fileOf = [](int sq) { return sq % 8; };
    auto rankOf = [](int sq) { return sq / 8; };

    if(b->Turn == White) {
        Bitboard friendOcc = b->wp | b->wn | b->wb | b->wr | b->wq | b->wk;
        Bitboard enemyOcc = b->bp | b->bn | b->bb | b->br | b->bq | b->bk;

        // ----- White Pawn Moves (unchanged) -----
        {
            Bitboard singlePush = (b->wp << 8) & ~occ;
            while(singlePush) {
                int toSquare = trailingZeros(singlePush);
                int fromSquare = toSquare - 8;
                moves.push_back({fromSquare, toSquare});
                // Pawn double push if on starting rank (rank 2, index 1)
                if(rankOf(fromSquare) == 1) {
                    int toSquare2 = toSquare + 8;
                    if(~occ & (1ULL << toSquare2))
                        moves.push_back({fromSquare, toSquare2});
                }
                singlePush &= singlePush - 1;
            }
        }
        // Pawn captures (diagonally) using delta logic (could be replaced similarly)
        {
            Bitboard pawns = b->wp;
            while(pawns) {
                int fromSquare = trailingZeros(pawns);
                int fromFile = fileOf(fromSquare);
                int fromRank = rankOf(fromSquare);
                if(fromFile > 0) {
                    int toSquare = fromSquare + 7;
                    if(rankOf(toSquare) == fromRank + 1 && (enemyOcc & (1ULL << toSquare)))
                        moves.push_back({fromSquare, toSquare});
                }
                if(fromFile < 7) {
                    int toSquare = fromSquare + 9;
                    if(rankOf(toSquare) == fromRank + 1 && (enemyOcc & (1ULL << toSquare)))
                        moves.push_back({fromSquare, toSquare});
                }
                pawns &= pawns - 1;
            }
        }
        // ----- White Knight Moves (using attack table) -----
        {
            Bitboard knights = b->wn;
            while(knights) {
                int fromSquare = trailingZeros(knights);
                Bitboard knightMoves = knightAttacks[fromSquare] & ~friendOcc;
                while(knightMoves) {
                    int toSquare = trailingZeros(knightMoves);
                    moves.push_back({fromSquare, toSquare});
                    knightMoves &= knightMoves - 1;
                }
                knights &= knights - 1;
            }
        }
        // ----- White Bishop Moves (using attack table) -----
        {
            Bitboard bishops = b->wb;
            while(bishops) {
                int fromSquare = trailingZeros(bishops);
                Bitboard bishopMoves = bishopAttacks[fromSquare] & ~friendOcc;
                while(bishopMoves) {
                    int toSquare = trailingZeros(bishopMoves);
                    moves.push_back({fromSquare, toSquare});
                    bishopMoves &= bishopMoves - 1;
                }
                bishops &= bishops - 1;
            }
        }
        // ----- White Rook Moves (using attack table) -----
        {
            Bitboard rooks = b->wr;
            while(rooks) {
                int fromSquare = trailingZeros(rooks);
                Bitboard rookMoves = rookAttacks[fromSquare] & ~friendOcc;
                while(rookMoves) {
                    int toSquare = trailingZeros(rookMoves);
                    moves.push_back({fromSquare, toSquare});
                    rookMoves &= rookMoves - 1;
                }
                rooks &= rooks - 1;
            }
        }
        // ----- White Queen Moves (using attack table) -----
        {
            Bitboard queens = b->wq;
            while(queens) {
                int fromSquare = trailingZeros(queens);
                Bitboard queenMoves = queenAttacks[fromSquare] & ~friendOcc;
                while(queenMoves) {
                    int toSquare = trailingZeros(queenMoves);
                    moves.push_back({fromSquare, toSquare});
                    queenMoves &= queenMoves - 1;
                }
                queens &= queens - 1;
            }
        }
        // ----- White King Moves (using attack table) -----
        {
            Bitboard king = b->wk;
            if(king) {
                int fromSquare = trailingZeros(king);
                Bitboard kingMoves = kingAttacks[fromSquare] & ~friendOcc;
                while(kingMoves) {
                    int toSquare = trailingZeros(kingMoves);
                    moves.push_back({fromSquare, toSquare});
                    kingMoves &= kingMoves - 1;
                }
            }
        }
    } else {
        // ----- Black Moves -----
        Bitboard friendOcc = b->bp | b->bn | b->bb | b->br | b->bq | b->bk;
        Bitboard enemyOcc = b->wp | b->wn | b->wb | b->wr | b->wq | b->wk;
        // ----- Black Pawn Moves (unchanged) -----
        {
            Bitboard singlePush = (b->bp >> 8) & ~occ;
            while(singlePush) {
                int toSquare = trailingZeros(singlePush);
                int fromSquare = toSquare + 8;
                moves.push_back({fromSquare, toSquare});
                // Pawn double push from starting rank (rank 7, index 6)
                if(rankOf(fromSquare) == 6) {
                    int toSquare2 = toSquare - 8;
                    if(~occ & (1ULL << toSquare2))
                        moves.push_back({fromSquare, toSquare2});
                }
                singlePush &= singlePush - 1;
            }
        }
        // Pawn captures (diagonally)
        {
            Bitboard pawns = b->bp;
            while(pawns) {
                int fromSquare = trailingZeros(pawns);
                int fromFile = fileOf(fromSquare);
                int fromRank = rankOf(fromSquare);
                if(fromFile > 0) {
                    int toSquare = fromSquare - 9;
                    if(rankOf(toSquare) == fromRank - 1 && (enemyOcc & (1ULL << toSquare)))
                        moves.push_back({fromSquare, toSquare});
                }
                if(fromFile < 7) {
                    int toSquare = fromSquare - 7;
                    if(rankOf(toSquare) == fromRank - 1 && (enemyOcc & (1ULL << toSquare)))
                        moves.push_back({fromSquare, toSquare});
                }
                pawns &= pawns - 1;
            }
        }
        // ----- Black Knight Moves (using attack table) -----
        {
            Bitboard knights = b->bn;
            while(knights) {
                int fromSquare = trailingZeros(knights);
                Bitboard knightMoves = knightAttacks[fromSquare] & ~friendOcc;
                while(knightMoves) {
                    int toSquare = trailingZeros(knightMoves);
                    moves.push_back({fromSquare, toSquare});
                    knightMoves &= knightMoves - 1;
                }
                knights &= knights - 1;
            }
        }
        // ----- Black Bishop Moves (using attack table) -----
        {
            Bitboard bishops = b->bb;
            while(bishops) {
                int fromSquare = trailingZeros(bishops);
                Bitboard bishopMoves = bishopAttacks[fromSquare] & ~friendOcc;
                while(bishopMoves) {
                    int toSquare = trailingZeros(bishopMoves);
                    moves.push_back({fromSquare, toSquare});
                    bishopMoves &= bishopMoves - 1;
                }
                bishops &= bishops - 1;
            }
        }
        // ----- Black Rook Moves (using attack table) -----
        {
            Bitboard rooks = b->br;
            while(rooks) {
                int fromSquare = trailingZeros(rooks);
                Bitboard rookMoves = rookAttacks[fromSquare] & ~friendOcc;
                while(rookMoves) {
                    int toSquare = trailingZeros(rookMoves);
                    moves.push_back({fromSquare, toSquare});
                    rookMoves &= rookMoves - 1;
                }
                rooks &= rooks - 1;
            }
        }
        // ----- Black Queen Moves (using attack table) -----
        {
            Bitboard queens = b->bq;
            while(queens) {
                int fromSquare = trailingZeros(queens);
                Bitboard queenMoves = queenAttacks[fromSquare] & ~friendOcc;
                while(queenMoves) {
                    int toSquare = trailingZeros(queenMoves);
                    moves.push_back({fromSquare, toSquare});
                    queenMoves &= queenMoves - 1;
                }
                queens &= queens - 1;
            }
        }
        // ----- Black King Moves (using attack table) -----
        {
            Bitboard king = b->bk;
            if(king) {
                int fromSquare = trailingZeros(king);
                Bitboard kingMoves = kingAttacks[fromSquare] & ~friendOcc;
                while(kingMoves) {
                    int toSquare = trailingZeros(kingMoves);
                    moves.push_back({fromSquare, toSquare});
                    kingMoves &= kingMoves - 1;
                }
            }
        }
    }
    return moves;
}

// ----- Apply Move ----- //
Board applyMove(Board b, const Move &move) {
    Bitboard fromMask = 1ULL << move.from;
    Bitboard toMask   = 1ULL << move.to;
    
    if(b.Turn == White) {
        // Remove any captured black piece.
        if(b.bp & toMask) b.bp &= ~toMask;
        if(b.bn & toMask) b.bn &= ~toMask;
        if(b.bb & toMask) b.bb &= ~toMask;
        if(b.br & toMask) b.br &= ~toMask;
        if(b.bq & toMask) b.bq &= ~toMask;
        if(b.bk & toMask) b.bk &= ~toMask;
        
        // Move the white piece.
        if(b.wp & fromMask) {
            b.wp &= ~fromMask;
            b.wp |= toMask;
        } else if(b.wn & fromMask) {
            b.wn &= ~fromMask;
            b.wn |= toMask;
        } else if(b.wb & fromMask) {
            b.wb &= ~fromMask;
            b.wb |= toMask;
        } else if(b.wr & fromMask) {
            b.wr &= ~fromMask;
            b.wr |= toMask;
        } else if(b.wq & fromMask) {
            b.wq &= ~fromMask;
            b.wq |= toMask;
        } else if(b.wk & fromMask) {
            b.wk &= ~fromMask;
            b.wk |= toMask;
        }
    } else {
        // Remove any captured white piece.
        if(b.wp & toMask) b.wp &= ~toMask;
        if(b.wn & toMask) b.wn &= ~toMask;
        if(b.wb & toMask) b.wb &= ~toMask;
        if(b.wr & toMask) b.wr &= ~toMask;
        if(b.wq & toMask) b.wq &= ~toMask;
        if(b.wk & toMask) b.wk &= ~toMask;
        
        // Move the black piece.
        if(b.bp & fromMask) {
            b.bp &= ~fromMask;
            b.bp |= toMask;
        } else if(b.bn & fromMask) {
            b.bn &= ~fromMask;
            b.bn |= toMask;
        } else if(b.bb & fromMask) {
            b.bb &= ~fromMask;
            b.bb |= toMask;
        } else if(b.br & fromMask) {
            b.br &= ~fromMask;
            b.br |= toMask;
        } else if(b.bq & fromMask) {
            b.bq &= ~fromMask;
            b.bq |= toMask;
        } else if(b.bk & fromMask) {
            b.bk &= ~fromMask;
            b.bk |= toMask;
        }
    }
    
    // Change turn.
    b.Turn = (b.Turn == White) ? Black : White;
    return b;
}

// ----- Evaluation Functions ----- //
int evaluatePiece(Bitboard bb, int value, const array<int, 64> &table) {
    int score = 0;
    while(bb) {
        int sq = trailingZeros(bb);
        score += value + table[sq];
        bb &= bb - 1;
    }
    return score;
}

int evaluateKing(Bitboard bb, bool endgame) {
    if(bb == 0) {
        // King missing: assign huge penalty to signal checkmate.
        return eventValues["checkmate"];
    }
    if(bb & (bb - 1)) {
        cerr << "Warning: king bitboard has multiple bits set" << endl;
    }
    int sq = trailingZeros(bb);
    return endgame ? kingEndgameTable[sq] : kingOpeningTable[sq];
}

int evaluateBoard(const Board *b) {
    int score = 0;
    // Count non-king pieces for endgame detection.
    int nonKingCount = popCount(b->wp) + popCount(b->wn) + popCount(b->wb) +
                       popCount(b->wr) + popCount(b->wq) +
                       popCount(b->bp) + popCount(b->bn) + popCount(b->bb) +
                       popCount(b->br) + popCount(b->bq);
    bool endgame = (nonKingCount <= 10);
    
    // Evaluate white pieces.
    score += evaluatePiece(b->wp, pieceValues["p"], pawnTable);
    score += evaluatePiece(b->wn, pieceValues["n"], knightTable);
    score += evaluatePiece(b->wb, pieceValues["b"], bishopTable);
    score += evaluatePiece(b->wr, pieceValues["r"], rookTable);
    score += evaluatePiece(b->wq, pieceValues["q"], queenTable);
    score += evaluateKing(b->wk, endgame);
    
    // Evaluate black pieces (subtract).
    score -= evaluatePiece(b->bp, pieceValues["p"], pawnTable);
    score -= evaluatePiece(b->bn, pieceValues["n"], knightTable);
    score -= evaluatePiece(b->bb, pieceValues["b"], bishopTable);
    score -= evaluatePiece(b->br, pieceValues["r"], rookTable);
    score -= evaluatePiece(b->bq, pieceValues["q"], queenTable);
    score -= evaluateKing(b->bk, endgame);
    
    // For simplicity, event-based scoring (check, checkmate, castling, fork) is omitted.
    return score;
}

// ----- Minimax and Best Move ----- //
int minimax(Board b, int depth, int alpha, int beta, bool maximizing) {
    if(depth == 0) {
        return evaluateBoard(&b);
    }
    // Generate moves.
    vector<Move> moves = generateMoves(&b);
    // Order moves based on a simple static evaluation.
    vector<pair<int, Move>> scoredMoves;
    for (auto &m : moves) {
        Board newB = applyMove(b, m);
        int score = evaluateBoard(&newB);
        scoredMoves.push_back({score, m});
    }
    if(maximizing) {
        sort(scoredMoves.begin(), scoredMoves.end(), [](auto &a, auto &b) {
            return a.first > b.first;
        });
    } else {
        sort(scoredMoves.begin(), scoredMoves.end(), [](auto &a, auto &b) {
            return a.first < b.first;
        });
    }
    
    if(maximizing) {
        int best = -1000000;
        for(auto &entry : scoredMoves) {
            Board newB = applyMove(b, entry.second);
            int val = minimax(newB, depth - 1, alpha, beta, false);
            best = max(best, val);
            alpha = max(alpha, best);
            if(beta <= alpha)
                break;
        }
        return best;
    } else {
        int best = 1000000;
        for(auto &entry : scoredMoves) {
            Board newB = applyMove(b, entry.second);
            int val = minimax(newB, depth - 1, alpha, beta, true);
            best = min(best, val);
            beta = min(beta, best);
            if(beta <= alpha)
                break;
        }
        return best;
    }
}

Move findBestMove(Board b, int depth) {
    int bestScore = -1000000;
    Move bestMove{0, 0};
    vector<Move> moves = generateMoves(&b);
    
    // Order the moves at the root.
    vector<pair<int, Move>> scoredMoves;
    for(auto &m : moves) {
        Board newB = applyMove(b, m);
        int score = evaluateBoard(&newB);
        scoredMoves.push_back({score, m});
    }
    sort(scoredMoves.begin(), scoredMoves.end(), [](auto &a, auto &b) {
        return a.first > b.first;  // highest score first for maximizing
    });
    
    int alpha = -1000000, beta = 1000000;
    for(auto &entry : scoredMoves) {
        Board newB = applyMove(b, entry.second);
        int score = minimax(newB, depth - 1, alpha, beta, false);
        if(score > bestScore) {
            bestScore = score;
            bestMove = entry.second;
        }
        alpha = max(alpha, bestScore);
        if(beta <= alpha)
            break;
    }
    return bestMove;
}

// ----- Main ----- //
int main(int argc, char *argv[]) {
    if(argc != 3) {
        cout << "Usage: \"<FEN>\" <depth>" << endl;
        return 1;
    }
    string fen = argv[1];
    int depth = stoi(argv[2]);
    
    // Check DB for matching move if available.
    vector<string> possibleDBMoves;
    istringstream dbStream(db);
    string line;
    while(getline(dbStream, line)) {
        if(line.empty() || line[0] == '*')
            continue;
        // If the line begins with the FEN then parse the move part.
        if(line.find(fen) == 0) {
            size_t pos = line.find(" : ");
            if(pos != string::npos && pos+3 < line.size()) {
                possibleDBMoves.push_back(line.substr(pos+3));
            }
        }
    }
    if(!possibleDBMoves.empty()) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, possibleDBMoves.size()-1);
        cout << possibleDBMoves[dis(gen)] << endl;
        return 0;
    }
    
    Board board = parseFEN(fen);
    Move bestMove = findBestMove(board, depth);
    cout << bestMove.toString() << endl;
    return 0;
}
