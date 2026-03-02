#include <iostream>
#include <chrono>
#include "misc.hpp"
#include "bitboard.hpp"
#include "movegen.hpp"

using namespace Eyra;

int main() {
    Eyra::Bitboards::Init();

    std::cout << Eyra::ENGINE_NAME << Eyra::ENGINE_VERSION << " by " << Eyra::ENGINE_AUTHOR << std::endl;

    Eyra::Position pos("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 6 7");

    constexpr int ITERATIONS = 10'000'000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; i++) {
        MoveGen::GenerateMoves(pos);
    }

    

    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    
    Eyra::MoveList list;
    Eyra::MoveGen::GenerateMoves(pos, list);
    std::cout << list;

    std::cout << ITERATIONS << " iterations in " << ms << "ms\n";
    std::cout << (ms / ITERATIONS * 1000000) << " ns/call\n";
    std::cout << (ITERATIONS / (ms / 1000) / 1'000'000) << "M calls/sec\n";
}