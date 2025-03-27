#pragma once

#include <bits/stdc++.h>

#include "board.cpp"
#include "zobrist.cpp"
#include "evaluate.cpp"
#include "move_gen.cpp"

using namespace std;
using Bitboard = uint64_t;
using Clock = chrono::steady_clock;

// Inline helper functions.
inline int encodeMove(const Move &move) { return move.from * 64 + move.to; }
inline bool timeExceeded(const Clock::time_point &start, int maxTime)
{
    return chrono::duration_cast<chrono::milliseconds>(Clock::now() - start).count() >= maxTime;
}

const int MAX_DEPTH = 100; // maximum search depth for killer move storage
string ttPath;

// Global timing variables.
Clock::time_point searchStart;
int maxTime = 10'000; // default time limit in milliseconds

// Global killer moves and history heuristic tables.
vector<array<Move, 2>> killerMoves(MAX_DEPTH);
unordered_map<int, int> historyHeuristic;

// Score a move using killer move and history heuristic data.
inline int scoreMove(const Move &m, int depth)
{
    int score = 0;
    if (depth < MAX_DEPTH)
    {
        if (m == killerMoves[depth][0])
            score += 10000;
        else if (m == killerMoves[depth][1])
            score += 5000;
    }
    score += historyHeuristic[encodeMove(m)];
    return score;
}

// ----- Minimax with Time Management ----- //
int minimax(Board b, int depth, int alpha, int beta, bool maximizing)
{
    // Check for time every call.
    if (timeExceeded(searchStart, maxTime))
        return evaluateBoard(&b);

    initZobrist();
    uint64_t hash = computeZobrist(b);
    auto it = transpositionTable.find(hash);
    if (it != transpositionTable.end())
    {
        TTEntry entry = it->second;
        if (entry.depth >= depth)
        {
            if (entry.flag == EXACT)
                return entry.value;
            else if (entry.flag == LOWERBOUND)
                alpha = max(alpha, entry.value);
            else if (entry.flag == UPPERBOUND)
                beta = min(beta, entry.value);
            if (alpha >= beta)
                return entry.value;
        }
    }

    if (depth == 0)
    {
        return evaluateBoard(&b);
    }

    vector<Move> moves = generateMoves(&b);
    vector<pair<int, Move>> scoredMoves;
    for (const auto &m : moves)
        scoredMoves.push_back({scoreMove(m, depth), m});

    sort(scoredMoves.begin(), scoredMoves.end(), [](auto &a, auto &b)
         { return a.first > b.first; });

    int best = maximizing ? -1000000 : 1000000;
    for (const auto &entry : scoredMoves)
    {
        Board newB = applyMove(&b, entry.second);
        int val = minimax(newB, depth - 1, alpha, beta, !maximizing);
        if (maximizing)
        {
            best = max(best, val);
            alpha = max(alpha, best);
        }
        else
        {
            best = min(best, val);
            beta = min(beta, best);
        }
        if (beta <= alpha)
        {
            int moveId = encodeMove(entry.second);
            historyHeuristic[moveId] += depth * depth;
            if (depth < MAX_DEPTH && !(killerMoves[depth][0] == entry.second))
            {
                killerMoves[depth][1] = killerMoves[depth][0];
                killerMoves[depth][0] = entry.second;
            }
            break; // beta cutoff.
        }
    }

    return best;
}

// ----- Parallel Best Move ----- //
Move findBestMove(Board b, int fixedDepth)
{
    Move bestMove{0, 0};
    int bestScore = -1000000;

    vector<Move> moves = generateMoves(&b);
    vector<pair<int, Move>> scoredMoves;
    for (const auto &m : moves)
        scoredMoves.push_back({scoreMove(m, fixedDepth), m});
    sort(scoredMoves.begin(), scoredMoves.end(), [](auto &a, auto &b)
         { return a.first > b.first; });

    unsigned int maxConcurrent = thread::hardware_concurrency();
    if (maxConcurrent == 0)
        maxConcurrent = 2; // default if hardware_concurrency() is not available
        
    // Process moves in parallel batches.
    for (size_t i = 0; i < scoredMoves.size(); i += maxConcurrent)
    {
        vector<future<int>> futures;
        vector<Move> moveBatch;
        for (size_t j = i; j < scoredMoves.size() && j < i + maxConcurrent; j++)
        {
            Board newB = applyMove(&b, scoredMoves[j].second);
            futures.push_back(async(launch::async, [newB, fixedDepth]() -> int
                                    { return minimax(newB, fixedDepth, -1000000, 1000000, false); }));
            moveBatch.push_back(scoredMoves[j].second);
        }
        for (size_t k = 0; k < futures.size(); k++)
        {
            int score = futures[k].get();
            if (score > bestScore)
            {
                bestScore = score;
                bestMove = moveBatch[k];
            }
        }
    }

    if (!ttPath.empty())
    {
        initZobrist();
        uint64_t hash = computeZobrist(b);
        if (transpositionTable.find(hash) == transpositionTable.end())
        {
            TTEntry entry = {fixedDepth, bestScore, EXACT, bestMove.toString()};
            transpositionTable[hash] = entry;
            storeTranspositionTable(transpositionTable, ttPath);
        }
    }
    return bestMove;
}
