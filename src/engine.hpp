#pragma once
#include "type.hpp"
#include "position.hpp"
#include "movegen.hpp"
#include <chrono>
#include <atomic>

using namespace std::chrono;

namespace Eyra {

constexpr int MAX_CP = 10000;
constexpr int INF    = 30001;
constexpr int MATE_EVAL = 30000;

struct SearchInfo {
    int depth = 0;
    uint64_t nodes = 0;

    steady_clock::time_point start_time;
    int max_time_ms = 0;

    std::atomic<bool> stop {false};

    inline void Reset () {
        depth = 0;
        nodes = 0;
        max_time_ms = 0;
    };

};

struct SearchResults {
    Move best_move = 0;
    int score = 0;
};

namespace Engine {
    extern SearchInfo search_info;
    extern Position pos;
    
    int Evaluate (Position& pos);
    float EGWeight (Position& pos);

    int Search (Position& pos, int depth, int alpha, int beta);
    int QSearch (Position& pos, int depth, int alpha, int beta);

    SearchResults GetBestMove (Position& pos, int depth, Move pv);

    void Go (int depth_limit, int movetime);
} // namespace Engine

    

} // namespace Eyra
