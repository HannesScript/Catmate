#pragma once

#include <iostream>
#include <vector>
#include <tuple>

#include "../board.cpp"

using namespace std;

class GameHistory
{
private:
    vector<tuple<Move, int, vector<pair<Move, int>>>> hist;

public:
    GameHistory() {};
    ~GameHistory() {};

    void push(Move *move, int eval, vector<pair<Move, int>> further) {
        hist.push_back(make_tuple(*move, eval, further));
    };

    tuple<Move, int, vector<pair<Move, int>>> getLast() {
        if (hist.empty())
            return tuple<Move, int, vector<pair<Move, int>>>{};

        auto lastEntry = hist.back();
        hist.pop_back();
        return lastEntry;
    };
};
