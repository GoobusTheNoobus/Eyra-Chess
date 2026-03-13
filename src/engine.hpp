#pragma once
#include "type.hpp"
#include "position.hpp"
#include "movegen.hpp"
#include <chrono>
#include <atomic>
#include <cstring>
#include "misc.hpp"
#include "uci.hpp"

using namespace std::chrono;

namespace Eyra {



struct SearchInfo {
    steady_clock::time_point start_time;
    uint64_t nodes = 0;
    std::atomic<bool> stop{false};
    int depth = 0;
    int max_time_ms = 0;
    

    inline void Reset () {
        depth = 0;
        nodes = 0;
        max_time_ms = 0;
        stop.store(false, std::memory_order_relaxed);
    };

};

struct SearchResults {
    Move best_move = 0;
    int score = 0;
};

namespace Engine {

    constexpr int MAX_DEPTH = 32;
    constexpr int KILLERS_PER_DEPTH = 2;

    extern SearchInfo search_info;
    extern Position position;
    extern Move killers[MAX_DEPTH][KILLERS_PER_DEPTH]; // Store 2 killers per each depth
    extern Move history[COLORS][BOARD_SIZE][BOARD_SIZE]; // For History Heuristics: store moves that has previously caused beta cutoff
    

    inline void ResetKillers () {
        std::memset(killers, 0, sizeof(killers));
    }

    inline void StoreKiller (Move move, int ply) {
        // If killer already exist, there is no need to store it.
        if (killers[ply][0] == move) return;
        if (killers[ply][1] == move) return;

        killers[ply][1] = killers[ply][0];
        killers[ply][0] = move;
    }
    
    int Evaluate (Position& pos);
    float EGWeight (Position& pos);

    int Search (Position& pos, int depth, int alpha, int beta, bool can_null_prune);
    int QSearch (Position& pos, int depth, int alpha, int beta);

    SearchResults GetBestMove (Position& pos, int depth, Move pv);
    void Go (int depth_limit, int movetime);

} // namespace Engine

    

} // namespace Eyra
