#include <iostream>
#include <chrono>
#include "misc.hpp"
#include "bitboard.hpp"
#include "movegen.hpp"
#include "engine.hpp"

using namespace Eyra;



int main () {
    Bitboards::Init();

    std::cout << ENGINE_NAME << ENGINE_VERSION << " by " << ENGINE_AUTHOR << std::endl;

    Position pos;
    constexpr int depth = 6;
    std::cout << "Analyzing at depth " << depth << std::endl;

    auto start = std::chrono::steady_clock::now();
    auto result = Engine::GetBestMove(pos, depth, 0);
    auto end = std::chrono::steady_clock::now();
    double ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Duration: " << ms << "ms" << std::endl;
    std::cout << "NPS: " << static_cast<int>(Engine::search_info.nodes / ms * 1000) << std::endl;
    std::cout << "Best Move: " << MoveToString(result.best_move) << std::endl;
    std::cout << "Score (side to move): " << result.score << std::endl;

    return 0;
}




//d2d3 : 328515
//b7b5 : 15701
