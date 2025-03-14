#ifndef TRANSPOSITION
#define TRANSPOSITION

#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>

// Transposition Table entry flag types.
enum TTFlag
{
    EXACT,
    LOWERBOUND,
    UPPERBOUND
};

struct TTEntry
{
    int depth;            // depth at which this entry was stored
    int value;            // evaluation score
    TTFlag flag;          // flag type: exact, alpha (lowerbound) or beta (upperbound)
    std::string bestMove; // best move in UCIX (UCI Extended) format
};

std::unordered_map<uint64_t, TTEntry> transpositionTable;

#endif
