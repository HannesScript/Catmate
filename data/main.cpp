/*
This Program reads ./db.cdb and converts it to the type:
    unordered_map<uint64_t, vector<string>>
with the uint64_t being a Zobrist hash.

Example Input:
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : d2d4
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : e2e4

Example Output:
{{3899691870434340789ULL, {"f8b4"}}};
*/

#include "util.cpp"
#include "zobrist.cpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <algorithm>

using namespace std;

int main() {
        // Initialize Zobrist table.
        initZobrist();

        // Mapping: Zobrist hash -> sequence of moves.
        unordered_map<uint64_t, vector<string>> openings;

        ifstream file("db.cdb");
        if (!file) {
                cerr << "Error opening db.cdb" << endl;
                return 1;
        }

        string line;
        while (getline(file, line)) {
                // Skip empty lines or comment lines starting with '*'
                if (line.empty() || line[0] == '*')
                        continue;

                // Find the colon separator.
                size_t colonPos = line.find(':');
                if (colonPos == string::npos)
                        continue;

                // Extract the FEN part and the move string.
                string fen = line.substr(0, colonPos);
                string move = line.substr(colonPos + 1);

                // Trim whitespace.
                auto trim = [](string &s) {
                        s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
                                return !isspace(ch);
                        }));
                        s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                                return !isspace(ch);
                        }).base(), s.end());
                };
                trim(fen);
                trim(move);

                // Parse the board from FEN and compute the Zobrist hash.
                Board board = parseFEN(fen);
                uint64_t hash = computeZobrist(board);

                // Avoid duplicate moves.
                auto &moves = openings[hash];
                if (find(moves.begin(), moves.end(), move) == moves.end())
                        moves.push_back(move);
        }
        file.close();

        // Output the mapping in the desired format.
        cout << "{";
        bool firstEntry = true;
        for (const auto &entry : openings) {
                if (!firstEntry)
                        cout << ", ";
                firstEntry = false;
                cout << "{" << entry.first << "ULL, {";
                bool firstMove = true;
                for (const auto &m : entry.second) {
                        if (!firstMove)
                                cout << ", ";
                        firstMove = false;
                        cout << "\"" << m << "\"";
                }
                cout << "}}";
        }
        cout << "};" << endl;

        return 0;
}