#ifndef DATABASE
#define DATABASE

#include <string>
using namespace std;

const string db = R"(
* //////////////////////////////////////////
* White openings
* //////////////////////////////////////////

* Queen's Gambit
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : d2d4
rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq : d7d5
rnbqkbnr/ppp1pppp/8/3p4/3P4/8/PPP1PPPP/RNBQKBNR w KQkq : c2c4

* King's Gambit
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : e2e4
rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq : e7e5
rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq : f2f4

* Italian Game
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : e2e4
rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq : e7e5
rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq : g1f3
rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq : b8c6
r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq : f1c4

* Ruy Lopez (aka. Spanish Opening)
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : e2e4
rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq : e7e5
rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq : g1f3
rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq : b8c6
r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq : f1b5

* //////////////////////////////////////////
* Black openings
* //////////////////////////////////////////

* Caro-Kann Defense
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : e2e4
rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq : c7c6
rnbqkbnr/pp1ppppp/2p5/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq : d2d4
rnbqkbnr/pp1ppppp/2p5/8/3PP3/8/PPP2PPP/RNBQKBNR b KQkq : d7d5
* exchange variation
rnbqkbnr/pp2pppp/2p5/3p4/3PP3/8/PPP2PPP/RNBQKBNR w KQkq : e4d5
rnbqkbnr/pp2pppp/2p5/3P4/3P4/8/PPP2PPP/RNBQKBNR b KQkq : c6d5
rnbqkbnr/pp2pppp/8/3p4/3P4/8/PPP2PPP/RNBQKBNR w KQkq : f1b5
rnbqkbnr/pp2pppp/8/1B1p4/3P4/8/PPP2PPP/RNBQK1NR b KQkq : b8c6
r1bqkbnr/pp2pppp/2n5/1B1p4/3P4/8/PPP2PPP/RNBQK1NR w KQkq : b5c6
r1bqkbnr/pp2pppp/2B5/3p4/3P4/8/PPP2PPP/RNBQK1NR b KQkq : b7c6
r1bqkbnr/p3pppp/2p5/3p4/3P4/8/PPP2PPP/RNBQK1NR w KQkq : g1f3
r1bqkbnr/p3pppp/2p5/3p4/3P4/5N2/PPP2PPP/RNBQK2R b KQkq : e7e6
r1bqkbnr/p4ppp/2p1p3/3p4/3P4/5N2/PPP2PPP/RNBQK2R w KQkq : e1g1
r1bqkbnr/p4ppp/2p1p3/3p4/3P4/5N2/PPP2PPP/RNBQ1RK1 b kq : f8d6
r1bqk1nr/p4ppp/2pbp3/3p4/3P4/5N2/PPP2PPP/RNBQ1RK1 w kq : h2h4
r1bqk1nr/p4ppp/2pbp3/3p4/3P3P/5N2/PPP2PP1/RNBQ1RK1 b kq : g8f6
r1bqk2r/p4ppp/2pbpn2/3p4/3P3P/5N2/PPP2PP1/RNBQ1RK1 w kq : f1e1
r1bqk2r/p4ppp/2pbpn2/3p4/3P3P/5N2/PPP2PP1/RNBQR1K1 b kq : e8f8
r1bq1rk1/p4ppp/2pbpn2/3p4/3P3P/5N2/PPP2PP1/RNBQR1K1 w - : b2b3
* Knight push by white
rnbqkbnr/pp1ppppp/2p5/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq : g1f3
rnbqkbnr/pp1ppppp/2p5/8/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq : d7d5
* exchange variation
rnbqkbnr/pp2pppp/2p5/3p4/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq : e4d5
rnbqkbnr/pp2pppp/2p5/3P4/8/5N2/PPPP1PPP/RNBQKB1R b KQkq : c6d5
rnbqkbnr/pp2pppp/8/3p4/8/5N2/PPPP1PPP/RNBQKB1R w KQkq : f1b5
rnbqkbnr/pp2pppp/8/1B1p4/8/5N2/PPPP1PPP/RNBQK2R b KQkq : b8c6
r1bqkbnr/pp2pppp/2n5/1B1p4/8/5N2/PPPP1PPP/RNBQK2R w KQkq : b5c6
r1bqkbnr/pp2pppp/2B5/3p4/8/5N2/PPPP1PPP/RNBQK2R b KQkq : b7c6
r1bqkbnr/p3pppp/2p5/3p4/8/5N2/PPPP1PPP/RNBQK2R w KQkq : e1g1
r1bqkbnr/p3pppp/2p5/3p4/8/5N2/PPPP1PPP/RNBQ1RK1 b kq : e7e6

* Scandinavian Defense
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : e2e4
rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq : d7d5
rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq : e4d5

* French Defense
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : e2e4
rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq : e7e6

* Sicilian Defense
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : e2e4
rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq : c7c5

* Nimzo-Indian Defense
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : d2d4
rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq : g8f6
rnbqkb1r/pppppppp/5n2/8/3P4/8/PPP1PPPP/RNBQKBNR w KQkq : c2c4
rnbqkb1r/pppppppp/5n2/8/2PP4/8/PP2PPPP/RNBQKBNR b KQkq : e7e6
rnbqkb1r/pppp1ppp/4pn2/8/2PP4/8/PP2PPPP/RNBQKBNR w KQkq : b1c3
rnbqkb1r/pppp1ppp/4pn2/8/2PP4/2N5/PP2PPPP/R1BQKBNR b KQkq : f8b4

* King's Indian Defense
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq : d2d4
rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq : g8f6
rnbqkb1r/pppppppp/5n2/8/3P4/8/PPP1PPPP/RNBQKBNR w KQkq : c2c4
rnbqkb1r/pppppppp/5n2/8/2PP4/8/PP2PPPP/RNBQKBNR b KQkq : g7g6
)";

#endif
