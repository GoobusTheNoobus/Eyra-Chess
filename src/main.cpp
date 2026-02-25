#include <iostream>

#include "misc.hpp"
#include "position.hpp"

int main() {
    std::cout << Beans::ENGINE_NAME << Beans::ENGINE_VERSION << " by " << Beans::ENGINE_AUTHOR << std::endl;
    
    Beans::Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    

    std::cout << pos.ToString();

    return 0;

}