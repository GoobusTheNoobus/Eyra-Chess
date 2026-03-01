#include <iostream>
#include <chrono>
#include "misc.hpp"
#include "bitboard.hpp"
#include "movegen.hpp"

using namespace Eyra;

int main() {
    Eyra::Bitboards::Init();

    std::cout << Eyra::ENGINE_NAME << Eyra::ENGINE_VERSION << " by " << Eyra::ENGINE_AUTHOR << std::endl;

    Eyra::Position pos("rn2kbnr/p3pppp/b1p5/q7/Ppp1P3/4BN2/1PQ2PPP/RN2KB1R w KQkq - 4 10");

    constexpr int ITERATIONS = 10'000'000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; i++) {
        MoveGen::GenerateMoves(pos);
    }

    

    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << std::boolalpha << pos.IsAttacked(C4, WHITE);
    Eyra::MoveList list;
    Eyra::MoveGen::GenerateMoves(pos, list);
    std::cout << list;

    std::cout << ITERATIONS << " iterations in " << ms << "ms\n";
    std::cout << (ms / ITERATIONS * 1000000) << " ns/call\n";
    std::cout << (ITERATIONS / (ms / 1000) / 1'000'000) << "M calls/sec\n";
}