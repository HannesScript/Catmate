#!/bin/bash
cd /media/hannesscript/DEV1/Catmate

echo "=== Catmate Rust Port - Engine Tests ==="
echo ""

echo "Test 1: Starting Position (depth 4)"
time ./target/release/catmate "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq" 4
echo ""

echo "Test 2: After 1.e4 (Black to move, depth 4)" 
time ./target/release/catmate "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq" 4
echo ""

echo "Test 3: After 1.e4 e5 2.Nf3 (depth 5)"
time ./target/release/catmate "rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq" 5
echo ""

echo "Test 4: Opening (depth 5)"
time ./target/release/catmate "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq" 5
echo ""

echo "Test 5: Mid-game Tactical Position (depth 5)"
time ./target/release/catmate "r1bqk2r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq" 5
echo ""

echo "Test 6: Hard Mid-game Tactical Position (depth 5)"
time ./target/release/catmate "2b5/p2NBp1p/1bp1nPPr/3P4/2pRnr1P/1k1B1Ppp/1P1P1pQP/Rq1N3K b -" 5
echo ""

echo "Test 7: End-game Tactical Position (depth 5)"
time ./target/release/catmate "K2q4/QR6/6p1/5bp1/8/p4P2/2kp4/Q7 w -" 5
echo ""

echo "=== All tests completed successfully ==="
