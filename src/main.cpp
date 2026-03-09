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

    constexpr int depth = 14;

    while (true) {
        std::string str;
        std::getline(std::cin, str);

        Engine::position.ParseFEN(str);

        auto start = std::chrono::steady_clock::now();
        Engine::Go(depth, 1000);
        auto end = std::chrono::steady_clock::now();
        
    }

    

    return 0;
}


