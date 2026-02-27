#include <iostream>
#include <chrono>

#include "misc.hpp"
#include "position.hpp"
#include "bitboard.hpp"
#include "type.hpp"

using namespace Eyra;

int main() {
    Bitboards::Init();
    
    std::cout << ENGINE_NAME << ENGINE_VERSION << " by " << ENGINE_AUTHOR << std::endl;
    
    
    return 0;

}